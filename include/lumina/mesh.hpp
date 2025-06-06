#pragma once
#include <vector>
#include <memory>

#include <GL/glew.h>

namespace lumina
{
    class Mesh
    {
    private:
        std::shared_ptr<std::vector<float>> vertices;
        std::shared_ptr<std::vector<unsigned int>> indices;

        std::vector<float> interleaved_vertices;

        unsigned int num_vertices;
        unsigned int num_indices;

        unsigned int EBO, VBO, VAO;

        void computeNormals()
        {
            std::vector<glm::vec3> positions;
            positions.reserve(num_vertices);
            for (size_t i = 0; i < num_vertices; i+=3)
            {
                positions.emplace_back((*vertices)[i], (*vertices)[i+1], (*vertices)[i+2]);
            }

            std::vector<glm::vec3> normals(num_vertices, glm::vec3(0.0f));

            for (size_t i = 0; i < indices->size(); i+=3)
            {
                unsigned int i0 = (*indices)[i];
                unsigned int i1 = (*indices)[i+1];
                unsigned int i2 = (*indices)[i+2];
                
                glm::vec3& v0 = positions[i0];
                glm::vec3& v1 = positions[i1];
                glm::vec3& v2 = positions[i2];

                glm::vec3 edge1 = v1 - v0;
                glm::vec3 edge2 = v2 - v0;
                glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));

                normals[i0] += normal;
                normals[i1] += normal;
                normals[i2] += normal;
            }

            interleaved_vertices.clear();
            for (size_t i = 0; i < num_vertices; i++)
            {
                glm::vec3 pos = positions[i];
                glm::vec3 norm = glm::normalize(normals[i]);

                interleaved_vertices.push_back(pos.x);
                interleaved_vertices.push_back(pos.y);
                interleaved_vertices.push_back(pos.z);

                interleaved_vertices.push_back(norm.x);
                interleaved_vertices.push_back(norm.y);
                interleaved_vertices.push_back(norm.z);
            }
        }

        void setupMesh()
        {
            computeNormals();
            
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
            glGenBuffers(1, &EBO);

            glBindVertexArray(VAO);

            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, interleaved_vertices.size() * sizeof(float), interleaved_vertices.data(), GL_STATIC_DRAW);
            
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices->size() * sizeof(unsigned int), indices->data(), GL_STATIC_DRAW);

            // Position attribute (location = 0)
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);
            
            // Normal attribute (location = 1)
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
            glEnableVertexAttribArray(1); 

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        }

    public:
        Mesh(std::shared_ptr<std::vector<float>> vertices,
            unsigned int num_vertices,
            std::shared_ptr<std::vector<unsigned int>> indices,
            unsigned int num_indices)
        : vertices(vertices), num_vertices(num_vertices),
          indices(indices), num_indices(num_indices)
        {
            setupMesh();
        }

        void bind() { glBindVertexArray(VAO); }
        void unbind() { glBindVertexArray(0); }

        std::shared_ptr<std::vector<float>> getVertices() const { return vertices; }
        std::shared_ptr<std::vector<unsigned int>> getIndices() const { return indices; }

        unsigned int getNumVertices() const { return num_vertices; }
        unsigned int getNumIndices() const { return num_indices; }
    };
}