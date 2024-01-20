#ifndef TERRAIN_TEXTURE_H
#define TERRAIN_TEXTURE_H

#include <glbinding/gl33core/gl.h>

class Texture {

private:
    gl::GLuint _id;
    int _w, _h;

public:
    Texture();
    ~Texture();

    void load(const char* fileName);

    gl::GLuint getId();

    void loadCubemap(const char* path);
};


#endif //TERRAIN_TEXTURE_H
