#include "Component.h"
#include "EventBus.h"
#include <vector>
#include "ResourceManager.h"
#include "AudioPlayer.h"
#include "raylib.h"
// 这里只实现 Component 的成员函数（如果有），不要再定义 class Component

void Component::draw()
{
}
void Component::Init()
{
}
void Component::update(float deltaTime)
{
}
void Component::destroy()
{
}

void MouseComponent::update(float deltaTime)
{
	if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
		// 创建一个鼠标点击事件
		MouseClickEvent mouseEvent{ GetMousePosition() };
		game->eventBus->Publish(mouseEvent, this);
	}
}
