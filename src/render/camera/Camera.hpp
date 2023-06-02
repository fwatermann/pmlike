//
// Created by finnw on 31.05.2023.
//

#ifndef PM_LIKE_CAMERA_HPP
#define PM_LIKE_CAMERA_HPP

#include "glm/glm.hpp"
#include "Frustum.hpp"

namespace pmlike::render {

    class Camera {

        public:
            explicit Camera(bool isPerspective = true);

            ~Camera();

            void setPosition(glm::vec3 pPosition);

            void move(glm::vec3 pDirection);

            void setYaw(float pYaw);

            void setYawRelative(float pYaw);

            void setPitch(float pPitch);

            void setPitchRelative(float pPitch);

            void setAspectRatio(float pAspectRatio);

            void setFieldOfView(float pFov);

            void setNearPlane(float pNearPlane);

            void setFarPlane(float pFarPlane);

            float getYaw();

            float getPitch();

            float getFieldOfView();

            glm::vec3 getDirection();

            void lookAt(glm::vec3 pPosition);

            void setDirection(glm::vec3 direction);

            void update();

            glm::mat4 viewMatrix = glm::mat4();
            glm::mat4 projectionMatrix = glm::mat4();
            glm::vec3 up = glm::vec3(0, 1, 0);
            glm::vec3 right = glm::vec3(1, 0, 0);
            glm::vec3 front = glm::vec3(0, 0, 1);
            glm::vec3 position = glm::vec3(0, 0, 0);

            Frustum frustum;

        private:
            bool dirtyDirection = true;
            bool dirtyProjection = true;

            bool isPerspective = true;

            float yaw = 0.0F;
            float pitch = 0.0F;

            float fov = glm::radians(70.0F);
            float aspectRatio = 16.0F / 9.0F;
            float nearPlane = 0.0001F;
            float farPlane = 1000.0F;

    };


};


#endif //PM_LIKE_CAMERA_HPP
