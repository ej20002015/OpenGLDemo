#include "PerspectiveCamera.h"

#include "glm/gtc/matrix_transform.hpp"

PerspectiveCamera::PerspectiveCamera(float FOV, float aspectRatio, float nearPlane, float farPlane,
	const glm::vec3& cameraPosition, const glm::vec3& cameraTarget, const glm::vec3& worldUp)
	: viewMatrix(glm::lookAt(cameraPosition, cameraTarget, worldUp)), projectionMatrix(glm::perspective(FOV, aspectRatio, nearPlane, farPlane)),
	  projectionViewMatrix(projectionMatrix * viewMatrix), cameraPosition(cameraPosition), cameraTarget(cameraTarget), worldSpaceUp(worldUp)
{
}

void PerspectiveCamera::recalculateViewProjectionMatrices()
{
	viewMatrix = glm::lookAt(cameraPosition, cameraTarget, worldSpaceUp);
	projectionViewMatrix = projectionMatrix * viewMatrix;
}
