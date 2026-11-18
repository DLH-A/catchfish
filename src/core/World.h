#pragma once
#include <vector>
#include <memory>
#include "Component.h"
#include "EventBus.h"
#include "GAME.h"

class World
{
private:
    Game& game;
    Texture2D coinTexture_; // 【加上这一行，用来保存数字图片】
    std::vector<std::unique_ptr<Component>> components_;
public:
    explicit World(Game& g) : game(g) {}

    void addComponent(std::unique_ptr<Component> c)
    {
        if (coinTexture_.width == 0) {
            coinTexture_ = LoadTexture("assets/images/coinText.png");
        }

        c->setGame(&game);
		c->setGame(&game);
        c->Init();
		
        components_.push_back(std::move(c));
    }

    void update(float dt)
    {
        for (auto& c : components_)
        {
            if (c) c->update(dt);
        }
    }
    void draw()
    {
		//绘制所有组件
        for (auto& c : components_)
        {
            if (c) c->draw();

        }

        // 2. 【新增】绘制左下角的计数器
    // 前提：你需要在 TurretComponent 的 Init() 里加载 coinText.png，或者在 main.cpp 里加载

    // 获取分数的每一位数字
        int score = game.score;
        int digits[] = { 0, 0, 0, 0, 0, 0 }; // 假设最多6位数
        for (int i = 5; i >= 0; --i) {
            digits[i] = score % 10;
            score /= 10;
            if (score == 0 && i != 0) break; // 前面补0
        }

        // 绘制每一位数字
        float startX = 40.0f;
        float startY = 565.0f; // 靠近底部
        float spacing = 8.5f; // 每个数字的间距

        for (int i = 0; i < 6; i++) {
            // 1. 从素材图片里剪切
            Rectangle src = { (float)(digits[i] * 36), 0.0f, 36.0f, 49.0f };

            // 2. 画到屏幕上，大小设为 15x15 (你觉得小就改大，觉得大就改小)
            Rectangle dest = { startX + i * (14.4f + spacing), startY, 14.0f, 19.0f };

            // 3. 用成员变量画出来
            DrawTexturePro(coinTexture_, src, dest, { 0,0 }, 0, WHITE);
        }
    }

};
