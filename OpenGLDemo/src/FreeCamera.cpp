#include "FreeCamera.h"

FreeCamera::FreeCamera(const PerspectiveCamera& perspectiveCamera, const int screenWidth, const int screenHeight, const bool clampY, const float movementSpeed, const float sensitivity)
	: perspectiveCamera(perspectiveCamera), lastX(screenWidth / 2.0f), lastY(screenHeight / 2.0f), yaw(-90.0f), pitch(0.0f), sensitivity(sensitivity), movementSpeed(movementSpeed),
      cameraDirection(0.0f, 0.0f, -1.0f), cameraPosition(perspectiveCamera.getPosition()), firstMouse(true), clampY(clampY)
{
}

void FreeCamera::moveForward(const float timeDelta)
{
	cameraPosition += cameraDirection * movementSpeed * timeDelta;
    updatePerspectiveCamera();
}

void FreeCamera::moveBackward(const float timeDelta)
{
	cameraPosition -= cameraDirection * movementSpeed * timeDelta;
    updatePerspectiveCamera();
}

void FreeCamera::strafeRight(const float timeDelta)
{
	cameraPosition += glm::normalize(glm::cross(cameraDirection, perspectiveCamera.getWorldUp())) * movementSpeed * timeDelta;
    updatePerspectiveCamera();
}

void FreeCamera::strafeLeft(const float timeDelta)
{
	cameraPosition -= glm::normalize(glm::cross(cameraDirection, perspectiveCamera.getWorldUp())) * movementSpeed * timeDelta;
    updatePerspectiveCamera();
}

void FreeCamera::changeCameraDirection(double screenXPos, double screenYPos)
{
    if (firstMouse)
    {
        lastX = screenXPos;
        lastY = screenYPos;
        firstMouse = false;
    }

    float xOffset = screenXPos - lastX;
    float yOffset = screenYPos - lastY;
    lastX = screenXPos;
    lastY = screenYPos;

    xOffset *= sensitivity;
    yOffset *= sensitivity;

    yaw += xOffset;
    pitch -= yOffset;

    //Constrain camera angle to stop flipping
    if (pitch > 89.0f)
        pitch = 89.0f;
    else if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraDirection = glm::normalize(direction);

    updatePerspectiveCamera();
}

void FreeCamera::updatePerspectiveCamera()
{
    if (clampY)
        cameraPosition.y = 0.0f;

    perspectiveCamera.setTarget(cameraDirection + cameraPosition);
    perspectiveCamera.setPostion(cameraPosition);
}
