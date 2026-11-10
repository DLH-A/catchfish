#pragma once
#include "../core/Component.h"
#include "FishBase.h"
#include <vector>
#include <memory>
#include <raylib.h>
#include <string>

struct FishConfig {
    float radius;
    float speed;
    Color color;
    float weight;
    const char* name;
    std::string texturePath;  // 新增：贴图路径
};

class FishSpawnerComponent : public Component {
private:
    std::vector<std::unique_ptr<FishBase>> fishes_;
    float spawnTimer_;
    float spawnInterval_;
    int maxFishes_;
    std::vector<FishConfig> fishTypes_;

public:
    FishSpawnerComponent() : spawnTimer_(0), spawnInterval_(1.0f), maxFishes_(20) {}

       void Init() override {
        // 配置5种鱼（路径必须和文件夹里完全一致）
        fishTypes_.push_back({15.0f, 150.0f, SKYBLUE, 30.0f, "SmallFish", "assets/images/fish_small.png"});
        fishTypes_.push_back({12.0f, 250.0f, YELLOW, 20.0f, "FastFish",  "assets/images/fish_fast.png"});
        fishTypes_.push_back({25.0f, 100.0f, GREEN, 30.0f, "MediumFish", "assets/images/fish_medium.png"});
        fishTypes_.push_back({40.0f, 60.0f, RED, 15.0f, "LargeFish",    "assets/images/fish_large.png"});
        fishTypes_.push_back({35.0f, 120.0f, PURPLE, 5.0f, "RareFish",  "assets/images/fish_rare.png"});
    }

    void update(float dt) override {
        if (!isActive) return;

        spawnTimer_ += dt;
        if (spawnTimer_ >= spawnInterval_ && static_cast<int>(fishes_.size()) < maxFishes_) {
            spawnTimer_ = 0.0f;
            SpawnFish();
        }

        for (auto& fish : fishes_) {
            fish->update(dt);
        }

        fishes_.erase(
            std::remove_if(fishes_.begin(), fishes_.end(),
                [](const std::unique_ptr<FishBase>& f) {
                    return f->IsReadyToRemove();
                }),
            fishes_.end()
        );
    }

    void draw() override {
        if (!isActive) return;

        for (auto& fish : fishes_) {
            fish->draw();
        }
    }

    void destroy() override {
        fishes_.clear();
    }

private:
    void SpawnFish() {
        int screenWidth = GetScreenWidth();
        int screenHeight = GetScreenHeight();

        int edge = GetRandomValue(0, 3);
        Vector2 startPos;
        float buffer = 60.0f;

        switch (edge) {
        case 0:
            startPos = { -buffer, static_cast<float>(GetRandomValue(0, screenHeight)) };
            break;
        case 1:
            startPos = { static_cast<float>(screenWidth + buffer), static_cast<float>(GetRandomValue(0, screenHeight)) };
            break;
        case 2:
            startPos = { static_cast<float>(GetRandomValue(0, screenWidth)), -buffer };
            break;
        case 3:
            startPos = { static_cast<float>(GetRandomValue(0, screenWidth)), static_cast<float>(screenHeight + buffer) };
            break;
        }

        Vector2 targetPos = {
            static_cast<float>(GetRandomValue(100, screenWidth - 100)),
            static_cast<float>(GetRandomValue(100, screenHeight - 100))
        };

        FishConfig selectedConfig = SelectFishByWeight();

        auto newFish = std::make_unique<FishBase>();

        // 1. 设置参数 (注意这里要传颜色，不然降级显示是白色)
        newFish->SetupWithTexture(startPos, targetPos, selectedConfig.speed,
            selectedConfig.radius, selectedConfig.color, selectedConfig.texturePath);

        // 2. 【关键】调用刚才写的强制加载方法！
        newFish->ForceInit(this->game);

        newFish->isActive = true;
        fishes_.push_back(std::move(newFish));
    }

    FishConfig SelectFishByWeight() {
        float totalWeight = 0;
        for (const auto& type : fishTypes_) {
            totalWeight += type.weight;
        }

        float randomValue = static_cast<float>(GetRandomValue(0, 1000)) / 10.0f;
        float currentWeight = 0;

        for (const auto& type : fishTypes_) {
            currentWeight += type.weight;
            if (randomValue <= currentWeight) {
                return type;
            }
        }

        return fishTypes_.back();
    }
};