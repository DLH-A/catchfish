#include "raylib.h"
#include "ResourceManager.h"
#include "AudioPlayer.h"
#include "GAME.h"
#include "EventBus.h"
#include "Component.h"
#include "World.h"


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
