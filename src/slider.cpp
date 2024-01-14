//
// Created by kitsune on 1/6/24.
//

#include "slider.h"

#include <cmath>

void slider::Render(SDL_Renderer *sdl_renderer, bool fillbg) {
    if(this->mouse_follow){
        if(this->rot == horizontal){
            this->button.x = this->mouse.x - this->mouse_offset.x;

            if (this->button.x <= this->butt_bounds.x) {
                this->button.x = this->butt_bounds.x;
            }

            if (this->button.x + this->button.w >= this->butt_bounds.x + this->butt_bounds.w) {
                this->button.x = (this->butt_bounds.x + this->butt_bounds.w) - this->button.w;
            }
        }else {
            this->button.y = this->mouse.y - this->mouse_offset.y;

            if (this->button.y <= this->butt_bounds.y) {
                this->button.y = this->butt_bounds.y;
            }

            if (this->button.y + this->button.h >= this->butt_bounds.y + this->butt_bounds.h) {
                this->button.y = (this->butt_bounds.y + this->butt_bounds.h) - this->button.h;
            }
        }

    }

    SDL_SetRenderDrawColor(sdl_renderer, 255, 0, 0, 255);
    if(fillbg)
        SDL_RenderFillRect(sdl_renderer, &this->bg);
    else
        SDL_RenderRect(sdl_renderer, &this->bg);
    SDL_SetRenderDrawColor(sdl_renderer, 0, 255, 255, 128);
    SDL_RenderFillRect(sdl_renderer, &this->button);


}

void slider::GetMouseState() {
    SDL_GetMouseState(&this->mouse.x, &this->mouse.y);
}

void slider::RegisterEvent(SDL_Event &event) {
    if(event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
        if (event.button.button == SDL_BUTTON_LEFT) {

            if (event.button.button == SDL_BUTTON_LEFT && SDL_PointInRectFloat(&this->mouse, &this->button) && !this->mouse_follow) {
                if(this->rot == horizontal)
                    this->mouse_offset.x = this->mouse.x - this->button.x;
                else
                    this->mouse_offset.y = this->mouse.y - this->button.y;
                this->mouse_follow = SDL_TRUE;
            }
        }
    }
    if(this->mouse_follow && event.type == SDL_EVENT_MOUSE_BUTTON_UP && event.button.button == SDL_BUTTON_LEFT){
        this->mouse_follow = SDL_FALSE;
    }
}

void slider::setValue(float val, float max) {
    if(val < 0)
        return;

    if(max <= 0)
        return;

    float vmin, vmax, bbx, bbw, bw;

    if(this->rot == horizontal) {
        bbx = this->butt_bounds.x;
        bbw = this->butt_bounds.w;
        bw = this->button.w;
    }else {
        bbx = this->butt_bounds.y;
        bbw = this->butt_bounds.h;
        bw = this->button.h;
    }

    vmin = bbx + bw/2;
    vmax = (bbx + bbw) - (bw/2);

    float vstep = (vmax - vmin)/max;
    float v = vstep * (max - val);

    if(this->rot == horizontal)
        this->button.x = vmax - v - this->button.w/2;
    else
        this->button.y = vmax - v - this->button.h/2;
}


float slider::getValue(float max) {
    if(max <= 0)
        return 0;

    float vmin, vmax, v;

    if(this->rot == horizontal) {
        vmin = this->butt_bounds.x + this->button.w/2;
        vmax = (this->butt_bounds.x + this->butt_bounds.w) - (this->button.w/2);
        v = vmax - (this->button.x + this->button.w/2);
    }else{
        vmin = this->butt_bounds.y + this->button.h/2;
        vmax = (this->butt_bounds.y + this->butt_bounds.h) - (this->button.h/2);
        v = vmax - (this->button.y + this->button.h/2);
    }

    float vstep = (vmax - vmin)/max;
    float value = max-(v/vstep);

    this->val = value;

    return this->val;
}


[[maybe_unused]] void slider::posChange(SDL_FRect opos) {
    this->pos = opos;
    this->w = this->pos.w;
    this->h = this->pos.h;
    this->bg.x = this->pos.x;
    this->bg.y = this->pos.y;
    this->button.x = (this->w/2)-(this->button.w/2)+this->pos.x;
    this->button.y = (this->h/2)-(this->button.h/2)+this->pos.y;
    this->butt_bounds.w=this->button.w;
    this->butt_bounds.h=this->h;
    this->butt_bounds.x=this->button.x;
    this->butt_bounds.y=this->pos.y;
    if(this->rot == Rot::horizontal){
        this->butt_bounds.w=this->w;
        this->butt_bounds.h=this->button.h;
        this->butt_bounds.y=this->button.y;
        this->butt_bounds.x=this->pos.x;
    }

}

void slider::assets_in(Rot p, SDL_FRect opos) {
    this->rot = p;
    this->pos = opos;
    this->w = this->pos.w;
    this->h = this->pos.h;
    this->bg.x = this->pos.x;
    this->bg.y = this->pos.y;
    this->bg.h = this->h;
    this->bg.w = this->w;

    this->button.w = 12;
    this->button.h = 16;
    this->button.x = (this->w/2)-(this->button.w/2)+this->pos.x;
    this->button.y = (this->h/2)-(this->button.h/2)+this->pos.y;

    if(this->rot == horizontal){
        this->butt_bounds.w=this->w;
        this->butt_bounds.h=this->button.h;
        this->butt_bounds.y=this->button.y;
        this->butt_bounds.x=this->pos.x;
    }else {
        this->butt_bounds.w=this->button.w;
        this->butt_bounds.h=this->h;
        this->butt_bounds.x=this->button.x;
        this->butt_bounds.y=this->pos.y;
    }

}

void slider::assets_out() {}
