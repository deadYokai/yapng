#include <cmath>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <filesystem>
#include <iostream>
#include <sys/stat.h>
#include <pwd.h>
#include <saveManager.h>
#include <random>
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
SDL_Window* initRender(bool opengl){
    if (SDL_Init(SDL_INIT_EVERYTHING)){
        SDL_Fail(SDL_GetError());
    }

    auto RENDERER_FLAG = SDL_WINDOW_VULKAN;
    if(opengl)
        RENDERER_FLAG = SDL_WINDOW_OPENGL;

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

int main(int argc, char* argv[]){
    bool opengl = false;
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);

        if (arg == "-h" || arg == "--help") {
            std::cout << "Usage: " << argv[0] << " [options]\n";
            std::cout << "Options:\n";
            std::cout << "  -h, --help    Show this help message\n";
            std::cout << "  -o, --opengl  Use OpenGL instead Vulkan\n";
            exit(0);
        }else if (arg == "-o" || arg == "--opengl") {
            opengl = true;
        }
    }

    saveManager sett;
    SDL_Window* window = initRender(opengl);
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

    if(sett.load() == saveManager::CANNOT_ACCESS)
        SDL_Fail("Cannot access to settings file");

    // load user avatar file
    // TODO: create entry in settings
    const char* savepath = "assets/save.json";
    Profile p = Profile(SAVE_PATH, savepath, renderer);
    std::vector<Sprite> img = p.sprites;
    std::vector<Sprite>::iterator img_iter;

    // initialize window
    SDL_ShowWindow(window);
    {
        int width, height, bbwidth, bbheight;
        SDL_GetWindowSize(window, &width, &height);
        SDL_GetWindowSizeInPixels(window, &bbwidth, &bbheight);
        if(opengl)
            SDL_Log("Using OpenGL window flag");
        SDL_Log("Window size: %ix%i", width, height);
        SDL_Log("Backbuffer size: %ix%i", bbwidth, bbheight);
        if (width != bbwidth){
            SDL_Log("This is a highdpi environment.");
        }
    }

    SDL_Log("Application started successfully!");

    // audio initialization
    // TODO: transform to other class
    const SDL_AudioSpec recSpec = {SDL_AUDIO_S16LE, 2, 44100};
    auto stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_CAPTURE, &recSpec, arc, nullptr);
    SDL_ResumeAudioDevice(SDL_GetAudioStreamDevice(stream));

    // initialize some timers
    // rewrite in future
    Uint64 vtimer = SDL_GetTicks();
    Uint64 btimer = SDL_GetTicks();
    Uint64 vDelay = sett.getEntry("voiceDelayMs");; // voice anim delay
    Uint64 bDelay = sett.getEntry("blinkDelayMs"); // blink anim delay

    // rect for mic slider
    float barW = 700; // width for active volume and width for rect
    SDL_FRect fq;
    fq.x = 10;
    fq.y = 16;
    fq.h = 16;
    fq.w = barW;

    // initialize volume slider
    slider volMeter;
    volMeter.assets_in(slider::Rot::horizontal, fq);

    // random for blinking
    std::random_device rd;  // a seed source for the random number engine
    std::mt19937 gen(rd()); // mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<> distrib(0, 10);

    // TODO: create frames changing
    // bool talking = false;
    // bool blinking = false;

    int ts = 1;
    int bs = 1;

    // set value for mic slider from save file
    volMeter.setValue(sett.getEntry("micSensitivity"), 8000);

    bool focusedWindow = true;
    while (!app_quit) {

        SDL_Event event;
        volMeter.GetMouseState();
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_EVENT_WINDOW_MOUSE_LEAVE:
                    focusedWindow = false;
                    break;
                case SDL_EVENT_WINDOW_MOUSE_ENTER:
                    focusedWindow = true;
                    break;
                case SDL_EVENT_QUIT:
                    app_quit = true;
                break;
                default:
                    break;
            }
            volMeter.RegisterEvent(event);
            break;
        }

        fq.w = (volume*100/8000)*barW/100;
        if(fq.w >= barW)
            fq.w = barW;

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);

        float micVal = volMeter.getValue(8000);
        sett.setEntry("micSensitivity", micVal);



        for (img_iter = img.begin(); img_iter != img.end(); ++img_iter) {
            Sprite a = *img_iter;
            Uint64 dTime = SDL_GetTicks() / 20;
            Uint64 animationStep = 1000;
            auto dt = static_cast<float>(dTime % animationStep);
            // talk
            if(volume > micVal) // configure mic sensitivity
                ts = 2;
            if((vtimer + vDelay) <= SDL_GetTicks()) {
                if(volume <= micVal)
                    ts = 1;
                vtimer = SDL_GetTicks();
            }
            a.setActiveTalkState(ts);

            // blinks
            int random = distrib(gen);
            if((btimer + bDelay) <= SDL_GetTicks()) {
                if(random == 7) // just a random number
                    bs = 2;
                else
                    bs = 1;
                btimer = SDL_GetTicks();
            }
            a.setActiveBlinkState(bs);

            // *** here i'm trying to animate sprites
            float x = a.rect.x;
            float y = a.rect.y;

            float xfreq = a.fa[0];
            float xamp = a.fa[2] / a.scale;
            float xvel =  dt * xfreq;
            float xsinVel = std::sin(xvel * M_PIf * 2);

            float yfreq = a.fa[1];
            float yamp = -a.fa[3] / a.scale;
            float yvel =  dt * yfreq;
            float ysinVel = std::sin(yvel * M_PIf * 2);

            x += xsinVel / a.scale;
            y += ysinVel / a.scale;
            a.setPos(x, y);
            // here i'm trying to animate sprites ***

            // render sprite
            a.render();
        }

        // render some objects if window is focused
        if(focusedWindow) {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            SDL_RenderFillRect(renderer, &fq);
            volMeter.Render(renderer, false);
        }

        SDL_RenderPresent(renderer);
    }

    // unload all
    volMeter.assets_out();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    // quit from app
    SDL_Quit();
    SDL_Log("Application quit successfully!");

    return 0;
}


