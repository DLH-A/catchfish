#pragma once
#include "raylib.h"
#include <memory>

struct SoundResource
{
    Sound sound{};
    ~SoundResource();
    SoundResource() = default;
    SoundResource(const SoundResource &) = delete;
    SoundResource &operator=(const SoundResource &) = delete;
};

using SoundHandle = std::shared_ptr<const SoundResource>;
