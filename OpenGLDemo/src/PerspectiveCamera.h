#pragma once

#include <glm/glm.hpp>

class PerspectiveCamera
{
public:
	PerspectiveCamera(float FOV, float aspectRatio, float nearPlane, float farPlane,
		const glm::vec3& cameraPosition = { 0.0f, 0.0f, 0.0f },
		const glm::vec3& cameraTarget = { 0.0f, 0.0f, 0.0f },
		const glm::vec3& worldUp = { 0.0f, 1.0f, 0.0f });
	
	inline const glm::vec3& getPosition() const { return cameraPosition; }
	inline void setPostion(const glm::vec3& position) { cameraPosition = position; recalculateViewProjectionMatrices(); }

	inline const glm::vec3& getTarget() const { return cameraTarget; }
	inline void setTarget(const glm::vec3& target) { cameraTarget = target; recalculateViewProjectionMatrices(); }

	inline const glm::vec3& getWorldUp() const { return worldSpaceUp; }
	inline void setWorldUp(const glm::vec3& worldUp) { worldSpaceUp = worldUp; recalculateViewProjectionMatrices(); }

	inline const glm::mat4& getViewMatrix() const { return viewMatrix; }
	inline const glm::mat4& getProjectionMatrix() const { return projectionMatrix; }
	inline const glm::mat4& getProjectionViewMatrix() const { return projectionViewMatrix; }

private:
	void recalculateViewProjectionMatrices();

private:
	glm::mat4 viewMatrix;
	glm::mat4 projectionMatrix;
	glm::mat4 projectionViewMatrix;

	glm::vec3 cameraPosition;
	glm::vec3 cameraTarget;
	glm::vec3 worldSpaceUp;
};