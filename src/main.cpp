#include <cstddef>
#include <iostream>
#include <SDL.h>
#include <SDL_main.h>
#include <SDL3_image/SDL_image.h>

void SDL_Fail(){
    SDL_LogError(SDL_LOG_CATEGORY_CUSTOM, "Error %s", SDL_GetError());
    exit(1);
}

static bool app_quit = false;
SDL_Renderer* renderer = nullptr;
SDL_Texture *img = NULL;
int w, h; // texture width & height

int main(int argc, char* argv[]){

    if (SDL_Init(SDL_INIT_EVERYTHING)){
        SDL_Fail();
    }


    SDL_Window* window = SDL_CreateWindow("Yet Another PNGTuber", 720, 720, SDL_WINDOW_OPENGL | SDL_WINDOW_TRANSPARENT);
    if (!window){
        SDL_Fail();
    }

    SDL_ShowWindow(window);
    {
        int width, height, bbwidth, bbheight;
        SDL_GetWindowSize(window, &width, &height);
        SDL_GetWindowSizeInPixels(window, &bbwidth, &bbheight);
        SDL_Log("Window size: %ix%i", width, height);
        SDL_Log("Backbuffer size: %ix%i", bbwidth, bbheight);
        if (width != bbwidth){
            SDL_Log("This is a highdpi environment.");
        }
    }


    renderer = SDL_CreateRenderer(window, NULL, 0);
    if (!renderer){
        SDL_Fail();
    }

    img = IMG_LoadTexture(renderer, "../assets/1.png");
    SDL_QueryTexture(img, NULL, NULL, &w, &h);

    SDL_Log("Application started successfully!");

    while (!app_quit) {

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT)
                app_quit = true;
            break;
        }


        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);
        SDL_RenderTexture(renderer, img, NULL, NULL);
        SDL_RenderPresent(renderer);

    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    SDL_Log("Application quit successfully!");

    return 0;
}


