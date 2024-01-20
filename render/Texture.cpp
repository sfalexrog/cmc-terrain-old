#include <sstream>
#include <iostream>
#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

using namespace gl33core;

Texture::Texture()
{
    glGenTextures(1, &_id);
}

Texture::~Texture()
{
    glDeleteTextures(1, &_id);
}

void Texture::load(const char* filename)
{
    int channels;
    auto imgdata = stbi_load(filename, &_w, &_h, &channels, 4);
    glBindTexture(GL_TEXTURE_2D, _id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _w, _h, 0, GL_RGBA, GL_UNSIGNED_BYTE, imgdata);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    stbi_image_free(imgdata);
}

GLuint Texture::getId()
{
    return _id;
}

void Texture::loadCubemap(const char *path)
{
    const std::string cubeFaces[] = {
            "cubemap_negX.png",
            "cubemap_negY.png",
            "cubemap_negZ.png",
            "cubemap_posX.png",
            "cubemap_posY.png",
            "cubemap_posZ.png",
    };

    const GLenum faceNames[] = {
            GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
            GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
            GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
            GL_TEXTURE_CUBE_MAP_POSITIVE_X,
            GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
            GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
    };

    glBindTexture(GL_TEXTURE_CUBE_MAP, _id);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    for (int i = 0; i < 6; ++i)
    {
        std::stringstream ss;
        ss << path << "/" << cubeFaces[i];
        int x, y, channels;
        auto data = stbi_load(ss.str().c_str(), &x, &y, &channels, 4);
        if (data == NULL)
        {
            std::cerr << "Could not load " << ss.str() << std::endl;
        }
        glTexImage2D(faceNames[i], 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
    }

}


