#pragma once
#include <string>
#include <GL/glew.h>

#include "../libs/stb/stb_image.h"

namespace lumina {

class Texture
{
private:
    GLuint texture_id = 0;
    int width = 0;
    int height = 0;

public:
    Texture() = default;

    // Load from file
    bool loadFromFile(const std::string& path)
    {
        // Use your preferred image loader (stb_image, etc.)
        int n;
        unsigned char* data = stbi_load(path.c_str(), &width, &height, &n, 4);
        if (!data) return false;

        glGenTextures(1, &texture_id);
        glBindTexture(GL_TEXTURE_2D, texture_id);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
                     GL_RGBA, GL_UNSIGNED_BYTE, data);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glBindTexture(GL_TEXTURE_2D, 0);
        stbi_image_free(data);
        return true;
    }

    void bind(int unit = 0) const
    {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, texture_id);
    }

    void unbind() const
    {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    int getWidth() const { return width; }
    int getHeight() const { return height; }
    GLuint getID() const { return texture_id; }

    ~Texture()
    {
        if (texture_id != 0)
            glDeleteTextures(1, &texture_id);
    }
};

}
