#pragma once

#include "glm/glm.hpp"
#include "PerspectiveCamera.h"

class FreeCamera
{
public:
	FreeCamera(const PerspectiveCamera& perspectiveCamera, const int screenWidth, const int screenHeight, const bool clampY = false, const float movementSpeed = 1.0f, const float sensitivity = 0.2f);

	void moveForward(const float timeDelta);
	void moveBackward(const float timeDelta);
	void strafeRight(const float timeDelta);
	void strafeLeft(const float timeDelta);

	void changeCameraDirection(double screenXPos, double screenYPos);

	inline const float getMovementSpeed() const { return movementSpeed; }
	inline void setMovementSpeed(const float speed) { movementSpeed = speed; }

	inline const float getSensitivity() const { return sensitivity; }
	inline void setSensitivity(const float sensitivity) { this->sensitivity = sensitivity; }

	inline const glm::mat4& getProjectionViewMatrix() const { return perspectiveCamera.getProjectionViewMatrix(); }

private:
	void updatePerspectiveCamera();

private:
	PerspectiveCamera perspectiveCamera;
	float lastX, lastY;
	float yaw, pitch;
	float sensitivity;
	float movementSpeed;
	glm::vec3 cameraDirection, cameraPosition;
	bool firstMouse;
	bool clampY;
};