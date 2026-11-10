#pragma once
#include "../core/Component.h"
#include "FishBase.h"
#include <vector>
#include <memory>
#include <raylib.h>

// 鱼的配置信息
struct FishConfig {
    float radius;      // 半径
    float speed;       // 速度
    Color color;       // 颜色
    float weight;      // 生成权重
    const char* name;  // 名称
};

class FishSpawnerComponent : public Component {
private:
    std::vector<std::unique_ptr<FishBase>> fishes_;  // 所有鱼的容器
    float spawnTimer_;           // 生成计时器
    float spawnInterval_;        // 生成间隔（秒）
    int maxFishes_;              // 最大鱼数量
    std::vector<FishConfig> fishTypes_;  // 5种鱼的配置

public:
    FishSpawnerComponent() : spawnTimer_(0), spawnInterval_(1.0f), maxFishes_(20) {}

    void Init() override {
        // 初始化5种鱼的配置
        fishTypes_.push_back({ 15.0f, 150.0f, SKYBLUE, 30.0f, "SmallFish" });   // 小鱼
        fishTypes_.push_back({ 12.0f, 250.0f, YELLOW, 20.0f, "FastFish" });     // 快速鱼
        fishTypes_.push_back({ 25.0f, 100.0f, GREEN, 30.0f, "MediumFish" });    // 中等鱼
        fishTypes_.push_back({ 40.0f, 60.0f, RED, 15.0f, "LargeFish" });        // 大鱼
        fishTypes_.push_back({ 35.0f, 120.0f, PURPLE, 5.0f, "RareFish" });      // 稀有鱼
    }

    void update(float dt) override {
        if (!isActive) return;

        // 1. 生成逻辑
        spawnTimer_ += dt;
        if (spawnTimer_ >= spawnInterval_ && static_cast<int>(fishes_.size()) < maxFishes_) {
            spawnTimer_ = 0.0f;
            SpawnFish();
        }

        // 2. 更新所有鱼
        for (auto& fish : fishes_) {
            fish->update(dt);
        }

        // 3. 安全清理已离开屏幕的鱼
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

        // 随机选择生成边：0=左, 1=右, 2=上, 3=下
        int edge = GetRandomValue(0, 3);
        Vector2 startPos;
        float buffer = 60.0f;  // 屏幕外缓冲距离

        // 在选定边的屏幕外生成
        switch (edge) {
        case 0: // 左边
            startPos = { -buffer, static_cast<float>(GetRandomValue(0, screenHeight)) };
            break;
        case 1: // 右边
            startPos = { static_cast<float>(screenWidth + buffer), static_cast<float>(GetRandomValue(0, screenHeight)) };
            break;
        case 2: // 上边
            startPos = { static_cast<float>(GetRandomValue(0, screenWidth)), -buffer };
            break;
        case 3: // 下边
            startPos = { static_cast<float>(GetRandomValue(0, screenWidth)), static_cast<float>(screenHeight + buffer) };
            break;
        }

        // 随机选择屏幕内的目标点
        Vector2 targetPos = {
            static_cast<float>(GetRandomValue(100, screenWidth - 100)),
            static_cast<float>(GetRandomValue(100, screenHeight - 100))
        };

        // 根据权重随机选择鱼的类型
        FishConfig selectedConfig = SelectFishByWeight();

        // 创建新鱼
        auto newFish = std::make_unique<FishBase>();
   
        newFish->Setup(startPos, targetPos, selectedConfig.speed, selectedConfig.radius, selectedConfig.color);
        newFish->isActive = true;

        fishes_.push_back(std::move(newFish));
    }

    FishConfig SelectFishByWeight() {
        // 计算总权重
        float totalWeight = 0;
        for (const auto& type : fishTypes_) {
            totalWeight += type.weight;
        }

        // 随机选择
        float randomValue = static_cast<float>(GetRandomValue(0, 1000)) / 10.0f;
        float currentWeight = 0;

        for (const auto& type : fishTypes_) {
            currentWeight += type.weight;
            if (randomValue <= currentWeight) {
                return type;
            }
        }

        return fishTypes_.back();  // 默认返回最后一种
    }
};
