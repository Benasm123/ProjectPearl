#pragma once
#include "PearlCore.h"
#include "Render/Vulkan/BDVK/BDVK_enums.h"


namespace vk {
	class Buffer;
	class DeviceMemory;
}

namespace PEARL_NAMESPACE::typesRender
{
	struct PushConstant
	{
		glm::mat4 mvp;
	};

	struct PushConstantInfo 
	{
		bdvk::ShaderType shaderStage;
		PushConstant data;
	};

	struct Triangle
	{
		uint32_t f1, f2, f3;
	};

	struct VertexInput2D
	{
		glm::vec3 point;
		glm::vec3 normal;
		glm::vec2 textureCoordinate;
	};

	struct MeshData
	{
		std::vector<VertexInput2D> points;
		std::vector<Triangle> triangles;
	};
	
	// TODO -> Remove vk::Buffer Usage and vk::DeviceMemory.
	struct BufferResource
	{
		vk::Buffer buffer;
		vk::DeviceMemory memory;
		void* dataPtr;
	};
}
