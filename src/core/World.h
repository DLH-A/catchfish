#pragma once
#include <vector>
#include <memory>
#include "Component.h"
#include "EventBus.h"
#include "GAME.h"
#include "TurretComponent.h"  // 【新增】让 World 认识 TurretComponent

class World
{
private:
    Game& game;
    Texture2D coinTexture_; // 【加上这一行，用来保存数字图片】
    Texture2D uiTexture_;
    bool uiLoaded_ = false;

    float btnSize = 40.0f;
    Rectangle addBtnRect_ = { 0, 0, 0, 0 };
    Rectangle subBtnRect_ = { 0, 0, 0, 0 };

    void InitUIPositions() {
        float screenW = GetScreenWidth();
        float screenH = GetScreenHeight();

        float turretCenterX = screenW / 2.0f;
        float turretY = screenH - 20.0f;

        // 【新增】自定义一个压扁后的高度，比如 25.0f (默认是40，你可以改小，越小越扁)
        float flatHeight = 30.0f;

        // 【修改】最后两个参数：宽度保持 btnSize(40)，高度改为 flatHeight
        subBtnRect_ = { turretCenterX - 48.0f, turretY - btnSize / 2.0f, btnSize, flatHeight };
        addBtnRect_ = { turretCenterX + 90.0f, turretY - btnSize / 2.0f, btnSize, flatHeight };
    }
    std::vector<std::unique_ptr<Component>> components_;
public:
    explicit World(Game& g) : game(g) {}

    void addComponent(std::unique_ptr<Component> c)
    {
        if (coinTexture_.width == 0) {
            coinTexture_ = LoadTexture("assets/images/coinText.png");
            InitUIPositions();
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
        Vector2 mousePos = GetMousePosition();
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            for (auto& c : components_)
            {
                TurretComponent* turret = dynamic_cast<TurretComponent*>(c.get());
                if (turret) {
                    if (CheckCollisionPointRec(mousePos, addBtnRect_)) {
                        turret->HandleUpgrade(true);
                    }
                    else if (CheckCollisionPointRec(mousePos, subBtnRect_)) {
                        turret->HandleUpgrade(false);
                    }
                    break;
                }
            }
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
       
        

        // 【重新添加】加载并绘制加减号按钮
        if (!uiLoaded_) {
            uiTexture_ = LoadTexture("assets/images/turret_base.png"); // 1. 确保加载图片
            uiLoaded_ = true;
        }

        if (uiLoaded_ && uiTexture_.id != 0) {
            // 2. 【剪切代码】根据你提供的 765x122 图片和 32x25 的按钮大小精确调整
            // 注意：Y轴设为 95.0f 是因为按钮在图片底部
            Rectangle srcSub = { 92.0f, 78.0f, 33.5f, 25.0f };
            Rectangle srcAdd = { 4.0f, 78.0f, 34.0f, 25.0f };

            // 3. 【粘贴代码】画到屏幕上
            DrawTexturePro(uiTexture_, srcAdd, addBtnRect_, { 0,0 }, 0, WHITE);
            DrawTexturePro(uiTexture_, srcSub, subBtnRect_, { 0,0 }, 0, WHITE);
        }
    }

};
