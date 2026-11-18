#pragma once
#include "../core/Component.h"
#include "ResourceManager.h"
#include <raylib.h>

class BackgroundComponent : public Component {
private:
    Texture2D bgTexture_;
    bool loaded_ = false;

public:
    BackgroundComponent() {
        bgTexture_ = { 0 };
    }

    void Init() override {
        // 加载背景图
        if (game && game->resourceManager) {
            ImageHandle img = game->resourceManager->GetImage("assets/images/game_bg_2_hd.png");
            if (img && img->texture.id != 0) {
                bgTexture_ = img->texture;
                loaded_ = true;
            }
            else {
                // 降级方案：直接加载
                bgTexture_ = LoadTexture("assets/images/game_bg_2_hd.png");
                if (bgTexture_.id != 0) loaded_ = true;
            }
        }
    }

    void update(float dt) override {
        // 背景不需要逻辑更新
    }

    void draw() override {
        if (!isActive) return;

        if (loaded_ && bgTexture_.id != 0) {
            // 方案 A：如果图片大小刚好是 800x600，直接画在 (0,0)
            // DrawTexture(bgTexture_, 0, 0, WHITE);

            // 方案 B（推荐）：强制拉伸铺满整个屏幕，适应不同分辨率
            Rectangle source = { 0, 0, (float)bgTexture_.width, (float)bgTexture_.height };
            Rectangle dest = { 0, 0, (float)GetScreenWidth(), (float)GetScreenHeight() };
            DrawTexturePro(bgTexture_, source, dest, { 0, 0 }, 0.0f, WHITE);
        }
        else {
            // 没加载出来就画个白底，防止黑屏
            ClearBackground(RAYWHITE);
        }
    }


    void destroy() override {
        // 如果是通过 resourceManager 加载的，不需要手动 UnloadTexture
    }
};