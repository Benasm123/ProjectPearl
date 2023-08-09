#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <memory>
#include "Core/Logger.h"
#include "Render/Vulkan/Renderer/VulkanRenderer.h"
#include "Collisions/SphereCollider.h"
#include "Collisions/BoxCollider.h"
#include "PhysicsObject.h"
#include "GravitationalPull.h"
#include "Core/StaticMesh.h"

#ifdef __cplusplus
extern "C"
#endif
#include <iostream>
#include "../TestClasses/SphereObject.h"

constexpr double targetFPS = 500;
constexpr double renderTime = 1.0 / targetFPS;

constexpr double targetHZ = 60000.0;
constexpr double physicsTime = 1.0 / targetHZ;

int main(int argc, char* argv[])
{
	int num = 10;

	spdlog::set_level(spdlog::level::trace);

	pearl::Window window("Pearl Engine", 1200, 1000);
	auto renderer = VulkanRenderer(window);

	// PLANET
	SphereObject planet{ renderer, 10000.0f, 5 };
	planet.Transform().position = { 0.0f, -10001.0f, 0.0f };

	// TODO -> Make a clock class to manage time.
	auto oldTime = std::chrono::system_clock::now();

	uint32_t frameCount = 0;
	double deltaCount = 0.0;
	double renderCount = 0.0;
	double physicsCount = 0.0;

	bool running = true;
	while (running)
	{
		auto currentTime = std::chrono::system_clock::now();
		double deltaTime = static_cast<double>((std::chrono::duration_cast<std::chrono::microseconds>(currentTime - oldTime)).count()) / 1e+6;
		if (deltaTime == 0.0) continue;
		if ( deltaTime > 0.1 ) deltaTime = 0.1;
		oldTime = currentTime;

		physicsCount += deltaTime;
		if (physicsCount >= physicsTime) {
			planet.PhysicsUpdate(physicsTime);

			int temp = std::floor(physicsCount / physicsTime);
			physicsCount -= (temp * physicsTime);

			for (SphereObject* orb : orbs)
			{
				orb->PhysicsUpdate(physicsTime / 100.0);
			}
		}

		deltaCount += deltaTime;
		frameCount++;
		if (deltaCount >= 1.0) {
			std::cout << "\r" << frameCount;;
			deltaCount -= 1.0;
			frameCount = 0;
		}


		renderCount += deltaTime;
		if (renderCount >= renderTime) {
			renderer.Update();
			int temp = std::floor(renderCount / renderTime);
			renderCount -= (temp * renderTime);
		}

		running = window.Update();
	}


	for ( int i = 0; i < num; i++ )
	{
		delete orbs[i];
	}

	return 0;
}
