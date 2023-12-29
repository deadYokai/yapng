#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <fstream>
#include <filesystem>
#include <sys/stat.h>
#include <pwd.h>

#include <SDL.h>
#include <SDL_main.h>


#include <nlohmann/json.hpp>
#include <future>

#include "base64.hpp"
#include "sprite.h"

using json = nlohmann::json;

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

std::vector<int> split(const std::string &s, char delimiter) {
    std::vector<int> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (getline(tokenStream, token, delimiter)) {
        tokens.push_back(stoi(token));
    }
    return tokens;
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
    std::string sp = SAVE_PATH;
    sp.append(savepath);
    savepath = sp.c_str();

    if(!std::filesystem::exists(savepath))
        SDL_Fail("save file not found");

    std::ifstream savefile(savepath);
    if(!savefile.is_open())
        SDL_Fail("cannot access to save file");

    // Getting info from json (save file)
    json savedata;
    savefile >> savedata;

    std::vector<Sprite> img;
    std::vector<Sprite>::iterator img_iter;

    for (auto dat : savedata) {
        if(dat["path"] == NULL)
            SDL_Fail("unsupported or broken save file");

        auto spath = dat["path"].get_ref<const std::string &>();
        auto sspath = spath.substr(7, spath.length()-7);
        auto path = sspath.c_str();
        std::ofstream png;
        std::string outp = SAVE_PATH;
        outp.append(path);

        auto outf = outp.substr(0, outp.find_last_of('/'));

        // Check if profile folder exists or create folder
        if(!std::filesystem::exists(outf.c_str()))
            if (mkdir(outf.c_str(), 0777) != 0)
                SDL_Fail("unable to create profile folder");

        // Copy save file to profile dir if not exists
        if(!std::filesystem::exists((outf+"/save.json").c_str()))
            std::filesystem::copy_file(savepath, (outf+"/save.json").c_str());

        // Decode file to profile dir if not exists
        if(!std::filesystem::exists(outp.c_str())){

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


        auto ps = dat["pos"].get_ref<const std::string &>();
        auto p = ps.substr(8, ps.length()-9);
        auto of = dat["offset"].get_ref<const std::string &>();
        auto off = of.substr(8, ps.length()-9);

        std::vector<int> ipos = split(p, ',');
        std::vector<int> ioff = split(off, ',');

        int pid = 0;
        if(!dat["parentId"].is_null()) {
            pid = dat["parentId"];
        }

        int state[2] = {dat["showTalk"],dat["showBlink"]};

        Sprite spr(renderer, outp.c_str(), 0, pid, ipos, ioff, dat["zindex"], state);
        img.push_back(spr);

    }

    std::sort(img.begin(), img.end(), [](Sprite &a, Sprite &b) -> bool {return a.zIndex < b.zIndex;});

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

    const SDL_AudioSpec recSpec = {SDL_AUDIO_S16LE, 2, 44100};
    auto stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_CAPTURE, &recSpec, arc, nullptr);
    SDL_ResumeAudioDevice(SDL_GetAudioStreamDevice(stream));

    // mic level rect
    SDL_FRect fq;
    fq.x = 0;
    fq.y = 0;
    fq.h = 16;
    fq.w = 16;

    while (!app_quit) {

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                app_quit = true;
            }
            break;
        }

        fq.w = (volume*100/16000)*720/100;
        SDL_Log("Mic level: %f", volume);

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

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    SDL_Log("Application quit successfully!");

    return 0;
}


