#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <memory>
#include "Core/Logger.h"
#include "Render/Vulkan/Renderer/VulkanRenderer.h"
#include "Collisions/SphereCollider.h"
#include "Collisions/BoxCollider.h"
#include "PhysicsObject.h"
#include "GravitationalPull.h"

#ifdef __cplusplus
extern "C"
#endif
#include <iostream>

constexpr double targetFPS = 4000;
constexpr double renderTime = 1.0 / targetFPS;

constexpr double targetHZ = 60000.0;
constexpr double physicsTime = 1.0 / targetHZ;

int main(int argc, char* argv[])
{
	int num = 1;

	spdlog::set_level(spdlog::level::trace);

	pearl::Window window("Pearl Engine", 1200, 1000);
	auto renderer = VulkanRenderer(window);

	// PLANET
	pearl::typesRender::Mesh mesh;
	mesh.data = utils::GetSpherePoints(70.0f, 3);
	mesh.position.z = 0;
	mesh.position.y = 0;
	mesh.position.x = 0;

	PhysicsObject phys{};
	phys.mass_ = 6.0e11f;
	GravitationalPull pull{ phys };
	pull.SetCentre(mesh.position);
	pull.SetIsGlobal(false);
	
	std::vector<SphereCollider*> sphereColliders{};

	SphereCollider planet{ 70, mesh.position };
	sphereColliders.push_back(&planet);

	pearl::typesRender::Mesh moon;
	moon.data = utils::GetSpherePoints(7.0f, 1);
	moon.position = { 1100, 0, 0 };
	PhysicsObject moonPhysics{};
	moonPhysics.mass_ = 6.0e8f;
	moonPhysics.velocity_.y = 2000;

	SphereCollider moonCol{ 7.0f, moon.position };
	sphereColliders.push_back(&moonCol);

	// ORB OBJECT

	struct Orb
	{
		pearl::typesRender::Mesh mesh;
		SphereCollider coll;
		PhysicsObject phys;
		GravitationalPull pull;

		Orb()
			: coll{ 20.0f, mesh.position }
			, pull{phys}
		{
			mesh.data = utils::GetSpherePoints(20.0f, 1);
			mesh.position.x = (float)(rand() % 500) - 250.0f;
			mesh.position.y = (float)(rand() % 500) - 250.0f;
			mesh.position.z = (float)(rand() % 500) - 250.0f;
			mesh.position = glm::normalize(mesh.position) * 1520.0f;
			mesh.position = { 1000, 0, 0 };

			phys.mass_ = 7.3e10;

			phys.velocity_.x += (rand() % 8000) - 4000;
			phys.velocity_.y += (rand() % 8000) - 4000;
			phys.velocity_.z += (rand() % 8000) - 4000;
			phys.velocity_ = { 0, 30000, 0 };

			pull.SetCentre(mesh.position);
			pull.SetIsGlobal(false);
		}

		void AddPulls(GravitationalPull* pulls)
		{
			phys.pulls.push_back(pulls);
		}

		void Update(float deltaTime, const std::vector<SphereCollider*>& sphereColliders)
		{
			coll.SetCentre(mesh.position);
			phys.position_ = mesh.position;
			pull.SetCentre(mesh.position);
			phys.Update(deltaTime);
			mesh.position += phys.velocity_ * deltaTime;

			for (const auto& plan : sphereColliders)
			{
				if (coll.IsColliding(*plan))
				{
					glm::vec3 normal = glm::normalize(coll.GetCentre() - plan->GetCentre());
					const glm::vec3 reflectionVector = phys.velocity_ - (2.0f * glm::dot(phys.velocity_, normal) * normal);
					phys.velocity_ = reflectionVector;
					mesh.position += deltaTime * 2 * phys.velocity_;
				}
			}
		}
	};
	
	std::vector<Orb*> orbs;
	for (int i = 0; i < num; i++)
	{
		auto orb = new Orb();
		renderer.DrawMesh(orb->mesh);
		orbs.push_back(orb);
	}

	moonPhysics.pulls.push_back(&orbs[0]->pull);
	moonPhysics.pulls.push_back(&pull);
	renderer.DrawMesh(moon);

	for (int i = 0; i< num; i++)
	{
		orbs[i]->AddPulls(&pull);
		// for (int j  = 0; j < num; j++)
		// {
		// 	if ( i == j ) continue;
		// 	orbs[i]->AddPulls(&orbs[j]->pull);
		// }
	}

	renderer.DrawMesh(mesh);

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
			moonPhysics.position_ = moon.position; 
			moonCol.SetCentre(moon.position);
			moonPhysics.Update(physicsTime / 100.0f);
			moon.position += moonPhysics.velocity_ * ((float)physicsTime / 100.0f);

			physicsCount -= physicsTime;

			for (Orb* orb : orbs)
			{
				orb->Update((physicsTime / 100.0f), sphereColliders);
			}
		}

		deltaCount += deltaTime;
		frameCount++;
		if (deltaCount >= 1.0) {
			std::cout << "\r" << frameCount;;
			deltaCount -= 1.0;
			frameCount = 0;
		}


		//renderCount += deltaTime;
		//if (renderCount >= renderTime) {
		renderer.Update();
		renderCount -= renderTime;
		//}

		running = window.Update();
	}

	renderer.DestroyMesh(mesh);

	for ( int i = 0; i < num; i++ )
	{
		renderer.DestroyMesh(orbs[i]->mesh);
		delete orbs[i];
	}
	renderer.DestroyMesh(moon);

	return 0;
}
