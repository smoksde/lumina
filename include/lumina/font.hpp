#pragma once

#include <cstdio>
#include <GL/glew.h>

#define STB_TRUETYPE_IMPLEMENTATION // if cpp then this in cpp
#include "../libs/stb/stb_truetype.h"
#include "../libs/glm/glm.hpp"

#include "defines.hpp"
#include "shader.hpp"

namespace lumina
{
    struct FontVertex
    {
        glm::vec2 position;
        glm::vec2 tex_coords;
    };

    class Font
    {
        public:
        std::shared_ptr<Shader> shader_ptr;

        ~Font()
        {
            if (font_vertex_buffer_data)
            {
                delete[] font_vertex_buffer_data;
                // other buffers should also be deleted
            }
        }
        Font(const char *filename, std::shared_ptr<Shader> shader_ptr)
        : shader_ptr(shader_ptr)
        {
            uint8 ttfBuffer[1 << 20];
            uint8 tmpBitmap[512 * 512];

            fread(ttfBuffer, 1, 1 << 20, fopen(filename, "rb"));

            stbtt_BakeFontBitmap(ttfBuffer, 0, 48.0f, tmpBitmap, 512, 512, 32, 96, cdata);

            glGenTextures(1, &font_texture);
            glBindTexture(GL_TEXTURE_2D, font_texture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 512, 512, 0, GL_RED, GL_UNSIGNED_BYTE, tmpBitmap);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glBindTexture(GL_TEXTURE_2D, 0);

            glGenVertexArrays(1, &font_vao);
            glBindVertexArray(font_vao);
            glGenBuffers(1, &font_vertex_buffer_id);
            glBindBuffer(GL_ARRAY_BUFFER, font_vertex_buffer_id);
            font_vertex_buffer_capacity = 20;
            font_vertex_buffer_data = new FontVertex[font_vertex_buffer_capacity * 6];
            glBufferData(GL_ARRAY_BUFFER, sizeof(FontVertex) * 6 * font_vertex_buffer_capacity, 0, GL_DYNAMIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(FontVertex), 0);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(FontVertex), (const void *)offsetof(FontVertex, tex_coords));
            glBindVertexArray(0);
        }

        glm::vec2 drawString(float x, float y, const char *text)
        {
            glBindVertexArray(font_vao);
            glBindBuffer(GL_ARRAY_BUFFER, font_vertex_buffer_id);

            uint32 len = strlen(text);
            if (font_vertex_buffer_capacity < len)
            {
                font_vertex_buffer_capacity = len;
                glBufferData(GL_ARRAY_BUFFER, sizeof(FontVertex) * 6 * font_vertex_buffer_capacity, 0, GL_DYNAMIC_DRAW);
                delete[] font_vertex_buffer_data;
                font_vertex_buffer_data = new FontVertex[font_vertex_buffer_capacity * 6];
            }

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, font_texture);
            glUniform1i(glGetUniformLocation(shader_ptr->shader_id, "u_texture"), 0);

            FontVertex *vData = font_vertex_buffer_data;
            uint32 numVertices = 0;

            glm::vec2 sumWidth(0.0f, 0.0f);

            while (*text)
            {
                if (*text >= 32 && *text < 128)
                {
                    stbtt_aligned_quad q;
                    stbtt_GetBakedQuad(cdata, 512, 512, *text - 32, &x, &y, &q, 1);

                    // Extracting the width and height
                    // float charWidth = q.x1 - q.x0;
                    // float charHeight = q.y1 - q.y0;

                    sumWidth = glm::vec2(q.x1, q.y0);

                    vData[0].position = glm::vec2(q.x0, q.y0);
                    vData[0].tex_coords = glm::vec2(q.s0, q.t0);
                    vData[1].position = glm::vec2(q.x1, q.y0);
                    vData[1].tex_coords = glm::vec2(q.s1, q.t0);
                    vData[2].position = glm::vec2(q.x1, q.y1);
                    vData[2].tex_coords = glm::vec2(q.s1, q.t1);
                    vData[3].position = glm::vec2(q.x0, q.y1);
                    vData[3].tex_coords = glm::vec2(q.s0, q.t1);
                    vData[4].position = glm::vec2(q.x0, q.y0);
                    vData[4].tex_coords = glm::vec2(q.s0, q.t0);
                    vData[5].position = glm::vec2(q.x1, q.y1);
                    vData[5].tex_coords = glm::vec2(q.s1, q.t1);
                    vData += 6;
                    numVertices += 6;
                }
                ++text;
            }

            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(FontVertex) * numVertices, font_vertex_buffer_data);
            glDrawArrays(GL_TRIANGLES, 0, numVertices);

            return sumWidth;
        }

        private:
        stbtt_bakedchar cdata[96];
        GLuint font_texture;
        GLuint font_vao;
        GLuint font_vertex_buffer_id;
        FontVertex *font_vertex_buffer_data = 0;
        uint32 font_vertex_buffer_capacity;
    };
}



;