#include "raylib.h"
#include "ResourceManager.h"
#include "AudioPlayer.h"
#include "GAME.h"
#include "EventBus.h"
#include "Component.h"
#include "World.h"
#include "FishSpawnerComponent.h"
#include "TurretComponent.h"
#include "BulletComponent.h"


int main() {
	//初始化
	InitWindow(800, 600, "捕鱼");
	InitAudioDevice();
	SetTargetFPS(60);

	Game game;
	ResourceManager resourceManager;
	AudioPlayer audioPlayer;
	EventBus eventBus;
	World world(game);

	game.resourceManager = &resourceManager;
	game.audioPlayer = &audioPlayer;
	game.eventBus = &eventBus;

	// ========== 添加第一阶段玩法组件 ==========
	world.addComponent(std::make_unique<FishSpawnerComponent>());
	world.addComponent(std::make_unique<TurretComponent>());
	world.addComponent(std::make_unique<BulletComponent>());

	//资源加载
	resourceManager.Init("assets");

	float dt = GetFrameTime();
	while (!WindowShouldClose()) {
		dt = GetFrameTime();

		//更新逻辑
		world.update(dt);
		audioPlayer.Update();

		//绘制逻辑
		BeginDrawing();
		ClearBackground(RAYWHITE);
		world.draw();
		EndDrawing();
	}
	world.destroy();
	CloseWindow();
}
