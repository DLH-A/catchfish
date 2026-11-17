#pragma once
#include "ResourceManager.h"
#include "AudioPlayer.h"
#include "EventBus.h"

class Component {
protected:
	Game* game;
public:
	bool isActive = true;
	virtual void Init();
	virtual void update(float deltaTime);
	virtual void draw();
	virtual void destroy();
	virtual ~Component() = default;
	void setGame(Game* g) { game = g; }
};

class MouseComponent : public Component {
public:
	void update(float deltaTime) override;
	~MouseComponent() override = default;
};

