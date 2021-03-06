#ifndef PATHTRACER_CAMERA_H
#define PATHTRACER_CAMERA_H

#include <cmath>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "gfx/Window.h"
#include "Config.h"

class Camera {
protected:
    glm::vec3 front, right, up, cameraOffset;
    float mouseSensitivity = 0.3f;

    explicit Camera() {
        this->front = glm::normalize(glm::vec3(
                std::cos(glm::radians(this->yaw)) * std::cos(glm::radians(this->pitch)),
                std::sin(glm::radians(this->pitch)),
                std::sin(glm::radians(this->yaw)) * std::cos(glm::radians(this->pitch))
        ));
        this->right = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), this->front));
        this->up = glm::normalize(glm::cross(this->front, this->right));
        this->cameraOffset = glm::vec3(0.f);
        this->cameraOffset.y = .3f;
    }

    /**
     * Update view matrix
     * @param position position of camera
     */
    void updateView(const glm::vec3& position) {
        this->view = glm::lookAt(position + this->cameraOffset, position + this->cameraOffset + this->front, this->up);
    }

    void updateFrame(Window *game) {
        if (!game->cursorLocked) return; // Return if in UI
        glm::vec2 mouseDelta = game->mouseManager.delta;
        yaw += mouseDelta.x * mouseSensitivity;
        pitch -= mouseDelta.y * mouseSensitivity;

        if (pitch >= 90) pitch = 89.999f;
        if (pitch <= -90) pitch = -89.999f;

        this->front = glm::normalize(glm::vec3(
                cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch)),
                sin(glm::radians(this->pitch)),
                sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch))
        ));
        this->right = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), this->front));
        this->up = glm::normalize(glm::cross(this->front, this->right));
    }

    void resize(int width, int height) {
        this->ortho = glm::ortho(0.0f, (float)width, (float)height, 0.0f);
        this->projection = glm::perspective(glm::radians(Config::FOV), (float)width/(float)height, 0.01f, 100.0f);
    }

public:
    inline glm::mat4 getView() const {
        return this->view;
    }

    inline glm::mat4 getProjection() const {
        return this->projection;
    }

    inline glm::mat4 getOthro() const {
        return this->ortho;
    }

    inline float getYaw() const {
        return this->yaw;
    }

    inline float getPitch() const {
        return this->pitch;
    }

private:
    float yaw = 90.f, pitch = 0.f;
    glm::mat4 view, projection, ortho;
};

#endif //PATHTRACER_CAMERA_H
