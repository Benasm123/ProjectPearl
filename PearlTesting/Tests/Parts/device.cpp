#include "pch.h"
#include "../../PearlRenderer/source/Render/Vulkan/GraphicsUnit.h"
#include "../../PearlRenderer/source/Render/Vulkan/RendererInstance.h"

using namespace pearl;

TEST(Devices, Creation) {
	auto layers = vk::enumerateInstanceLayerProperties();
	auto extensions = vk::enumerateInstanceExtensionProperties();

	const Window window("Test", 100, 100);
	const RendererInstance instance(window, {}, {});
	const GraphicsUnit graphicsUnit{instance, instance.Get().enumeratePhysicalDevices().front()};
}

TEST(Devices, InvalidLayers) {
	auto layers = vk::enumerateInstanceLayerProperties();
	auto extensions = vk::enumerateInstanceExtensionProperties();

	const Window window("Test", 100, 100);
	const RendererInstance instance(window, {}, {});
	ASSERT_ANY_THROW(const GraphicsUnit graphicsUnit(instance, nullptr));
}

TEST(Devices, InvalidExtensions) {
	auto layers = vk::enumerateInstanceLayerProperties();
	auto extensions = vk::enumerateInstanceExtensionProperties();

	const Window window("Test", 100, 100);
	const RendererInstance instance(window, {}, {});
	const GraphicsUnit graphicsUnit1{instance, instance.Get().enumeratePhysicalDevices().front()};
	const GraphicsUnit graphicsUnit2{instance, instance.Get().enumeratePhysicalDevices().front()};
}