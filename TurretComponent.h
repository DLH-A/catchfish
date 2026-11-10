#pragma once
#include "../core/Component.h"
#include "../core/EventBus.h" // 确保包含 EventBus
#include <raylib.h>
#include <cmath>

class TurretComponent : public Component {
private:
    Vector2 basePos_;
    float currentAngle_;
    Vector2 aimDirection_;

public:
    TurretComponent() : currentAngle_(-PI / 2.0f) {
        aimDirection_ = { 0.0f, -1.0f };
    }

    void Init() override {
        int screenWidth = GetScreenWidth();
        int screenHeight = GetScreenHeight();
        basePos_ = { static_cast<float>(screenWidth) / 2.0f, static_cast<float>(screenHeight) - 50.0f };

        // 订阅鼠标点击事件 (注意回调签名必须匹配: const BaseEvent*, Component*)
        game->eventBus->Subscribe(EVENT_MOUSE_CLICK, this, [this](const BaseEvent* e, Component* sender) {
            if (e->GetType() == EVENT_MOUSE_CLICK) {
                // 框架的 MouseClickEvent 默认代表有效点击，直接发射
                float barrelLength = 45.0f;
                Vector2 muzzlePos = {
                    basePos_.x + aimDirection_.x * barrelLength,
                    basePos_.y + aimDirection_.y * barrelLength
                };

                // 创建并发布发射事件
                TurretFireEvent fireEvent(muzzlePos, aimDirection_);
                game->eventBus->Publish(fireEvent, this);
            }
            });
    }

    void update(float dt) override {
        if (!isActive) return;

        Vector2 mousePos = GetMousePosition();
        Vector2 toMouse = { mousePos.x - basePos_.x, mousePos.y - basePos_.y };

        if (toMouse.x != 0 || toMouse.y != 0) {
            float targetAngle = atan2f(toMouse.y, toMouse.x);

            // 限制瞄准范围：只能在屏幕上方半圆（-PI 到 0）
            if (targetAngle > 0) {
                targetAngle = (targetAngle < PI / 2.0f) ? 0.0f : -PI;
            }

            currentAngle_ = targetAngle;
            aimDirection_ = { cosf(currentAngle_), sinf(currentAngle_) };
        }
    }

    void draw() override {
        if (!isActive) return;

        DrawCircleV(basePos_, 30.0f, DARKGRAY);

        float barrelLength = 45.0f;
        Vector2 muzzlePos = {
            basePos_.x + aimDirection_.x * barrelLength,
            basePos_.y + aimDirection_.y * barrelLength
        };

        DrawLineEx(basePos_, muzzlePos, 12.0f, GRAY);
        DrawCircleV(muzzlePos, 8.0f, LIGHTGRAY);
    }

    void destroy() override {
        // 注意：框架中是 UnSubscribe (大写 S)
        game->eventBus->UnSubscribe(EVENT_MOUSE_CLICK, this);
    }
};