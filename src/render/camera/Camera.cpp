//
// Created by finnw on 31.05.2023.
//

#include "Camera.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "util/Log.hpp"

#define CLAMP_ANGLE(angle) (((int)(angle * 100) % 36000) / 100.0F)
#define CLAMP_YAW(yaw) CLAMP_ANGLE(yaw)
#define CLAMP_PITCH(pitch) pitch < -89.99f ? -89.99f : (pitch > 89.99f ? 89.99f : pitch)

using namespace pmlike::render;

Camera::Camera(bool isPerspective) {
    this->isPerspective = isPerspective;
    this->dirtyProjection = true;
}

Camera::~Camera() = default;

void Camera::setPosition(glm::vec3 pPosition) {
    this->position = pPosition;
    this->dirtyDirection = true;
}

void Camera::move(glm::vec3 pDirection) {
    this->position += pDirection;
    this->dirtyDirection = true;
}

void Camera::setYaw(float pYaw) {
    this->yaw = CLAMP_YAW(pYaw);
    this->dirtyDirection = true;
}

void Camera::setYawRelative(float pYaw) {
    this->setYaw(this->yaw + pYaw);
}

void Camera::setPitch(float pPitch) {
    this->pitch = CLAMP_PITCH(pPitch);
    this->dirtyDirection = true;
}

void Camera::setPitchRelative(float pPitch) {
    this->setPitch(this->pitch + pPitch);
}

void Camera::setAspectRatio(float pAspectRatio) {
    this->aspectRatio = pAspectRatio;
}

void Camera::setFieldOfView(float pFov) {
    this->fov = glm::radians(pFov);
    this->dirtyProjection = true;
}

void Camera::setNearPlane(float pNearPlane) {
    this->nearPlane = pNearPlane;
    this->dirtyProjection = true;
}

void Camera::setFarPlane(float pFarPlane) {
    this->farPlane = pFarPlane;
    this->dirtyProjection = true;
}

float Camera::getYaw() {
    return this->yaw;
}

float Camera::getPitch() {
    return this->pitch;
}

float Camera::getFieldOfView() {
    return this->fov;
}

glm::vec3 Camera::getDirection() {
    return this->front;
}

void Camera::lookAt(glm::vec3 pPosition) {
    glm::vec3 direction = glm::normalize(pPosition - this->position);
    this->setDirection(direction);
    this->dirtyDirection = true;
}

void Camera::setDirection(glm::vec3 direction) {
    this->front = glm::normalize(direction);
    this->right = glm::normalize(glm::cross(this->front, glm::vec3(0.0F, 1.0F, 0.0F)));
    this->up = glm::normalize(glm::cross(this->right, this->front));
    this->yaw = CLAMP_YAW(glm::degrees(atan2(this->front.x, this->front.z)));
    this->pitch = CLAMP_PITCH(glm::degrees(asin(this->front.y)));
}

void Camera::update() {
    if(dirtyDirection) {
        this->front.x = glm::cos(glm::radians(this->yaw)) * glm::cos(glm::radians(this->pitch));
        this->front.y = glm::sin(glm::radians(this->pitch));
        this->front.z = glm::sin(glm::radians(this->yaw)) * glm::cos(glm::radians(this->pitch));

        this->front = glm::normalize(this->front);
        this->right = glm::normalize(glm::cross(this->front, glm::vec3(0.0F, 1.0F, 0.0F)));
        this->up = glm::normalize(glm::cross(this->right, this->front));
        this->viewMatrix = glm::lookAt(this->position, this->position + this->front, this->up);
        this->dirtyDirection = false;

        if(!dirtyProjection) {
            this->frustum = Frustum(this->projectionMatrix * this->viewMatrix);
        }
    }

    if(dirtyProjection) {
        if(this->isPerspective) {
            this->projectionMatrix = glm::perspective(this->fov, this->aspectRatio, this->nearPlane, this->farPlane);
        } else {
            this->projectionMatrix = glm::ortho(-this->aspectRatio, this->aspectRatio, -1.0F, 1.0F, this->nearPlane, this->farPlane);
        }
        this->dirtyProjection = false;
        this->frustum = Frustum(this->projectionMatrix * this->viewMatrix);
    }

}
