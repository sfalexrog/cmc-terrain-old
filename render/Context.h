#ifndef TERRAIN_CONTEXT_H
#define TERRAIN_CONTEXT_H

#include <SDL_video.h>


class Context {
private:
    SDL_Window *_w;
    SDL_GLContext _ctx;

public:
    Context(SDL_Window *w);
    ~Context();

    void swap();

};


#endif //TERRAIN_CONTEXT_H
