#include "pch.h"
#include "..\..\Pearl\source\Render\Vulkan\RendererInstance.h"
#include "../../Pearl/source/Render/Vulkan/GraphicsUnit.h"

using namespace pearl;

TEST(Instances, Creations) {
	auto layers = vk::enumerateInstanceLayerProperties();
	auto extensions = vk::enumerateInstanceExtensionProperties();

	const Window window("Test", 100, 100);
	const RendererInstance instance1(window, {},{});
	ASSERT_TRUE(instance1.Get());


	const RendererInstance instance2(window, {layers.front().layerName}, {});
	ASSERT_TRUE(instance2.Get());


	const RendererInstance instance3(window, {}, {extensions.front().extensionName});
	ASSERT_TRUE(instance3.Get());


	const RendererInstance instance4(window, {layers.front().layerName}, {extensions.front().extensionName});
	ASSERT_TRUE(instance4.Get());
}

TEST(Instances, IInvalidLayers) {
	auto layers = vk::enumerateInstanceLayerProperties();
	auto extensions = vk::enumerateInstanceExtensionProperties();

	const Window window("Test", 100, 100);
	ASSERT_THROW(const RendererInstance instance(window, { "InvalidLayer" }, {}), std::exception);
	ASSERT_THROW(const RendererInstance instance(window, { layers.front().layerName, "InvalidLayer"}, {}), std::exception);
	ASSERT_THROW(const RendererInstance instance(window, { "InvalidLayer" }, {extensions.front().extensionName}), std::exception);
}

TEST(Instances, InvalidExtensions) {
	auto layers = vk::enumerateInstanceLayerProperties();
	auto extensions = vk::enumerateInstanceExtensionProperties();

	const Window window("Test", 100, 100);
	ASSERT_THROW(const RendererInstance instance(window, { }, { "InvalidExtension" }), std::exception);
	ASSERT_THROW(const RendererInstance instance(window, { layers.front().layerName }, { "InvalidExtension" }), std::exception);
	ASSERT_THROW(const RendererInstance instance(window, { layers.front().layerName }, { extensions.front().extensionName, "InvalidExtension" }), std::exception);
}