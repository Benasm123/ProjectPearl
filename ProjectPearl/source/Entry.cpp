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

	pearl::Window window("Pearl Engine", 1000, 1000);
	auto renderer = VulkanRenderer2D(window);

	pearl::typesRender::Mesh mesh;
	mesh.data = utils::GetPlanePoints(3.0f);
	mesh.position.z = 40;
	mesh.position.y = -10;
	
	pearl::typesRender::Mesh mesh2;
	mesh2.data = utils::GetSpherePoints(1.0f, 3);
	mesh2.position.z = 40;
	mesh2.position.y = 0;

	renderer.DrawMesh(mesh);
	renderer.DrawMesh(mesh2);
	
	float vel = 0.0;

	auto oldTime = std::chrono::system_clock::now();

	bool running = true;
	while (running)
	{
		double deltaTime = 0.0f;
		auto currentTime = std::chrono::system_clock::now();
		deltaTime = static_cast<double>((std::chrono::duration_cast<std::chrono::microseconds>(currentTime - oldTime)).count()) / 1e+6;
		oldTime = currentTime;

		running = window.Update();
		renderer.Update();
	}

	renderer.DestroyMesh(mesh);
	renderer.DestroyMesh(mesh2);

	return 0;
}
