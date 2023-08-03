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
	StaticMesh mesh{renderer, utils::GetSpherePoints(70.0f, 5)};

	PhysicsObject phys{};
	GravitationalPull pull{ phys };
	phys.mass_ = 6.0e11f;
	pull.SetCentre(mesh.Position());
	pull.SetIsGlobal(false);
	
	std::vector<SphereCollider*> sphereColliders{};

	SphereCollider planet{ 70, mesh.Position()};
	sphereColliders.push_back(&planet);

	StaticMesh moon{renderer, utils::GetSpherePoints(7.0f, 1) };
	moon.SetPosition({ 1100.0f, 0.0f, 0.0f });
	PhysicsObject moonPhysics{};
	moonPhysics.mass_ = 6.0e8f;
	moonPhysics.velocity_.y = 2000;

	SphereCollider moonCol{ 7.0f, moon.Position()};
	sphereColliders.push_back(&moonCol);

	
	std::vector<SphereObject*> orbs;
	for (int i = 0; i < num; i++)
	{
		auto orb = new SphereObject(renderer, 5.0f, 2);
		orb->Transform().position = { 1000, 0 + (i * 100), 0};
		orb->Physics().velocity_ = { 0, 100000, 0 };
		orbs.push_back(orb);
	}

	moonPhysics.pulls.push_back(&orbs[0]->Gravity());
	moonPhysics.pulls.push_back(&pull);

	for (int i = 0; i< num; i++)
	{
		orbs[i]->Physics().pulls.push_back(&pull);
	}

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
			moonPhysics.position_ = moon.Position();
			moonCol.SetCentre(moon.Position());
			moonPhysics.Update(physicsTime / 100.0);
			moon.SetPosition(moon.Position() + moonPhysics.velocity_ * ((float)physicsTime / 100.0f));
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
