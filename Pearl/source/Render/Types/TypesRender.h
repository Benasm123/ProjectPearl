#pragma once
#include "PearlCore.h"


namespace PEARL_NAMESPACE::typesRender
{
	struct PushConstant
	{
		float x, y, z;
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

	struct Mesh
	{
		MeshData data;
		vk::Buffer vertexBuffer;
		vk::DeviceMemory vertexMemory;
		void* vertexData;
		vk::Buffer indexBuffer;
		vk::DeviceMemory indexMemory;
		void* indexData;
	};
}