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
#include <vector>
#include <algorithm>
#include <stdlib.h>

using json = nlohmann::json;

#ifndef SAVE_PATH
// TODO: change to user config folder
#define SAVE_PATH "../savedata/"
#endif

#ifdef WINDOW_FLAG_OPENGL
#define RENDERER_FLAG SDL_WINDOW_OPENGL
#else
#define RENDERER_FLAG SDL_WINDOW_VULKAN
#endif

// Function for logging errors
void SDL_Fail(const char* errText){
    SDL_LogError(SDL_LOG_CATEGORY_CUSTOM, "%s", errText);
    exit(1);
}

std::vector<int> split(const std::string &s, char delimiter) {
    std::vector<int> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (getline(tokenStream, token, delimiter)) {
        tokens.push_back(stoi(token));
    }
    return tokens;
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

typedef struct Image{
    int id;
    SDL_Texture* img;
    int zindex;
    bool blink;
    int talk;    // 0, 1, 2
    int pid;     // parent id
    SDL_FRect rect;
} Image;

bool Image_zindex_cmp(const Image &a, const Image &b){
    return a.zindex < b.zindex;
}

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

    std::vector<Image> img;
    std::vector<Image>::iterator img_iter;


    if (SDL_Init(SDL_INIT_EVERYTHING)){
        SDL_Fail(SDL_GetError());
    }


    SDL_Window* window = SDL_CreateWindow("Yet Another PNGTuber", 720, 720, RENDERER_FLAG | SDL_WINDOW_TRANSPARENT);
    if (!window){
        SDL_Fail(SDL_GetError());
    }

    renderer = SDL_CreateRenderer(window, NULL, SDL_RENDERER_ACCELERATED);
    if (!renderer){
        SDL_Fail(SDL_GetError());
    }

    float scale = 0.7;
    for (json::iterator it = savedata.begin(); it != savedata.end(); ++it) {
        auto dat = *it;

        if(dat["path"] == NULL)
            SDL_Fail("unsupported or broken save file");

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

            // Check if encoded image exists in a save file
            if(dat["imageData"] == NULL)
                SDL_Fail("image data not found");

            // Write files to profile folder
            SDL_Log("Decoding %s to %s", path, outp.c_str());
            png.open(outp.c_str());

            if(!png.is_open())
                SDL_Fail("Fail to write files");

            std::string imgdat = dat["imageData"];
            png << base64::from_base64(imgdat);
            png.close();
        }



        SDL_FRect r;
        SDL_Texture* a = IMG_LoadTexture(renderer, outp.c_str());

        int w, h;
        SDL_QueryTexture(a, NULL, NULL, &w, &h);

        auto ps = dat["pos"].get_ref<const std::string &>();
        auto p = ps.substr(8, ps.length()-9);

        std::vector<int> ipos = split(p, ',');

        int new_x = w/2 - (w/2 * scale);
        int new_y = abs(720-h)/2 - (abs(720-h)/2 * scale);

        r.x = new_x;
        r.y = new_y;
        r.w = w * scale;
        r.h = h * scale;

        int pid = 0;
        if(!dat["parentId"].is_null())
            pid = dat["parentId"];

        img.push_back({dat["identification"], a, dat["zindex"], false, 0, pid, r});

    }

    std::sort(img.begin(), img.end(), Image_zindex_cmp);

    SDL_AudioDeviceID recordingDeviceId = 0; // use default
    SDL_AudioSpec desiredRecordingSpec;

    SDL_zero(desiredRecordingSpec);
    desiredRecordingSpec.freq = 44100;
    desiredRecordingSpec.format = AUDIO_S16;
    desiredRecordingSpec.channels = 2;
    desiredRecordingSpec.samples = 4096;
    desiredRecordingSpec.callback = audioRecordingCallback;

    if(SDL_GetNumAudioDevices(SDL_TRUE) < 1)
        SDL_Fail(SDL_GetError());


    SDL_ShowWindow(window);
    {
        int width, height, bbwidth, bbheight;
        SDL_GetWindowSize(window, &width, &height);
        SDL_GetWindowSizeInPixels(window, &bbwidth, &bbheight);
#ifdef WINDOW_FLAG_OPENGL
        SDL_Log("Using OpenGL window flag");
#endif
        SDL_Log("Window size: %ix%i", width, height);
        SDL_Log("Backbuffer size: %ix%i", bbwidth, bbheight);
        if (width != bbwidth){
            SDL_Log("This is a highdpi environment.");
        }
    }

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
        for (img_iter = img.begin(); img_iter != img.end(); ++img_iter) {
            Image a = *img_iter;
            SDL_RenderTexture(renderer, a.img, NULL, &a.rect);
        }
        SDL_RenderPresent(renderer);

        SDL_Delay(15);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    SDL_Log("Application quit successfully!");

    return 0;
}


