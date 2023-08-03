#pragma once
#include "Render/Vulkan/Renderer/VulkanRenderer.h"
#include "Render/Types/TypesRender.h"

class StaticMesh {
public:
	StaticMesh(VulkanRenderer& renderer, pearl::typesRender::MeshData data);
	~StaticMesh();

	glm::vec3 Position() { return position_; }
	void SetPosition(glm::vec3 pos) { position_ = pos; }
	void SetX(float x) { position_.x = x; }
	void SetY(float y) { position_.y = y; }
	void SetZ(float z) { position_.z = z; }

	glm::vec3 Rotation() { return rotation_; }

private:
	VulkanRenderer& renderer_;

	glm::vec3 position_ = glm::vec3(0.0f);
	glm::vec3 rotation_ = glm::vec3(0.0f);

	pearl::typesRender::MeshData data_;

	pearl::typesRender::BufferResource vertexResource_;
	pearl::typesRender::BufferResource indexResource_;

	glm::mat4 modelMatrix_ = glm::mat4(1.0f);

	pearl::typesRender::PushConstant mvp_;

	friend class VulkanRenderer;
	friend class pearl::GraphicsUnit;
	friend class pearl::CommandBuffer;
};
