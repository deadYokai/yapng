//
// Created by kitsune on 1/6/24.
//

#include "SDL.h"
#ifndef YAPNG_SLIDER_H
#define YAPNG_SLIDER_H

class slider {
public:
    void Render(SDL_Renderer *sdl_renderer);
    void RegisterEvent(SDL_Event &event);
    void GetMouseState();
    void assets_in();
    void assets_out();
private:
    float w;
    float h;
    SDL_Surface* tmp;
//    SDL_Texture* bg;
//    SDL_Texture* button;
    SDL_FRect bg;
    SDL_FRect button;
    SDL_Rect butt_dst;
    SDL_FRect butt_bounds;
    SDL_Rect bg_dst;
    SDL_FPoint mouse;
    SDL_bool mouse_follow = SDL_FALSE;
    SDL_FPoint mouse_offset;
};


#endif //YAPNG_SLIDER_H
