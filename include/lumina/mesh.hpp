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

        unsigned int num_vertices;
        unsigned int num_indices;

        unsigned int EBO, VBO, VAO;
    public:
        Mesh(std::shared_ptr<std::vector<float>> vertices, unsigned int num_vertices, std::shared_ptr<std::vector<unsigned int>> indices, unsigned int num_indices)
        : vertices(vertices), num_vertices(num_vertices), indices(indices), num_indices(num_indices)
        {
            setupMesh();
        }

        void setupMesh()
        {
            // Create EBO
            // unsigned int EBO;
            glGenBuffers(1, &EBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices->size() * sizeof(unsigned int), indices->data(), GL_STATIC_DRAW);

            // Create VAO and VBO
            // unsigned int VBO, VAO;
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
            glBindVertexArray(VAO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, vertices->size() * sizeof(float), vertices->data(), GL_STATIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        }

        void bind()
        {
            glBindVertexArray(VAO);
        }

        void unbind()
        {
            glBindVertexArray(0);
        }

        std::shared_ptr<std::vector<float>> getVertices() const { return vertices; }
        std::shared_ptr<std::vector<unsigned int>> getIndices() const { return indices; }

        unsigned int getNumVertices() const { return num_vertices; }
        unsigned int getNumIndices() const { return num_indices; }
    };
}