#include "EventBus.h"
//订阅事件的类型,组件指针和回调函数
void EventBus::Subscribe(EventType type, Component* comp, EventCallback cb)
{
    subscribers[type].emplace_back(comp, cb);
}
//取消订阅事件，移除指定组件
void EventBus::UnSubscribe(EventType type, Component* comp)
{
    auto& list = subscribers[type];
    for (auto it = list.begin(); it != list.end(); )
    {
        if (it->first == comp)
            it = list.erase(it);
        else
            ++it;
    }
}
//发布事件，调用所有订阅该事件的组件的回调函数
void EventBus::Publish(const BaseEvent& event, Component* sender)
{
	//获取事件类型，查找订阅者列表，如果没有订阅者则返回
    EventType t = event.GetType();
    auto it = subscribers.find(t);
    if (it == subscribers.end())
        return;
	//遍历订阅者列表，调用回调函数
    auto& list = it->second;

    for (auto& p : list)
    {
		//调用回调函数，传入事件和组件指针
        p.second(&event, sender);
    }
}
