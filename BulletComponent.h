#pragma once
#include "../core/Component.h"
#include "../core/EventBus.h"
#include "ResourceManager.h"
#include <vector>
#include <raylib.h>
#include <string>

struct BulletData {
    Vector2 position;
    Vector2 direction;
    float speed;
    float radius;
    bool isActive;

    BulletData() : speed(800.0f), radius(10.0f), isActive(true) {
        position = { 0, 0 };
        direction = { 0, 0 };
    }
};

class BulletComponent : public Component {
private:
    std::vector<BulletData> bullets_;
    Texture2D bulletTexture_;
    bool textureLoaded_ = false;

public:
    BulletComponent() {
        bulletTexture_ = { 0 };
    }

    void Init() override {
        // 1. 加载子弹贴图
        // 确保 assets/images/ 下有 bullet.png
        if (game && game->resourceManager) {
            ImageHandle img = game->resourceManager->GetImage("assets/images/bullet.png");
            if (img && img->texture.id != 0) {
                bulletTexture_ = img->texture;
                textureLoaded_ = true;
            }
        }
        // 如果 ResourceManager 加载失败，尝试直接加载
        if (!textureLoaded_) {
            bulletTexture_ = LoadTexture("assets/images/bullet.png");
            if (bulletTexture_.id != 0) textureLoaded_ = true;
        }

        // 2. 订阅炮塔发射事件
        game->eventBus->Subscribe(EVENT_TURRET_FIRE, this, [this](const BaseEvent* e, Component* sender) {
            if (e->GetType() == EVENT_TURRET_FIRE) {
                const TurretFireEvent* fireEvent = dynamic_cast<const TurretFireEvent*>(e);
                if (fireEvent) {
                    BulletData newBullet;
                    newBullet.position = fireEvent->spawnPos;
                    newBullet.direction = fireEvent->direction;
                    bullets_.push_back(newBullet);
                }
            }
            });
    }

    void update(float dt) override {
        if (!isActive) return;

        for (auto& bullet : bullets_) {
            if (!bullet.isActive) continue;

            // 移动
            bullet.position.x += bullet.direction.x * bullet.speed * dt;
            bullet.position.y += bullet.direction.y * bullet.speed * dt;

            // 出屏检测
            float screenWidth = GetScreenWidth();
            float screenHeight = GetScreenHeight();
            if (bullet.position.x < -50 || bullet.position.x > screenWidth + 50 ||
                bullet.position.y < -50 || bullet.position.y > screenHeight + 50) {
                bullet.isActive = false;
            }
        }

        // 清理失效子弹
        bullets_.erase(
            std::remove_if(bullets_.begin(), bullets_.end(), [](const BulletData& b) { return !b.isActive; }),
            bullets_.end()
        );
    }

    void draw() override {
        if (!isActive) return;

        for (const auto& bullet : bullets_) {
            if (bullet.isActive) {
                if (textureLoaded_ && bulletTexture_.id != 0) {
                    // 计算角度
                    float angle = atan2f(bullet.direction.y, bullet.direction.x) * 180.0f / PI;

                    // 绘制大小 (可以根据贴图调整)
                    float size = 20.0f;
                    Rectangle source = { 0, 0, (float)bulletTexture_.width, (float)bulletTexture_.height };
                    Rectangle dest = { bullet.position.x - size / 2.0f, bullet.position.y - size / 2.0f, size, size };
                    Vector2 origin = { size / 2.0f, size / 2.0f };

                    // 如果子弹图片是横向的，可能需要 +90 度，先试直接传 angle
                    DrawTexturePro(bulletTexture_, source, dest, origin, angle + 90.0f, WHITE);
                }
                else {
                    // 降级：画黄色圆点
                    DrawCircleV(bullet.position, bullet.radius, YELLOW);
                }
            }
        }
    }

    void destroy() override {
        game->eventBus->UnSubscribe(EVENT_TURRET_FIRE, this);
        if (textureLoaded_) UnloadTexture(bulletTexture_);
        bullets_.clear();
    }
};