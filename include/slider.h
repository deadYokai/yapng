//
// Created by kitsune on 1/6/24.
//

#include "SDL.h"

#ifndef YAPNG_SLIDER_H
#define YAPNG_SLIDER_H

class slider {
public:
    enum Rot : bool{
        vertical = false,
        horizontal = true
    };
    void Render(SDL_Renderer *sdl_renderer, bool fillbg = true);
    void RegisterEvent(SDL_Event &event);
    void GetMouseState();
    void assets_in(Rot p, SDL_FRect opos);
    void assets_out();

    float getValue(float max = 100);
    void setValue(float val, float max = 100);

    [[maybe_unused]] void posChange(SDL_FRect opos);
private:
    float w{};
    float h{};
    float val = 0;;
    bool rot{};
    SDL_FRect pos{};
    SDL_FRect bg{};
    SDL_FRect button{};
    SDL_FRect butt_bounds{};
    SDL_FPoint mouse{};
    SDL_bool mouse_follow = SDL_FALSE;
    SDL_FPoint mouse_offset{};
};


#endif //YAPNG_SLIDER_H
