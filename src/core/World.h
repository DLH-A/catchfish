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
    std::vector<std::unique_ptr<Component>> components_;
public:
    explicit World(Game& g) : game(g) {}

    void addComponent(std::unique_ptr<Component> c)
    {
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
        // 2. 【在这里加上这一行】在画面最上方绘制分数
        DrawText(TextFormat("Score: %d", game.score), 20, 20, 40, BLACK);
	}
    void destroy()
    {
        for (auto& c : components_)
        {
            if (c) c->destroy();
        }
        components_.clear();
	}

};
