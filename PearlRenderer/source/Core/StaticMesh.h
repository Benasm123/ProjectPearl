#pragma once
#include "Render/Vulkan/Renderer/VulkanRenderer.h"
#include "Render/Types/TypesRender.h"

class StaticMesh {
public:
	StaticMesh(const VulkanRenderer& renderer, pearl::typesRender::MeshData data);
	~StaticMesh();

private:
	const VulkanRenderer& renderer_;

	glm::vec3 position_ = glm::vec3(0.0f);
	glm::vec3 rotation_ = glm::vec3(0.0f);

	pearl::typesRender::MeshData data_;

	pearl::typesRender::BufferResource vertexResource_;
	pearl::typesRender::BufferResource indexResource_;

	glm::mat4 modelMatrix_ = glm::mat4(1.0f);

	pearl::typesRender::PushConstant mvp_;
};
