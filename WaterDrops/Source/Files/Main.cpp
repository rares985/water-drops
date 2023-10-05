#include <ctime>
#include <iostream>

#include <Core/Engine.h>

#include "macros.h"
#include "Scene3D.h"

int main(int argc, char** argv)
{
	srand((unsigned int)time(NULL));
	
	WindowProperties windowProperties;
	windowProperties.resolution = glm::ivec2(WINDOW_HEIGHT, WINDOW_WIDTH);

	WindowObject* window = Engine::Init(windowProperties);
	
	World* world = new Scene3D();
	world->Init();
	world->Run();

	Engine::Exit();

	return 0;
}