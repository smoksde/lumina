#pragma once

#include "../libs/glm/glm.hpp"
#include "../libs/glm/ext/matrix_transform.hpp"
#include "../libs/glm/gtc/matrix_transform.hpp"
#include "../libs/glm/gtc/type_ptr.hpp"

#include <cmath>

namespace lumina
{

    class Camera
    {
        public:
        Camera()
        {

        }

        virtual glm::mat4 getViewMatrix() = 0;
        virtual glm::mat4 getProjectionMatrix() = 0;
        virtual void processKeyboard(float delta_time) {};
        virtual void processMouseMovement(float x_offset, float y_offset, bool constrain_pitch = true) {};
        virtual void processMouseScroll(float y_offset) {};
        private:
    };
    // TODO: Move input logic into Camera2D
    class Camera2D : public Camera
    {
        public:

        float width;
        float height;
        float zoom;
        glm::mat4 view;
        glm::mat4 projection;
        glm::vec3 position;

        Camera2D(float width, float height)
        : width(width), height(height)
        {
            zoom = 1.0f;
            projection = glm::ortho(-zoom * width / 2.0f, 
                                    zoom * width / 2.0f,
                                    -zoom * height / 2.0f,
                                    zoom * height / 2.0f,
                                    -1.0f,
                                    1000.0f
                                    );
        }

        void translate(glm::vec3 translation_vector)
        {
            position += translation_vector * -1.0f;
        }

        glm::mat4 getViewMatrix() override
        {
            return glm::mat4(1.0f);
        }

        glm::mat4 getProjectionMatrix() override
        {
            return glm::ortho(0.0f, (float)width, (float)height, 0.0f);
        }

        void processKeyboard(float delta_time) override
        {
            return;
        }

        void processMouseMovement(float x_offset, float y_offset, bool constrain_pitch = true) override
        {
            return;
        }

        void processMouseScroll(float y_offset) override
        {
            return;
        }

        float getX(){return position[0];}
        
        float getY(){return position[1];}
        
        float getZ(){return position[2];}
    };

    class Camera3D : public Camera
    {
    public:
        glm::vec3 position;
        glm::vec3 front;
        glm::vec3 up;
        glm::vec3 right;
        glm::vec3 world_up;

        float fov;
        float aspect_ratio;
        float near_clip;
        float far_clip;

        float movement_speed;
        float mouse_sensitivity;
        float pitch;
        float yaw;

        glm::vec3 velocity = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 velocity_change = glm::vec3(0.0f, 0.0f, 0.0f);
        float velocity_damper_factor = 0.05f;
        float velocity_change_factor = 0.005f;

        Camera3D(glm::vec3 init_position, glm::vec3 init_front, glm::vec3 init_up, float init_fov, float init_aspect_ratio, float init_near_clip, float init_far_clip, 
        float init_movement_speed = 2.5f, float init_mouse_sensitivity = 0.1f)
        : position(init_position), front(init_front), up(init_up), world_up(init_up),
        fov(init_fov), aspect_ratio(init_aspect_ratio), near_clip(init_near_clip), far_clip(init_far_clip),
        movement_speed(init_movement_speed), mouse_sensitivity(init_mouse_sensitivity), pitch(0.0f), yaw(-90.0f)
        {

        }

        glm::mat4 getViewMatrix() override
        {
            return glm::lookAt(position, position + front, up);
        }

        glm::mat4 getProjectionMatrix() override
        {
            return glm::perspective(glm::radians(fov), aspect_ratio, near_clip, far_clip);
        }

        void processKeyboard(float delta_time) override
        {
            velocity_change = glm::vec3(0.0f, 0.0f, 0.0f);

            const Uint8* state = SDL_GetKeyboardState(NULL);

            if (state[SDL_SCANCODE_W]) {
                glm::vec3 proj_front = front;
                proj_front.y = 0.0f;
                proj_front = glm::normalize(proj_front);
                velocity_change += proj_front;
            }
            if (state[SDL_SCANCODE_S]) {
                glm::vec3 proj_front = front;
                proj_front.y = 0.0f;
                proj_front = glm::normalize(proj_front);
                velocity_change -= proj_front;
            }
            if (state[SDL_SCANCODE_A]) {
                velocity_change -= right;
            }
            if (state[SDL_SCANCODE_D]) {
                velocity_change += right;
            }
            if (state[SDL_SCANCODE_SPACE]) {
                velocity_change += world_up;
            }
            if (state[SDL_SCANCODE_LSHIFT]) {
                velocity_change -= world_up;
            }

            velocity += velocity_change * delta_time * velocity_change_factor;
            velocity *= velocity_damper_factor * delta_time;
            position += velocity;
        }


        void processMouseMovement(float x_offset, float y_offset, bool constrain_pitch = true) override
        {
            x_offset *= mouse_sensitivity;
            y_offset *= mouse_sensitivity;

            yaw += x_offset;
            pitch += y_offset;

            if (constrain_pitch)
            {
                if (pitch > 89.0f)
                    pitch = 89.0f;
                if (pitch < -89.0f)
                    pitch = -89.0f;
            }

            updateCameraVectors();
        }

        void processMouseScroll(float y_offset) override
        {
            fov -= y_offset;
            if (fov < 1.0f)
                fov = 1.0f;
            if (fov > 45.0f)
                fov = 45.0f;
        }

    private:
        void updateCameraVectors()
        {
            glm::vec3 front_temp;
            front_temp.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
            front_temp.y = sin(glm::radians(pitch));
            front_temp.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
            front = glm::normalize(front_temp);
            right = glm::normalize(glm::cross(front, world_up));
            up = glm::normalize(glm::cross(right, front));
        }
    };
}

