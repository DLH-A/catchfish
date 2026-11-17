#pragma once
#include "../core/Component.h"
#include "../core/EventBus.h"
#include "ResourceManager.h"
#include <raylib.h>
#include <cmath>
#include <string>

class TurretComponent : public Component {
private:
    Vector2 basePos_;
    float currentAngle_;
    Vector2 aimDirection_;

    Texture2D baseTexture_;
    Texture2D cannonTexture_;

    bool baseLoaded_ = false;
    bool cannonLoaded_ = false;

public:
    TurretComponent() : currentAngle_(-PI / 2.0f) {
        aimDirection_ = { 0.0f, -1.0f };
        baseTexture_ = { 0 };
        cannonTexture_ = { 0 };
    }

    void Init() override {
        int screenWidth = GetScreenWidth();
        int screenHeight = GetScreenHeight();
        // 炮塔位置稍微往下一点，压在底座上
        basePos_ = { (float)screenWidth / 2.0f, (float)screenHeight - 20.0f };

        LoadTextureSafe("assets/images/turret_base.png", baseTexture_, baseLoaded_);
        LoadTextureSafe("assets/images/cannon1.png", cannonTexture_, cannonLoaded_);

        game->eventBus->Subscribe(EVENT_MOUSE_CLICK, this, [this](const BaseEvent* e, Component* sender) {
            if (e->GetType() == EVENT_MOUSE_CLICK) {
                FireBullet(); // 确保调用了这个函数
            }
            });
    }

    void update(float dt) override {
        if (!isActive) return;
        Vector2 mousePos = GetMousePosition();
        Vector2 toMouse = { mousePos.x - basePos_.x, mousePos.y - basePos_.y };

        if (toMouse.x != 0 || toMouse.y != 0) {
            float targetAngle = atan2f(toMouse.y, toMouse.x);
            if (targetAngle > 0) targetAngle = (targetAngle < PI / 2.0f) ? 0.0f : -PI;
            currentAngle_ = targetAngle;
            aimDirection_ = { cosf(currentAngle_), sinf(currentAngle_) };
        }
    }

    void draw() override {
        if (!isActive) return;

        // 1. 画底座
        if (baseLoaded_ && baseTexture_.id != 0) {
            DrawTexture(baseTexture_,
                (int)(basePos_.x - baseTexture_.width / 2.0f),
                (int)(basePos_.y - baseTexture_.height / 2.0f), WHITE);
        }

        // 2. 画炮管
        if (cannonLoaded_ && cannonTexture_.id != 0) {
            float angle = atan2f(aimDirection_.y, aimDirection_.x) * 180.0f / PI;
            float finalAngle = angle + 90.0f;

            float scale = 0.8f;
            float drawWidth = cannonTexture_.width * scale;
            float drawHeight = cannonTexture_.height * scale;

            // 【关键修改】：旋转中心往上提
            // 0.75f 表示旋转中心在图片高度的 75% 处（避开底部的空白/阴影）
            // 如果还是悬空，可以改成 0.7f 或 0.6f
            float originYRatio = 0.4f;
            Vector2 origin = { drawWidth / 2.0f, drawHeight * originYRatio };

            // 【关键修改】：绘制位置也要对应调整
            // 我们要让 origin 点（旋转中心）对齐 basePos_
            // 所以 dest.y = basePos_.y - origin.y
            float offsetX = 65.0f;  // <--- 调整这个值！

            Rectangle dest = {
                basePos_.x - origin.x + offsetX,  // 加上偏移量
                basePos_.y - origin.y,
                drawWidth,
                drawHeight
            };

            Rectangle source = { 0, 0, (float)cannonTexture_.width, (float)cannonTexture_.height };

            DrawTexturePro(cannonTexture_, source, dest, origin, finalAngle, WHITE);
        }
        else {
            DrawLineEx(basePos_, { basePos_.x + aimDirection_.x * 50.0f, basePos_.y + aimDirection_.y * 50.0f }, 5.0f, RED);
        }
    }

    void destroy() override {
        game->eventBus->UnSubscribe(EVENT_MOUSE_CLICK, this);
        if (baseLoaded_) UnloadTexture(baseTexture_);
        if (cannonLoaded_) UnloadTexture(cannonTexture_);
    }

private:
    void LoadTextureSafe(const std::string& path, Texture2D& tex, bool& loaded) {
        if (game && game->resourceManager) {
            ImageHandle img = game->resourceManager->GetImage(path);
            if (img && img->texture.id != 0) { tex = img->texture; loaded = true; return; }
        }
        tex = LoadTexture(path.c_str());
        if (tex.id != 0) loaded = true;
    }

    void FireBullet() {
        float len = cannonLoaded_ ? (float)cannonTexture_.height : 50.0f; // 用高度作为长度
        Vector2 muzzlePos = {
            basePos_.x + aimDirection_.x * len,
            basePos_.y + aimDirection_.y * len
        };
        TurretFireEvent fireEvent(muzzlePos, aimDirection_);
        game->eventBus->Publish(fireEvent, this);
    }
};