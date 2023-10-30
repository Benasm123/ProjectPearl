#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <memory>
#include <glm/ext/scalar_constants.hpp>

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

constexpr double target_fps = 3000;
constexpr double render_time = 1.0 / target_fps;

constexpr double target_hz = 10000.0;
constexpr double physics_time = 1.0 / target_hz;


int main(int argc, char* argv[])
{
	int num = 10;

	spdlog::set_level(spdlog::level::trace);

	pearl::Window window("Pearl Engine", 1200, 1000);
	Camera camera{ 45.0f, {1200, 1000} };

	auto renderer = VulkanRenderer(window, &camera);

	int grid_points = 5;
	auto original_points = utils::GetGridPlanePointsBD(1.0f, grid_points);

	// TEST
	StaticMesh grid_floor(renderer, original_points);
	grid_floor.SetPosition({ -4, -20, 0 });

	auto removed_points = utils::GetGridReduced(original_points, grid_points, 2);
	StaticMesh grid_floor_removed(renderer, removed_points);
	grid_floor_removed.SetPosition({ 4, -20, 0 });

	LOG_INFO("WENT FROM {} TO {}", original_points.triangles.size(), removed_points.triangles.size());


	// Write a function to get the points of a sphere.

	// //FLOOR
	StaticMesh floor_plane(renderer, utils::GetPlanePoints(100));
	floor_plane.SetPosition({ 0, -10, 0 });
	
	//PLANET
	SphereObject player{ renderer, 0.1f, 2 };
	player.Transform().position = { -4.0f, -10.0f, 0.0f };
	//
	// //ENEMIES
	std::vector<SphereObject*> enemies(100);
	for (int i = 0; i < 100; i++)
	{
		auto enemy = new SphereObject(renderer, 0.05f, 1);
		float random = rand() % static_cast<int>(glm::pi<float>() * 200.0f);
		float random_dist = (rand() % 100) / 100.0f;
		enemy->Transform().position = {
			(4 + random_dist) * cos(random),
			-10,
			(4 + random_dist) * sin(random),
		};
		enemy->Physics().velocity_ = glm::normalize(player.Transform().position - enemy->Transform().position) * 200.0f * 0.001f;
	
		enemies[i] = (enemy);
	}

	//HEALTH
	auto health_data = utils::GetPlanePoints(1.0f);
	for (auto& [point, normal, textureCoordinate] : health_data.points)
	{
		normal = { 100.0f, 0.0f, 0.0f };
	}
	StaticMesh health_plane(renderer, health_data);
	health_plane.SetPosition({ 300, -9, 3 });

	// TODO -> Make a clock class to manage time.
	auto oldTime = std::chrono::system_clock::now();

	uint32_t frame_count = 0;
	double delta_count = 0.0;
	double render_count = 0.0;
	double physics_count = 0.0;

	bool key_pressed[4] = {0, 0, 0, 0};

	std::function<void(SDL_Event)> function = [&key_pressed](const SDL_Event& event)
	{
		if (event.type == SDL_KEYDOWN)
		{
			if (event.key.keysym.sym == SDLK_w)
			{
				key_pressed[0] = true;
			}
			if (event.key.keysym.sym == SDLK_a)
			{
				key_pressed[1] = true;
			}
			if (event.key.keysym.sym == SDLK_s)
			{
				key_pressed[2] = true;
			}
			if (event.key.keysym.sym == SDLK_d)
			{
				key_pressed[3] = true;
			}
		}

		if (event.type == SDL_KEYUP)
		{
			if (event.key.keysym.sym == SDLK_w)
			{
				key_pressed[0] = false;
			}
			if (event.key.keysym.sym == SDLK_a)
			{
				key_pressed[1] = false;
			}
			if (event.key.keysym.sym == SDLK_s)
			{
				key_pressed[2] = false;
			}
			if (event.key.keysym.sym == SDLK_d)
			{
				key_pressed[3] = false;
			}
		}
	};

	window.AddEventCallback(&function);

	bool running = true;
	while (running)
	{
		// Get DeltaTime, if its too quick return
		auto current_time = std::chrono::system_clock::now();
		double delta_time = static_cast<double>((std::chrono::duration_cast<std::chrono::microseconds>(
			current_time - oldTime)).count()) / 1e+6;
		if (delta_time == 0.0) continue;
		if (delta_time > 0.1) delta_time = 0.1;
		oldTime = current_time;

		// Physics Update
		physics_count += delta_time;
		while (physics_count >= physics_time)
		{
			player.PhysicsUpdate((float)physics_time);
			for (auto enemy : enemies)
			{
				enemy->PhysicsUpdate((float)physics_time);
				if (enemy->Collider().IsColliding(player.Collider()))
				{
					float random = rand() % static_cast<int>(glm::pi<float>() * 200.0f);
					float random_dist = (rand() % 100) / 100.0f;
					enemy->Transform().position = {
						(4 + random_dist) * cos(random),
						-10,
						(4 + random_dist) * sin(random),
					};
					health_plane.SetPosition(health_plane.Position() + glm::vec3(0.0f, 0.0f, 0.01f));
				}
			
				enemy->Physics().velocity_ = glm::normalize(player.Transform().position - enemy->Transform().position) * (200.0f) * 0.001f;
			}

			physics_count -= physics_time;

			if (key_pressed[0])
			{
				player.Transform().position += glm::vec3(0.0f, 0.0f, -1.0f) * static_cast<float>(physics_time);
			}
			if (key_pressed[1])
			{
				player.Transform().position += glm::vec3(-1.0f, 0.0f, 0.0f) * static_cast<float>(physics_time);
			}
			if (key_pressed[2])
			{
				player.Transform().position += glm::vec3(0.0f, 0.0f, 1.0f) * static_cast<float>(physics_time);
			}
			if (key_pressed[3])
			{
				player.Transform().position += glm::vec3(1.0f, 0.0f, 0.0f) * static_cast<float>(physics_time);
			}
		}

		glm::vec3 old_position = camera.GetPosition();
		old_position.x = player.Transform().position.x;
		old_position.z = player.Transform().position.z;
		camera.SetPosition(old_position);
		camera.UpdateView();

		// Printing DeltaTime;
		delta_count += delta_time;
		frame_count++;
		if (delta_count >= 1.0)
		{
			std::cout << "\r" << frame_count;
			delta_count -= 1.0;
			frame_count = 0;
		}

		// Render Update
		render_count += delta_time;
		if (render_count >= render_time)
		{
			renderer.Update();
		
			render_count -= render_time;
		}

		running = window.Update();
	}

	for (auto enemy : enemies)
	{
		delete enemy;
	}

	return 0;
}
