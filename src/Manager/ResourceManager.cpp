#include "ResourceManager.h"
#include <exception>
#include <filesystem>

namespace fs = std::filesystem;

ImageResource::~ImageResource()
{
    if (texture.id && IsWindowReady())
        UnloadTexture(texture);
}

ResourceManager::~ResourceManager()
{
    Clear();
}

bool ResourceManager::fail(const std::string &message)
{
    error_ = message;
    TraceLog(LOG_WARNING, "ResourceManager: %s", message.c_str());
    return false;
}

void ResourceManager::Clear()
{
    images_.clear();
    sounds_.clear();
    root_.clear();
    error_.clear();
}

bool ResourceManager::Init(const std::string &assetRoot)
{
    Clear();
    if (assetRoot.empty())
        return fail("Resource directory is empty");
    try
    {
        auto root = fs::absolute(fs::u8path(assetRoot)).lexically_normal();
        if (!fs::is_directory(root))
            return fail("Directory not found: " + assetRoot);
        root_ = root.u8string();
        return true;
    }
    catch (const std::exception &e)
    {
        return fail(e.what());
    }
}

std::string ResourceManager::resolve(const std::string &path)
{
    if (root_.empty())
    {
        fail("Init must succeed before loading resources");
        return {};
    }
    if (path.empty())
    {
        fail("Resource path is empty");
        return {};
    }
    try
    {
        auto full = (fs::u8path(root_) / fs::u8path(path)).lexically_normal();
        if (!fs::is_regular_file(full))
        {
            fail("Resource file not found: " + full.u8string());
            return {};
        }
        return full.u8string();
    }
    catch (const std::exception &e)
    {
        fail(e.what());
        return {};
    }
}

ImageHandle ResourceManager::GetImage(const std::string &path)
{
    auto full = resolve(path);
    if (full.empty())
        return {};
    auto cached = images_.find(full);
    if (cached != images_.end())
    {
        error_.clear();
        return cached->second;
    }
    if (!IsWindowReady())
    {
        fail("InitWindow must precede GetImage");
        return {};
    }
    auto resource = std::make_shared<ImageResource>();
    resource->texture = LoadTexture(full.c_str());
    if (!resource->texture.id)
    {
        fail("Cannot load texture: " + full);
        return {};
    }
    images_[full] = resource;
    error_.clear();
    return resource;
}

Texture2D ResourceManager::GetTexture(const std::string &path)
{
    auto image = GetImage(path);
    return image ? image->texture : Texture2D{};
}

SoundHandle ResourceManager::GetSound(const std::string &path)
{
    auto full = resolve(path);
    if (full.empty())
        return {};
    auto cached = sounds_.find(full);
    if (cached != sounds_.end())
    {
        error_.clear();
        return cached->second;
    }
    if (!IsAudioDeviceReady())
    {
        fail("InitAudioDevice must precede GetSound");
        return {};
    }
    auto resource = std::make_shared<SoundResource>();
    resource->sound = LoadSound(full.c_str());
    if (!IsSoundValid(resource->sound))
    {
        fail("Cannot load sound: " + full);
        return {};
    }
    sounds_[full] = resource;
    error_.clear();
    return resource;
}
