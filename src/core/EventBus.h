#pragma once
#include <unordered_map>
#include <raylib.h>
#include <string>
#include "GAME.h"
#include <functional>
#include <vector>
#include <utility>

class Component;

class BaseEvent;
// 事件回调函数类型，接受一个BaseEvent指针和一个Component指针(发起者的指针)
using EventCallback = std::function<void(const BaseEvent*, Component*)>;

enum EventType
{
	//暂停逻辑
	EVENT_PAUSE_BUTTON_CLICK,
	EVENT_RESUME_BUTTON_CLICK,
	//游戏状态切换
	EVENT_GAME_STATE_CHANGE,
	EVENT_SCENE_CHANGE,

	//输入
	EVENT_MOUSE_CLICK,
	EVENT_SPACE_KEY_PRESS,
	//游戏结束
	EVENT_GAME_OVER,
	EVENT_GAME_WIN,
	
	//音效播放
	EVENT_PLAY_SOUND,
};

class EventBus
{
private:
	//事件类型->一系列<Component*, 回调函数>的映射
	std::unordered_map<EventType, std::vector<std::pair<Component*, EventCallback>>> subscribers;
public:
	void Subscribe(EventType event, Component* component, EventCallback);
	void UnSubscribe(EventType type, Component* comp);
	void Publish(const BaseEvent& event, Component* sender);
};


// 事件抽象基类
class BaseEvent
{
public:
	virtual ~BaseEvent() = default;
	virtual EventType GetType() const = 0;
};

// 鼠标点击事件，携带鼠标坐标
class MouseClickEvent : public BaseEvent
{
public:
	Vector2 pos;
	explicit MouseClickEvent(Vector2 p) : pos(p) {}
	EventType GetType() const override { return EventType::EVENT_MOUSE_CLICK; }
};

// 播放音效事件，携带音效名称字符串
class PlaySoundEvent : public BaseEvent
{
public:
	std::string soundName;
	explicit PlaySoundEvent(std::string name) : soundName(std::move(name)) {}
	EventType GetType() const override { return EventType::EVENT_PLAY_SOUND; }
};

// 场景切换事件
class SceneChangeEvent : public BaseEvent
{
public:
	GameState targetState;
	explicit SceneChangeEvent(GameState s) : targetState(s) {}
	EventType GetType() const override { return EventType::EVENT_SCENE_CHANGE; }
};
