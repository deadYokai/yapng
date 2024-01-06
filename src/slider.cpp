//
// Created by kitsune on 1/6/24.
//

#include "slider.h"

void slider::Render(SDL_Renderer *sdl_renderer) {
    if(this->mouse_follow){
        this->button.y = this->mouse.y - this->mouse_offset.y;

        if(this->button.y <= this->butt_bounds.y) {
            this->button.y = this->butt_bounds.y;
        }

        if(this->button.y + this->button.h >= this->butt_bounds.y + this->butt_bounds.h) {
            this->button.y = (this->butt_bounds.y + this->butt_bounds.h) - this->button.h;
        }

    }

    SDL_SetRenderDrawColor(sdl_renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(sdl_renderer, &this->bg);
    SDL_SetRenderDrawColor(sdl_renderer, 0, 255, 0, 255);
    SDL_RenderFillRect(sdl_renderer, &this->button);


}

void slider::GetMouseState() {
    SDL_GetMouseState(&this->mouse.x, &this->mouse.y);
}

void slider::RegisterEvent(SDL_Event &event) {
    if(event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
        if (event.button.button == SDL_BUTTON_LEFT) {

            if (event.button.button == SDL_BUTTON_LEFT && SDL_PointInRectFloat(&this->mouse, &this->button) && !this->mouse_follow) {
                this->mouse_offset.y = this->mouse.y - this->button.y;
                this->mouse_follow = SDL_TRUE;
            }
        }
    }
    if(this->mouse_follow && event.type == SDL_EVENT_MOUSE_BUTTON_UP && event.button.button == SDL_BUTTON_LEFT){
        this->mouse_follow = SDL_FALSE;
    }
}

void slider::assets_in() {
    this->w = 407;
    this->h = 314;
    this->bg.x = 0;
    this->bg.y = 0;
    this->bg.h = this->h;
    this->bg.w = this->w;

    this->button.w = 32;
    this->button.h = 46;
    this->button.x = (this->w/2)-(this->button.w/2);
    this->button.y = (this->h/2)-(this->button.h/2);

    this->butt_bounds.w=button.w;
    this->butt_bounds.h=button.h*5;
    this->butt_bounds.x=button.x;
    this->butt_bounds.y=button.y-(this->button.h*2.5);

}

void slider::assets_out() {

//    SDL_DestroyTexture(this->bg);
//    SDL_DestroyTexture(this->button);
}
