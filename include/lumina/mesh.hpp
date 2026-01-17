#pragma once
#include <vector>
#include <memory>

#include <GL/glew.h>

namespace lumina
{
    class Mesh
    {
    private:
        std::shared_ptr<std::vector<float>> positions;
        std::shared_ptr<std::vector<float>> colors;
        std::shared_ptr<std::vector<float>> uvs;

        std::shared_ptr<std::vector<unsigned int>> indices;

        std::vector<float> interleaved_vertices;
        unsigned int vertex_stride = 0;

        unsigned int num_vertices;
        unsigned int num_indices;

        unsigned int EBO = 0, VBO = 0, VAO = 0;

        void interleave()
        {
            interleaved_vertices.clear();
            size_t count = positions->size() / 3;
            vertex_stride = 3 + (colors ? 3 : 0) + (uvs ? 2 : 0);

            for (size_t i = 0; i < count; i++)
            {
                // Position
                interleaved_vertices.push_back((*positions)[i * 3 + 0]);
                interleaved_vertices.push_back((*positions)[i * 3 + 1]);
                interleaved_vertices.push_back((*positions)[i * 3 + 2]);

                // Color (optional)
                if (colors && colors->size() >= i * 3 + 3)
                {
                    interleaved_vertices.push_back((*colors)[i * 3 + 0]);
                    interleaved_vertices.push_back((*colors)[i * 3 + 1]);
                    interleaved_vertices.push_back((*colors)[i * 3 + 2]);
                }

                // UV (optional)
                if (uvs && uvs->size() >= i * 2 + 2)
                {
                    interleaved_vertices.push_back((*uvs)[i * 2 + 0]);
                    interleaved_vertices.push_back((*uvs)[i * 2 + 1]);
                }
            }
        }

        void setup()
        {
            interleave();
            
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
            glGenBuffers(1, &EBO);

            glBindVertexArray(VAO);

            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, interleaved_vertices.size() * sizeof(float), interleaved_vertices.data(), GL_STATIC_DRAW);
            
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices->size() * sizeof(unsigned int), indices->data(), GL_STATIC_DRAW);

            // Layout
            int offset = 0;

            // Position (location = 0)
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertex_stride * sizeof(float), (void*)(offset * sizeof(float)));
            glEnableVertexAttribArray(0);
            offset += 3;

            // Color (location = 1)
            if (colors)
            {
                glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, vertex_stride * sizeof(float), (void*)(offset * sizeof(float)));
                glEnableVertexAttribArray(1);
                offset += 3;
            }

            // UV (location = 2)
            if (uvs)
            {
                glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, vertex_stride * sizeof(float), (void*)(offset * sizeof(float)));
                glEnableVertexAttribArray(2);
                offset += 2;
            }

            // glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        }

    public:
        Mesh(std::shared_ptr<std::vector<float>> positions,
             std::shared_ptr<std::vector<unsigned int>> indices,
             std::shared_ptr<std::vector<float>> colors = nullptr,
             std::shared_ptr<std::vector<float>> uvs = nullptr)
        : positions(positions), indices(indices), colors(colors), uvs(uvs)
        {
            num_vertices = static_cast<unsigned int>(positions->size() / 3);
            num_indices = static_cast<unsigned int>(indices->size());
            setup();
        }

        ~Mesh()
        {
            glDeleteBuffers(1, &VBO);
            glDeleteBuffers(1, &EBO);
            glDeleteVertexArrays(1, &VAO);
        }

        void bind() { glBindVertexArray(VAO); }
        void unbind() { glBindVertexArray(0); }

        void draw() const
        {
            glBindVertexArray(VAO);
            if (indices && !indices->empty())
            {
                glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_INT, 0);
            }
            else
            {
                glDrawArrays(GL_TRIANGLES, 0, num_vertices);
            }
            glBindVertexArray(0);
        }

        std::shared_ptr<std::vector<float>> getPositions() const { return positions; }
        std::shared_ptr<std::vector<float>> getColors() const { return colors; }
        std::shared_ptr<std::vector<float>> getUVs() const { return uvs; }
        std::shared_ptr<std::vector<unsigned int>> getIndices() const { return indices; }

        unsigned int getNumVertices() const { return num_vertices; }
        unsigned int getNumIndices() const { return num_indices; }
    };
}