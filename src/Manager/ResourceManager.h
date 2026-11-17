#pragma once
#include "raylib.h"
#include "SoundResource.h"
#include <cstddef>
#include <memory>
#include <string>
#include <unordered_map>

struct ImageResource
{
    Texture2D texture{};
    ~ImageResource();
    ImageResource() = default;
    ImageResource(const ImageResource &) = delete;
    ImageResource &operator=(const ImageResource &) = delete;
};
using ImageHandle = std::shared_ptr<const ImageResource>;

class ResourceManager
{
  public:
    ResourceManager() = default;
    ~ResourceManager();
    ResourceManager(const ResourceManager &) = delete;
    ResourceManager &operator=(const ResourceManager &) = delete;

    // Sets the resource root. Files are loaded on demand, without scanning directories.
    bool Init(const std::string &assetRoot);
    // Paths include their extension and are relative to the root, or absolute.
    ImageHandle GetImage(const std::string &path);
    SoundHandle GetSound(const std::string &path);
    // Borrowed texture: do not unload it; retain an ImageHandle if it must outlive Clear().
    Texture2D GetTexture(const std::string &path);
    // Drops the cache and resets initialization. External handles keep resources alive.
    void Clear();
    void Shutdown() { Clear(); }
    std::size_t TextureCount() const { return images_.size(); }
    const std::string &LastError() const { return error_; }

  private:
    bool fail(const std::string &message);
    std::string resolve(const std::string &path);
    std::string root_, error_;
    std::unordered_map<std::string, std::shared_ptr<ImageResource>> images_;
    std::unordered_map<std::string, std::shared_ptr<SoundResource>> sounds_;
};
