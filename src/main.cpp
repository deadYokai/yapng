#include <cstddef>
#include <cstdio>
#include <iostream>
#include <SDL.h>
#include <SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <fstream>
#include <nlohmann/json.hpp>
#include <sys/types.h>
#include <sys/stat.h>
#include <filesystem>
#include "base64.hpp"

using json = nlohmann::json;

#ifndef SAVE_PATH
// TODO: change to user config folder
#define SAVE_PATH "../savedata/"
#endif

// Function for logging errors
void SDL_Fail(const char* errText){
    char* err = (char*)SDL_GetError();
    if(err != NULL)
        err = (char*)errText;
    SDL_LogError(SDL_LOG_CATEGORY_CUSTOM, "%s", err);
    exit(1);
}

void copyFile(const char* from, const char* to){

    SDL_Log("Copying %s to %s", from, to);

    char buf[BUFSIZ];
    size_t size;

    FILE* s = fopen(from, "r");
    FILE* d = fopen(to, "w");

    while(size = fread(buf, 1, BUFSIZ, s))
        fwrite(buf, 1, size, d);

    fclose(s);
    fclose(d);
}

static bool app_quit = false;
SDL_Renderer* renderer = nullptr;


int main(int argc, char* argv[]){

    // Check if SAVE_PATH exists or create
    struct stat info;
    if(stat(SAVE_PATH, &info) != 0)
        if (mkdir(SAVE_PATH, 0777) != 0)
            SDL_Fail("unable to create user data folder");

    // Open save file
    // TODO: create entry in settings
    const char* savepath = "../assets/save.json";
    std::ifstream savefile(savepath);

    if(!savefile.is_open())
        SDL_Fail("cannot access to save file");

    // Getting info from json (save file)
    json savedata;
    savefile >> savedata;

    for (json::iterator it = savedata.begin(); it != savedata.end(); ++it) {
        auto dat = *it;

        // Check if encoded image exists in a save file
        if(dat["imageData"] == NULL)
            SDL_Fail("Unsopported save file");

        auto spath = dat["path"].get_ref<const std::string &>();
        auto sspath = spath.substr(7, spath.length()-7);
        auto path = sspath.c_str();
        std::ofstream png;
        std::string outp = SAVE_PATH;
        outp.append(path);

        auto outf = outp.substr(0, outp.find_last_of("/"));


        // Check if profile folder exists or create folder
        if(stat(outf.c_str(), &info) != 0)
            if (mkdir(outf.c_str(), 0777) != 0)
                SDL_Fail("unable to create profile folder");

        // Copy save file to profile dir if not exists
        if(stat((outf+"/save.json").c_str(), &info) != 0)
            copyFile(savepath, (outf+"/save.json").c_str());

        // Decode file to profile dir if not exists
        if(stat(outp.c_str(), &info) != 0){

            // Write files to profile folder
            SDL_Log("Decoding %s to %s", path, outp.c_str());
            png.open(outp.c_str());

            if(!png.is_open())
                SDL_Fail("Fail to write files");

            std::string imgdat = dat["imageData"];
            png << base64::from_base64(imgdat);
            png.close();
        }


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


