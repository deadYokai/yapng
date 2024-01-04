#include "sprite.h"

Sprite::Sprite(SDL_Renderer* renderer, const char *texturePath, int id, int parentId, std::vector<int> pos, std::vector<int> offset, int zIndex, const int image_state[2]) {

    this->renderer = renderer;
    this->tex = IMG_LoadTexture(this->renderer, texturePath);
    SDL_QueryTexture(this->tex, nullptr, nullptr, &this->width, &this->height);
    int new_x = this->width/2 - (this->width/2 * this->scale);
    int new_y = abs(720-this->height)/2 - (abs(720-this->height)/2 * this->scale);

    this->rect.x = new_x;
    this->rect.y = new_y;
    this->rect.w = this->width * this->scale;
    this->rect.h = this->height * this->scale;
    this->zIndex = zIndex;
    this->pos[0] = pos[0];
    this->pos[1] = pos[1];
    this->off[0] = offset[0];
    this->off[1] = offset[1];
    this->talkState = image_state[0];
    this->blinkState = image_state[1];

}

void Sprite::setActiveBlinkState(int state) {
    this->activeBlinkState = state;
}

void Sprite::setActiveTalkState(int state) {
    this->activeTalkState = state;
}

void Sprite::setPos(int x, int y) {
    this->pos[0] = x;
    this->pos[1] = y;
}

void Sprite::setOffset(int x, int y) {
    this->off[0] = x;
    this->off[1] = y;
}

void Sprite::render() {
    if((this->blinkState == 0 || this->blinkState == this->activeBlinkState) && (this->talkState == 0 || this->talkState == this->activeTalkState))
        SDL_RenderTexture(renderer, this->tex, nullptr, &this->rect);
}

void Sprite::setRenderer(SDL_Renderer *sdl_renderer) {
    this->renderer = sdl_renderer;
}

void Sprite::setScale(float img_scale) {
    this->scale = img_scale;
}
