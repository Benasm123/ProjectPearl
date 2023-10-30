#pragma once
#include "PearlCore.h"
#include "Window/RenderSurface.h"


class Camera
{
public:
	Camera(float fov, glm::vec2 render_area);
	~Camera();

	void SetViewArea(const glm::vec2 viewArea) { viewArea_ = viewArea; }
	void UpdatePerspective();
	void UpdateView();

	[[nodiscard]] glm::mat4 GetPerspective() const { return perspectiveMatrix_; }
	[[nodiscard]] glm::mat4 GetView() const { return viewMatrix_; }
	[[nodiscard]] glm::vec3 GetPosition() const { return position_; }
	[[nodiscard]] glm::vec3 GetRotation() const { return rotation_; }

	void SetPosition(const glm::vec3 new_position) { position_ = new_position; }

private:
	glm::mat4 perspectiveMatrix_{};
	glm::mat4 viewMatrix_{};

	glm::vec3 position_{0, 0, 0};
	glm::vec3 rotation_{0, -1, 0};

	glm::vec2 viewArea_{};

	float fov_{};
};
