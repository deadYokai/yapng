#include "sprite.h"

#include <algorithm>
#include <cmath>
#include <utility>

Sprite::Sprite(SDL_Renderer* renderer, const char *texturePath, int id, int parentId, std::vector<int> pos, std::vector<int> offset, int zIndex, const int image_state[2], std::vector<float> freqAmp) {
    this->renderer = renderer;
    this->tex = IMG_LoadTexture(this->renderer, texturePath);
    SDL_QueryTexture(this->tex, nullptr, nullptr, &this->width, &this->height);

    //
    // ** idk why this needed **
    //
    this->pos[0] = pos[0];
    this->pos[1] = pos[1];
    this->off[0] = offset[0];
    this->off[1] = offset[1];
    //
    // *******
    //

    const float sizes[2] = {720/static_cast<float>(this->width), 720/static_cast<float>(this->height)};
    this->scale = std::round((std::min(sizes[0], sizes[1]) - 0.02f) * 100) / 100; // cut to 2 numbers after dot

    const float new_x = std::abs(static_cast<float>(this->width))/2 - std::abs(static_cast<float>(this->width))/2 * this->scale;
    const float new_y = std::abs(720-static_cast<float>(this->height))/2 - std::abs(720-static_cast<float>(this->height))/2 * this->scale;

    this->rect.x = new_x;
    this->rect.y = new_y;
    this->rect.w = static_cast<float>(this->width) * this->scale;
    this->rect.h = static_cast<float>(this->height) * this->scale;
    this->zIndex = zIndex;
    this->talkState = image_state[0];
    this->blinkState = image_state[1];
    this->fa = std::move(freqAmp);

}

void Sprite::setActiveBlinkState(int state) {
    this->activeBlinkState = state;
}

void Sprite::setActiveTalkState(int state) {
    this->activeTalkState = state;
}

void Sprite::setPos(float x, float y) {
    this->rect.x = x;
    this->rect.y = y;
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
