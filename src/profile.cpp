//
// Created by kitsune on 1/4/24.
//

#include "profile.h"
#include "log.h"

using json = nlohmann::json;
using namespace Log;

std::vector<int> split(const std::string &s, char delimiter) {
    std::vector<int> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (getline(tokenStream, token, delimiter)) {
        tokens.push_back(stoi(token));
    }
    return tokens;
}

Profile::Profile(const char *SAVE_PATH, const char *savepath, SDL_Renderer* renderer) {

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
            if (std::filesystem::create_directory(outf.c_str()) != 0)
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

        std::vector<float> freqAmp = {dat["xAmp"], dat["xFrq"], dat["yAmp"], dat["yFrq"]};

        int pid = 0;
        if(!dat["parentId"].is_null()) {
            pid = dat["parentId"];
        }

        int state[2] = {dat["showTalk"],dat["showBlink"]};

        Sprite spr(renderer, outp.c_str(), 0, pid, ipos, ioff, dat["zindex"], state, freqAmp);
        img.push_back(spr);

    }

    std::sort(img.begin(), img.end(), [](Sprite &a, Sprite &b) -> bool {return a.zIndex < b.zIndex;});

    this->sprites = img;
}
