#pragma once
#include <Render/Vulkan/Renderer/VulkanRenderer.h>
#include <Core/StaticMesh.h>
#include <PhysicsObject.h>
#include <GravitationalPull.h>
#include <Collisions/SphereCollider.h>

namespace pearl {
	struct Transform {
		glm::vec3 position;
		glm::vec3 rotation;
		glm::vec3 scale;
	};
}

class SphereObject {
public:
	SphereObject(VulkanRenderer& renderer, float radius, int precision=1);
	~SphereObject() = default;

	void Update(float deltaTime);
	void PhysicsUpdate(float deltaTime);

	pearl::Transform& Transform() { return transform_; }
	StaticMesh& Mesh() { return mesh_; }
	PhysicsObject& Physics() { return physics_; }
	GravitationalPull& Gravity() { return gravity_; }
	SphereCollider& Collider() { return collider_; }


private:
	pearl::Transform transform_;

	StaticMesh mesh_;
	PhysicsObject physics_;
	GravitationalPull gravity_;
	SphereCollider collider_;
};