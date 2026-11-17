#pragma once

#include <vector> // std::vector 在头中必须可见

// 前向声明：只在接口中以指针/引用或不需要完整定义时使用
class ResourceManager;
class Component;
class EventBus;
class AudioPlayer;

enum GameState
{

    MENU,
    GAME_PLAY,


};

struct Game
{
    GameState state = GameState::MENU;
    ResourceManager* resourceManager = nullptr;
	AudioPlayer* audioPlayer = nullptr;
    EventBus* eventBus = nullptr;

    int currentLevel = 1;
    int sunNumber = 0;
    int plantCardNumber = 5;
    int unlockedPlantCardNumber = 5;
    int sceneID = 0;
    int currentWave = 0;
    int totalWave = 0;
    int selectedPlant = -1;
    std::vector<int> zombiecategory;
};
