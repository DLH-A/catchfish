#pragma once
#include "../core/Component.h"
#include <raylib.h>

class FishBase : public Component {
protected:
    Vector2 position_;        // 当前位置
    Vector2 direction_;       // 移动方向（归一化）
    float speed_;             // 速度（像素/秒）
    float radius_;            // 半径
    Color color_;             // 颜色
    bool hasEnteredScreen_;   // 是否已完全进入屏幕

public:
    FishBase() : speed_(0), radius_(0), hasEnteredScreen_(false) {
        position_ = { 0, 0 };
        direction_ = { 0, 0 };
        color_ = WHITE;
    }

    // 初始化鱼的参数
    void Setup(Vector2 startPos, Vector2 targetPos, float speed, float radius, Color color) {
        position_ = startPos;

        // 计算方向向量并归一化
        Vector2 diff = { targetPos.x - startPos.x, targetPos.y - startPos.y };
        float length = sqrtf(diff.x * diff.x + diff.y * diff.y);
        if (length > 0) {
            direction_ = { diff.x / length, diff.y / length };
        }
        else {
            direction_ = { 1.0f, 0.0f };
        }

        speed_ = speed;
        radius_ = radius;
        color_ = color;
        hasEnteredScreen_ = false;
    }

    void update(float dt) override {
        if (!isActive) return;

        // 匀速直线运动：位置 += 方向 * 速度 * dt
        position_.x += direction_.x * speed_ * dt;
        position_.y += direction_.y * speed_ * dt;

        // 检测是否完全进入屏幕
        if (!hasEnteredScreen_) {
            float screenWidth = GetScreenWidth();
            float screenHeight = GetScreenHeight();

            if (position_.x >= radius_ &&
                position_.x <= screenWidth - radius_ &&
                position_.y >= radius_ &&
                position_.y <= screenHeight - radius_) {
                hasEnteredScreen_ = true;
            }
        }
    }

    void draw() override {
        if (!isActive) return;

        // 绘制鱼身（圆形）
        DrawCircleV(position_, radius_, color_);

        // 绘制眼睛（表示朝向）
        float eyeOffset = radius_ * 0.5f;
        Vector2 eyePos = {
            position_.x + direction_.x * eyeOffset,
            position_.y + direction_.y * eyeOffset
        };
        DrawCircleV(eyePos, radius_ * 0.3f, WHITE);
    }

    // 判断是否可以移除（完全离开屏幕）
    bool IsReadyToRemove() const {
        if (!hasEnteredScreen_) return false;  // 未入场的鱼不能删除

        float screenWidth = GetScreenWidth();
        float screenHeight = GetScreenHeight();
        float buffer = radius_ + 10.0f;

        return (position_.x < -buffer ||
            position_.x > screenWidth + buffer ||
            position_.y < -buffer ||
            position_.y > screenHeight + buffer);
    }

    // Getter
    float GetRadius() const { return radius_; }
};
