#pragma once
#include "SoundResource.h"
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

class AudioPlayer
{
  public:
    explicit AudioPlayer(size_t maxVoices = 32);
    ~AudioPlayer();
    AudioPlayer(const AudioPlayer &) = delete;
    AudioPlayer &operator=(const AudioPlayer &) = delete;
    // Returns 0 on failure. Handles are monotonically increasing, never reused.
    uint64_t PlaySound(SoundHandle sound, float volume = 1, float pitch = 1, bool loop = false);
    bool StopSound(uint64_t id);
    bool IsSoundPlaying(uint64_t id) const;
    bool SetSoundVolume(uint64_t id, float volume);
    bool SetSoundPitch(uint64_t id, float pitch);
    void StopAllSounds();
    bool PlayMusic(const std::string &path, bool loop = true, float volume = 1);
    void PauseMusic();
    void ResumeMusic();
    void StopMusic();
    bool IsMusicPlaying() const;
    bool SetMusicVolume(float volume);
    void Update(); // Call once every game frame, including paused gameplay.
    void Shutdown();
    const std::string &LastError() const
    {
        return error_;
    }

  private:
    struct Voice
    {
        uint64_t id = 0;
        SoundHandle source;
        Sound alias{};
        bool loop = false;
    };
    Voice *find(uint64_t id);
    bool fail(const std::string &message);
    static void release(Voice &v);
    std::vector<Voice> voices_;
    size_t maxVoices_;
    uint64_t nextId_ = 1;
    Music music_{};
    bool musicLoaded_ = false, musicActive_ = false;
    std::string error_;
};
