#pragma once
#include "PearlCore.h"


namespace PEARL_NAMESPACE::typesRender
{
	struct PushConstant
	{
		glm::mat4 mvp;
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
	
	struct BufferResource
	{
		vk::Buffer buffer;
		vk::DeviceMemory memory;
		void* dataPtr;
	};

	struct Mesh
	{
		MeshData data;

		BufferResource vertexResource;
		BufferResource indexResource;

		glm::mat4 modelMatrix = glm::mat4(1.0f);

		PushConstant mvp;

		glm::vec3 position = glm::vec3(0.0f);
		glm::vec3 rotation = glm::vec3(0.0f);
	};
}
