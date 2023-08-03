#include "pch.h"
#include "../../PearlRenderer/source/Render/Vulkan/Renderer/VulkanRenderer2D.h"

using namespace pearl;

TEST(Renderer, Creation) {
	// HOW IT IS NOW

	{
		const Window window{"Test", 1000, 1000};

		VulkanRenderer renderer{window};

		std::vector<pearl::typesRender::Mesh*> meshes;

		for (int i = 0; i < 10; i++)
		{
			auto mesh = new typesRender::Mesh;
			mesh->data = utils::GetSpherePoints(1.0f, 1);
			meshes.push_back(mesh);
			renderer.DrawMesh(*mesh);
		}

		renderer.Update();

		for ( int i = 0; i < 10; i++ )
		{
			renderer.DestroyMesh(*meshes[i]);
			delete meshes[i];
		}
	}

	// HOW I WANT IT

	const Window window{ "Test", 1000, 1000 };

	VulkanRenderer renderer{ window };

	pearl::typesRender::Mesh mesh;
	mesh.data = utils::GetSpherePoints(1, 1);

	renderer.LoadMesh(mesh);

	for (int i = 0; i < 1000; i++)
	{
		if (i > 300)
		{
			mesh.Disable();
		}

		if (i > 600)
		{
			mesh.Enable();
		}

		if (i > 900)
		{
			mesh.UnloadMesh(mesh);
		}

		renderer.Update();
	}


}

TEST(Renderer, InvalidLayers) {
	auto layers = vk::enumerateInstanceLayerProperties();
	auto extensions = vk::enumerateInstanceExtensionProperties();

	const Window window("Test", 100, 100);
	const RendererInstance instance(window, {}, {});
	ASSERT_ANY_THROW(const GraphicsUnit graphicsUnit(instance, nullptr));
}

TEST(Renderer, InvalidExtensions) {
	auto layers = vk::enumerateInstanceLayerProperties();
	auto extensions = vk::enumerateInstanceExtensionProperties();

	const Window window("Test", 100, 100);
	const RendererInstance instance(window, {}, {});
	const GraphicsUnit graphicsUnit1{ instance, instance.Get().enumeratePhysicalDevices().front() };
	const GraphicsUnit graphicsUnit2{ instance, instance.Get().enumeratePhysicalDevices().front() };
}