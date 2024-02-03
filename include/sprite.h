#ifndef SPRITE_H
#define SPRITE_H

#include "SDL.h"
#include "SDL3_image/SDL_image.h"
#include <cstdlib>
#include <vector>

class Sprite{
public:
    Sprite(SDL_Renderer* renderer, const char* texturePath, int id, int parentId, std::vector<int> pos, std::vector<int> offset, int zIndex, const int image_state[2], std::vector<float> freqAmp);

    int getPos[2]{};
    int getOffset[2]{};
    int getSize[2]{};

    void setActiveBlinkState(int state);
    void setActiveTalkState(int state);
    void setPos(float x, float y);
    void setOffset(int x, int y);
    void setScale(float scale);
    void setRenderer(SDL_Renderer* renderer);

    SDL_FRect rect{};

    std::vector<float> fa;
    int zIndex;
    void render();
    float scale = 0.7f;

private:
    SDL_Texture* tex;
    int width{};
    int height{};
    int pos[2]{};
    int activeBlinkState{};
    int activeTalkState{};
    int blinkState;
    int talkState;
    int off[2]{};
    SDL_Renderer* renderer;
};

#endif
