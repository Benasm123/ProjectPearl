#include "SphereObject.h"

SphereObject::SphereObject(VulkanRenderer& renderer, float radius, int precision)
	: mesh_(renderer, utils::GetSpherePoints(radius, precision))
	, physics_()
	, gravity_(physics_)
	, collider_(radius)
{
	physics_.mass_ = 6.0e11f;
	gravity_.SetCentre(mesh_.Position());
	gravity_.SetIsGlobal(false);
}

void SphereObject::Update(float deltaTime)
{
}

void SphereObject::PhysicsUpdate(float deltaTime)
{
	mesh_.SetPosition(transform_.position);
	physics_.position_ = transform_.position;
	gravity_.SetCentre(transform_.position);
	collider_.SetCentre(transform_.position);

	physics_.Update(deltaTime);
	transform_.position += physics_.velocity_ * deltaTime;
}
