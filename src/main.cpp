#include <cstddef>
#include <iostream>
#include <SDL.h>
#include <SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <fstream>
#include <nlohmann/json.hpp>
#include <sys/types.h>
#include <sys/stat.h>
#include "base64.hpp"

using json = nlohmann::json;

#ifndef SAVE_PATH
#define SAVE_PATH "../savedata/"
#endif

void SDL_Fail(const char* errText){
    char* err = (char*)SDL_GetError();
    if(err != NULL)
        err = (char*)errText;
    SDL_LogError(SDL_LOG_CATEGORY_CUSTOM, "%s", err);
    exit(1);
}

static bool app_quit = false;
SDL_Renderer* renderer = nullptr;


int main(int argc, char* argv[]){

    struct stat info;

    if(stat(SAVE_PATH, &info) != 0)
        if (mkdir(SAVE_PATH, 0777) != 0)
            SDL_Fail("unable to create user data folder");

    std::ifstream savefile("../assets/save.json");
    json savedata;
    savefile >> savedata;

    for (json::iterator it = savedata.begin(); it != savedata.end(); ++it) {
        auto dat = *it;

        if(dat["imageData"] == NULL)
            SDL_Fail("Unsopported save file");

        auto spath = dat["path"].get_ref<const std::string &>();
        auto sspath = spath.substr(7, spath.length()-7);
        auto path = sspath.c_str();
        std::ofstream png;
        std::string outp = SAVE_PATH;
        outp.append(path);

        auto outf = outp.substr(0, outp.find_last_of("/"));

        SDL_Log("Decoding %s to %s", path, outp.c_str());
        if(stat(outf.c_str(), &info) != 0)
            if (mkdir(outf.c_str(), 0777) != 0)
                SDL_Fail("unable to create profile folder");

        png.open(outp.c_str());

        if(!png.is_open())
            SDL_Fail("Fail to write files");

        std::string imgdat = dat["imageData"];
        png << base64::from_base64(imgdat);
        png.close();

    }

    if (SDL_Init(SDL_INIT_EVERYTHING)){
        SDL_Fail("");
    }


    SDL_Window* window = SDL_CreateWindow("Yet Another PNGTuber", 720, 720, SDL_WINDOW_VULKAN | SDL_WINDOW_TRANSPARENT);
    if (!window){
        SDL_Fail("");
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
        SDL_Fail("");
    }

    // SDL_Texture* img = IMG_LoadTexture(renderer, "../assets/1.png");
    // int w, h;
    // SDL_QueryTexture(img, NULL, NULL, &w, &h);

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
        // SDL_RenderTexture(renderer, img, NULL, NULL);
        SDL_RenderPresent(renderer);

    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    SDL_Log("Application quit successfully!");

    return 0;
}


