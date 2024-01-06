#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <filesystem>
#include <sys/stat.h>
#include <pwd.h>

#include <SDL.h>
#include <SDL_main.h>

#include <nlohmann/json.hpp>
#include "base64.hpp"
#include "log.h"
#include "profile.h"
#include "sprite.h"
#include "slider.h"

using json = nlohmann::json;
using namespace Log;

#ifdef WINDOW_FLAG_OPENGL
#define RENDERER_FLAG SDL_WINDOW_OPENGL
#else
#define RENDERER_FLAG SDL_WINDOW_VULKAN
#endif

float volume = 0;
void arc(void *userdata, SDL_AudioStream *stream, int additional_amount, int total_amount){
    if (additional_amount > 0) {
        auto *data = SDL_stack_alloc(Uint8, additional_amount);
        if (data) {
            SDL_GetAudioStreamData(stream, data, additional_amount);

            short sample;
            auto scale = (float)(1.0 / (double)(1 << (sizeof(sample) - 1)));
            std::vector<float> samples;
            for(int i = 0; i < total_amount; i += sizeof(data)){
                memcpy(&sample, &data[i], sizeof(sample));
                short s = sample * scale;
                samples.push_back(s);
            }
            volume = std::max_element(samples.begin(), samples.end())[0];
        }
    }
}


static bool app_quit = false;
SDL_Renderer* renderer = nullptr;
SDL_Window* initRender(){
    if (SDL_Init(SDL_INIT_EVERYTHING)){
        SDL_Fail(SDL_GetError());
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

    SDL_Window* window = SDL_CreateWindow("Yet Another PNGTuber", 720, 720, RENDERER_FLAG | SDL_WINDOW_TRANSPARENT);
    if (!window){
        SDL_Fail(SDL_GetError());
    }

    renderer = SDL_CreateRenderer(window, nullptr, SDL_RENDERER_ACCELERATED);
    if (!renderer){
        SDL_Fail(SDL_GetError());
    }

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    return window;
}

int main(){
    SDL_Window* window = initRender();
    const char* SAVE_PATH;
    std::string cpath;
    if ((SAVE_PATH = getenv("HOME")) == nullptr) {
        SAVE_PATH = getpwuid(getuid())->pw_dir;
    }
    cpath = SAVE_PATH;
    cpath.append("/.config/yapng/");
    SAVE_PATH = cpath.c_str();

    // Check if SAVE_PATH exists or create
    if(!std::filesystem::exists(SAVE_PATH))
        if (mkdir(SAVE_PATH, 0777) != 0)
            SDL_Fail("unable to create user data folder");

    // Open save file
    // TODO: create entry in settings
    const char* savepath = "assets/save.json";
    Profile p = Profile(SAVE_PATH, savepath, renderer);
    std::vector<Sprite> img = p.sprites;
    std::vector<Sprite>::iterator img_iter;

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

    //TODO: transform to other class
    const SDL_AudioSpec recSpec = {SDL_AUDIO_S16LE, 2, 44100};
    auto stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_CAPTURE, &recSpec, arc, nullptr);
    SDL_ResumeAudioDevice(SDL_GetAudioStreamDevice(stream));

    // mic level rect
    SDL_FRect fq;
    fq.x = 0;
    fq.y = 0;
    fq.h = 16;
    fq.w = 16;

    slider s;
    s.assets_in();

    while (!app_quit) {

        SDL_Event event;
        s.GetMouseState();
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                app_quit = true;
            }
            s.RegisterEvent(event);
            break;
        }

        fq.w = (volume*100/16000)*720/100;
//        SDL_Log("Mic level: %f", volume);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);

        for (img_iter = img.begin(); img_iter != img.end(); ++img_iter) {
            Sprite a = *img_iter;
            int ts = 1;
            if(volume > 50) // configure mic sensitivity
                ts = 2;
            a.setActiveTalkState(ts);
            a.setActiveBlinkState(1); // TODO: create random time change
            a.render();
        }

        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderRect(renderer, &fq);

        s.Render(renderer);

        SDL_RenderPresent(renderer);
    }

    s.assets_out();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    SDL_Log("Application quit successfully!");

    return 0;
}


