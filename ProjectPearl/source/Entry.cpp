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

	pearl::Window window("Pearl Engine", 1400, 1400);
	// const std::unique_ptr<Renderer> renderer = std::make_unique<VulkanRenderer2D>(window);
	auto renderer = VulkanRenderer2D(window);

	pearl::typesRender::Mesh mesh;
	mesh.data = utils::GetSpherePoints(1, 0);

	renderer.DrawMesh(&mesh);

	bool running = true;
	while (running)
	{
		running = window.Update();
		renderer.Update();
	}

	return 0;
}
