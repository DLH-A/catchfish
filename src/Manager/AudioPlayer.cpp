#include "AudioPlayer.h"
#include <algorithm>
#include <cctype>
#include <cmath>
#include <filesystem>
#include <utility>

AudioPlayer::AudioPlayer(size_t n) : maxVoices_(std::clamp<size_t>(n, 1, 1024))
{
}
AudioPlayer::~AudioPlayer()
{
    Shutdown();
}
bool AudioPlayer::fail(const std::string &s)
{
    error_ = s;
    TraceLog(LOG_WARNING, "AudioPlayer: %s", s.c_str());
    return false;
}
AudioPlayer::Voice *AudioPlayer::find(uint64_t id)
{
    for (auto &v : voices_)
        if (v.id == id)
            return &v;
    return nullptr;
}
void AudioPlayer::release(Voice &v)
{
    if (IsAudioDeviceReady() && v.alias.frameCount)
    {
        ::StopSound(v.alias);
        UnloadSoundAlias(v.alias);
    }
    v.alias = {};
    v.source.reset();
}
uint64_t AudioPlayer::PlaySound(SoundHandle source, float volume, float pitch, bool loop)
{
    if (!IsAudioDeviceReady() || !source || !IsSoundValid(source->sound) || !std::isfinite(volume) ||
        !std::isfinite(pitch) || pitch <= 0)
    {
        fail("Invalid sound/device/volume/pitch");
        return 0;
    }
    Update();
    if (voices_.size() >= maxVoices_)
    {
        release(voices_.front());
        voices_.erase(voices_.begin());
    }
    Voice v;
    v.alias = LoadSoundAlias(source->sound);
    if (!IsSoundValid(v.alias))
    {
        fail("LoadSoundAlias failed");
        return 0;
    }
    v.id = nextId_++;
    v.source = std::move(source);
    v.loop = loop;
    ::SetSoundVolume(v.alias, std::clamp(volume, 0.0f, 1.0f));
    ::SetSoundPitch(v.alias, pitch);
    ::PlaySound(v.alias);
    voices_.push_back(std::move(v));
    error_.clear();
    return voices_.back().id;
}
bool AudioPlayer::StopSound(uint64_t id)
{
    for (auto it = voices_.begin(); it != voices_.end(); ++it)
        if (it->id == id)
        {
            release(*it);
            voices_.erase(it);
            return true;
        }
    return false;
}
bool AudioPlayer::IsSoundPlaying(uint64_t id) const
{
    if (!IsAudioDeviceReady())
        return false;
    for (const auto &v : voices_)
        if (v.id == id)
            return ::IsSoundPlaying(v.alias);
    return false;
}
bool AudioPlayer::SetSoundVolume(uint64_t id, float v)
{
    auto p = find(id);
    if (!p || !IsAudioDeviceReady() || !std::isfinite(v))
        return false;
    ::SetSoundVolume(p->alias, std::clamp(v, 0.0f, 1.0f));
    return true;
}
bool AudioPlayer::SetSoundPitch(uint64_t id, float v)
{
    auto p = find(id);
    if (!p || !IsAudioDeviceReady() || !std::isfinite(v) || v <= 0)
        return false;
    ::SetSoundPitch(p->alias, v);
    return true;
}
void AudioPlayer::StopAllSounds()
{
    for (auto &v : voices_)
        release(v);
    voices_.clear();
}
bool AudioPlayer::PlayMusic(const std::string &path, bool loop, float volume)
{
    if (!IsAudioDeviceReady() || !std::isfinite(volume))
        return fail("Music requires audio device and finite volume");
    auto ext = std::filesystem::u8path(path).extension().u8string();
    for (char &c : ext)
        c = char(std::tolower((unsigned char)c));
    if (ext != ".ogg" && ext != ".wav")
        return fail("Music supports OGG/WAV only; convert MO3 externally: " + path);
    Music next = LoadMusicStream(path.c_str());
    if (!IsMusicValid(next))
        return fail("Cannot decode music: " + path);
    StopMusic();
    music_ = next;
    musicLoaded_ = musicActive_ = true;
    music_.looping = loop;
    ::SetMusicVolume(music_, std::clamp(volume, 0.0f, 1.0f));
    PlayMusicStream(music_);
    error_.clear();
    return true;
}
void AudioPlayer::PauseMusic()
{
    if (musicLoaded_ && IsAudioDeviceReady())
        PauseMusicStream(music_);
}
void AudioPlayer::ResumeMusic()
{
    if (musicLoaded_ && musicActive_ && IsAudioDeviceReady())
        ResumeMusicStream(music_);
}
void AudioPlayer::StopMusic()
{
    if (musicLoaded_ && IsAudioDeviceReady())
    {
        StopMusicStream(music_);
        UnloadMusicStream(music_);
    }
    music_ = {};
    musicLoaded_ = musicActive_ = false;
}
bool AudioPlayer::IsMusicPlaying() const
{
    return musicLoaded_ && IsAudioDeviceReady() && IsMusicStreamPlaying(music_);
}
bool AudioPlayer::SetMusicVolume(float volume)
{
    if (!musicLoaded_ || !IsAudioDeviceReady() || !std::isfinite(volume))
        return false;
    ::SetMusicVolume(music_, std::clamp(volume, 0.0f, 1.0f));
    return true;
}
void AudioPlayer::Update()
{
    if (!IsAudioDeviceReady())
        return;
    for (auto it = voices_.begin(); it != voices_.end();)
    {
        if (!::IsSoundPlaying(it->alias))
        {
            if (it->loop)
            {
                ::PlaySound(it->alias);
                ++it;
            }
            else
            {
                release(*it);
                it = voices_.erase(it);
            }
        }
        else
            ++it;
    }
    if (musicLoaded_)
        UpdateMusicStream(music_);
}
void AudioPlayer::Shutdown()
{
    StopAllSounds();
    StopMusic();
    error_.clear();
}
