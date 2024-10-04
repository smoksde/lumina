#pragma once

#include <list>
#include <memory>

#include "../libs/glm/glm.hpp"
#include "../libs/glm/ext/matrix_transform.hpp"
#include "../libs/glm/gtc/matrix_transform.hpp"
#include "../libs/glm/gtc/type_ptr.hpp"

#include "mesh.hpp"
#include "shader.hpp"
#include "camera.hpp"

#include <iostream>

namespace lumina
{
    class Node : public std::enable_shared_from_this<Node>
    {
    public:
        std::list<std::shared_ptr<Node>> childs;
        std::shared_ptr<Mesh> mesh;
        std::shared_ptr<Shader> shader;
        std::shared_ptr<Camera> camera; // Adjust such that its only Camera

        glm::vec3 position;
        glm::vec3 rotation;
        glm::vec3 scale;

        std::shared_ptr<Node> parent;

        glm::mat4 global_transform;

        Node(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, std::shared_ptr<Mesh> mesh, std::shared_ptr<Shader> shader, std::shared_ptr<Camera> camera)
        : position(position), rotation(rotation), scale(scale), mesh(mesh), shader(shader), camera(camera)
        {

        }

        void update()
        {
            computeGlobalTransform();

            for (auto child : childs)
            {
                child->update();
            }
        }

        void render()
        {
            if (mesh && shader)
            {
                shader->bind();
                glm::mat4 model = global_transform;
                glm::mat4 view = camera->getViewMatrix();
                glm::mat4 projection = camera->getProjectionMatrix();
                glUniformMatrix4fv(glGetUniformLocation(shader->shader_id, "model"), 1, GL_FALSE, glm::value_ptr(model));
                glUniformMatrix4fv(glGetUniformLocation(shader->shader_id, "view"), 1, GL_FALSE, glm::value_ptr(view));
                glUniformMatrix4fv(glGetUniformLocation(shader->shader_id, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
                mesh->bind();
                glDrawElements(GL_TRIANGLES, mesh->getNumIndices(), GL_UNSIGNED_INT, 0);
                mesh->unbind();
                shader->unbind();
            }
            
            for (auto child : childs)
            {
                child->render();
            }
        }

        void addChild(std::shared_ptr<Node> child)
        {
            child->parent = shared_from_this();
            childs.push_back(child);
        }

        void removeChild(std::shared_ptr<Node> child)
        {
            childs.remove(child);
        }

        void updateMesh(std::shared_ptr<Mesh> new_mesh)
        {
            mesh = std::move(new_mesh);
        }

        void setPosition(glm::vec3 new_position)
        {
            position = new_position;
        }

        void setRotation(glm::vec3 new_rotation)
        {
            rotation = new_rotation;
        }

        void setScale(glm::vec3 new_scale)
        {
            scale = new_scale;
        }

        glm::mat4 getLocalTransform()
        {
            glm::mat4 local_transform = glm::mat4(1.0f);

            local_transform = glm::translate(local_transform, position);
            
            local_transform = glm::rotate(local_transform, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
            local_transform = glm::rotate(local_transform, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
            local_transform = glm::rotate(local_transform, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

        
            local_transform = glm::scale(local_transform, scale);

            return local_transform;
        }

        void computeGlobalTransform()
        {
            /*is_dirty = true;
            if (is_dirty)
            {
                if (auto parent_ptr = parent.lock())
                {
                    global_transform = parent_ptr->global_transform * getLocalTransform();
                }
                else
                {
                    global_transform = getLocalTransform();
                }
                is_dirty = false;
            }*/
            if (parent)
            {
                global_transform = parent->global_transform * getLocalTransform();
            }
            else
            {
                global_transform = getLocalTransform();
            }
        }
    private:
    };
}