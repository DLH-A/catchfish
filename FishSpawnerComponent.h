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
public:
    std::vector<std::unique_ptr<FishBase>> fishes_;

    // 👇 保留 static 声明
    static FishSpawnerComponent* Instance;

private:
    float spawnTimer_;
    float spawnInterval_;
    int maxFishes_;
    std::vector<FishConfig> fishTypes_;

public:
    FishSpawnerComponent() : spawnTimer_(0), spawnInterval_(1.0f), maxFishes_(20) {
        Instance = this;
    }

    void Init() override {
        // 配置5种鱼（路径必须和文件夹里完全一致）
        fishTypes_.push_back({ 15.0f, 150.0f, SKYBLUE, 30.0f, "SmallFish", "assets/images/fish_small.png" });
        fishTypes_.push_back({ 12.0f, 250.0f, YELLOW, 20.0f, "FastFish",  "assets/images/fish_fast.png" });
        fishTypes_.push_back({ 25.0f, 100.0f, GREEN, 30.0f, "MediumFish", "assets/images/fish_medium.png" });
        fishTypes_.push_back({ 40.0f, 60.0f, RED, 15.0f, "LargeFish",    "assets/images/fish_large.png" });
        fishTypes_.push_back({ 35.0f, 120.0f, PURPLE, 5.0f, "RareFish",  "assets/images/fish_rare.png" });
    }

    void update(float dt) override {
        if (!isActive) return;

        // --- 1. 生成鱼 ---
        spawnTimer_ += dt;
        if (spawnTimer_ >= spawnInterval_ && static_cast<int>(fishes_.size()) < maxFishes_) {
            spawnTimer_ = 0.0f;
            SpawnFish();
        }

        // --- 2. 更新所有鱼的状态 ---
        for (auto& fish : fishes_) {
            if (fish->isDead_) continue; // 【新增】如果鱼已经死了，就直接跳过，不用更新了

            fish->update(dt);

            // 【修复1】如果鱼的生命值耗尽，标记为死亡
            if (fish->hp_ <= 0) {
                fish->isDead_ = true; // 使用我们新增的“死亡开关”
            }
        }

        // --- 3. 清除不再需要的鱼 ---
        fishes_.erase(
            std::remove_if(fishes_.begin(), fishes_.end(),
                [](const std::unique_ptr<FishBase>& f) {
                    // 【修复2】修改移除条件：不仅跑出屏幕的要移除，死掉的也要移除
                    return f->IsReadyToRemove() || f->isDead_; // 检查我们新增的“死亡开关”
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

        // 1. 设置参数
        newFish->SetupWithTexture(startPos, targetPos, selectedConfig.speed,
            selectedConfig.radius, selectedConfig.color, selectedConfig.texturePath);

        // 2. 强制初始化
        newFish->ForceInit(this->game);

        // 3. 设置血量和分数
        if (selectedConfig.name == "SmallFish") {
            newFish->hp_ = 1;
            newFish->score = 10;
        }
        else if (selectedConfig.name == "FastFish") {
            newFish->hp_ = 1;
            newFish->score = 15;
        }
        else if (selectedConfig.name == "MediumFish") {
            newFish->hp_ = 3;
            newFish->score = 30;
        }
        else if (selectedConfig.name == "LargeFish") {
            newFish->hp_ = 5;
            newFish->score = 50;
        }
        else if (selectedConfig.name == "RareFish") {
            newFish->hp_ = 10;
            newFish->score = 100;
        }

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

// 👇 【关键】在类定义的最底部，加上这行代码（相当于给它分配了内存）
FishSpawnerComponent* FishSpawnerComponent::Instance = nullptr;