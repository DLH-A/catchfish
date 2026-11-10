#pragma once
#include "../core/Component.h"
#include "../core/EventBus.h"
#include <vector>
#include <raylib.h>

struct BulletData {
    Vector2 position;
    Vector2 direction;
    float speed;
    float radius;
    bool isActive;

    BulletData() : speed(600.0f), radius(6.0f), isActive(true) {
        position = { 0, 0 };
        direction = { 0, 0 };
    }
};

class BulletComponent : public Component {
private:
    std::vector<BulletData> bullets_;

public:
    void Init() override {
        // 订阅炮塔发射事件
        game->eventBus->Subscribe(EVENT_TURRET_FIRE, this, [this](const BaseEvent* e, Component* sender) {
            if (e->GetType() == EVENT_TURRET_FIRE) {
                // 安全类型转换
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

            bullet.position.x += bullet.direction.x * bullet.speed * dt;
            bullet.position.y += bullet.direction.y * bullet.speed * dt;

            float screenWidth = GetScreenWidth();
            float screenHeight = GetScreenHeight();
            float buffer = 20.0f;

            if (bullet.position.x < -buffer || bullet.position.x > screenWidth + buffer ||
                bullet.position.y < -buffer || bullet.position.y > screenHeight + buffer) {
                bullet.isActive = false;
            }
        }

        // 安全清理失效的子弹
        bullets_.erase(
            std::remove_if(bullets_.begin(), bullets_.end(),
                [](const BulletData& b) { return !b.isActive; }),
            bullets_.end()
        );
    }

    void draw() override {
        if (!isActive) return;

        for (const auto& bullet : bullets_) {
            if (bullet.isActive) {
                DrawCircleV(bullet.position, bullet.radius, YELLOW);
            }
        }
    }

    void destroy() override {
        game->eventBus->UnSubscribe(EVENT_TURRET_FIRE, this);
        bullets_.clear();
    }
};