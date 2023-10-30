#include "Camera.h"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>


Camera::Camera(const float fov, const glm::vec2 render_area)
	:viewArea_{render_area}
	, fov_{fov}
{
	UpdatePerspective();
	UpdateView();
}


Camera::~Camera()
{
}


void Camera::UpdatePerspective()
{
	perspectiveMatrix_ = glm::perspective(glm::radians(fov_), viewArea_.x / viewArea_.y, 0.1f, 10000.0f);
	perspectiveMatrix_[1][1] *= -1;
}


void Camera::UpdateView()
{
	viewMatrix_ = glm::lookAt(position_, position_ + rotation_, glm::vec3{0, 0, -1});
}