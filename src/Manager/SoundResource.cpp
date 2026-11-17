#include "SoundResource.h"

SoundResource::~SoundResource()
{
    if (sound.frameCount && IsAudioDeviceReady())
        UnloadSound(sound);
}
