#pragma once
#include "../core/EventBus.h"
#include <raylib.h>

// 定义事件类型
#define EVENT_TURRET_FIRE "EVENT_TURRET_FIRE"

// 炮塔发射事件数据结构
struct FireEventData {
    Vector2 spawnPos;      // 子弹生成位置（炮口）
    Vector2 direction;     // 发射方向（已归一化）
};