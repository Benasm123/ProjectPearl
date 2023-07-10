#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <memory>
#include "Core/Logger.h"
#include "Render/Generic/Renderer.h"
#include "Render/Vulkan/Renderer/VulkanRenderer2D.h"

#ifdef __cplusplus
extern "C"
#endif

int main(int argc, char* argv[])
{
	spdlog::set_level(spdlog::level::trace);

	pearl::Window window("Pearl Engine", 1200, 1000);
	auto renderer = VulkanRenderer2D(window);

	pearl::typesRender::Mesh mesh;
	mesh.data = utils::GetPlanePoints(100.0f);
	mesh.position.z = 40;
	mesh.position.y = -10;
	
	pearl::typesRender::Mesh mesh2;
	mesh2.data = utils::GetSpherePoints(10.0f, 8);
	mesh2.position.z = 40;
	mesh2.position.y = -10;

	pearl::typesRender::Mesh mesh3;
	mesh3.data = utils::GetSpherePoints(3.0f, 3);
	mesh3.position.x = 0.0f;
	mesh3.position.y = 20.0f;
	mesh3.position.z = 50.0f;

	renderer.DrawMesh(mesh);
	renderer.DrawMesh(mesh2);
	renderer.DrawMesh(mesh3);
	
	float vel = 0.0;

	auto oldTime = std::chrono::system_clock::now();

	bool running = true;
	while (running)
	{
		auto currentTime = std::chrono::system_clock::now();
		double deltaTime = static_cast<double>((std::chrono::duration_cast<std::chrono::microseconds>(currentTime - oldTime)).count()) / 1e+6;
		oldTime = currentTime;

		vel += deltaTime * 9.81f * 10.0f;
		mesh3.position.y -= deltaTime * vel;
		if (mesh3.position.y < 3.0f)
		{
			vel *= -1;
			mesh3.position.y = 3.0f;
		}

		running = window.Update();
		renderer.Update();
	}

	renderer.DestroyMesh(mesh);
	renderer.DestroyMesh(mesh2);
	renderer.DestroyMesh(mesh3);

	return 0;
}
