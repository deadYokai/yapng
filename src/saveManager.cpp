//
// Created by kitsune on 12/29/23.
//

#include "saveManager.h"
#include <pwd.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fstream>
#include <slider.h>

int saveManager::load(){
    std::string p;
    if ((SAVE_PATH = getenv("HOME")) == nullptr) {
        SAVE_PATH = getpwuid(getuid())->pw_dir;
    }
    p = SAVE_PATH;
    p.append("/.config/yapng");
    if(!std::filesystem::exists(p))
        if (mkdir(SAVE_PATH, 0777) != 0)
            return CANNOT_ACCESS;
    p.append("/yapng.json");

    std::strcpy(SAVE_PATH, p.data());

    if(!std::filesystem::exists(SAVE_PATH)){
        std::ofstream savefile(SAVE_PATH);
        if(!savefile.is_open())
            return CANNOT_ACCESS;
        json j;

        // Default preferences
        j["loadedSave"] = "";
        j["micSensitivity"] = 50;
        j["animForce"] = 50;
        j["voiceDelayMs"] = 200;
        j["blinkDelayMs"] = 500;

        j["micSliderPos"] = {10, 16};
        j["micSliderSize"] = {700, 16};
        j["micSliderRotation"] = slider::horizontal;
        j["micSliderColorBar"] = {255, 0, 0, 255};
        j["micSliderSolidBackground"] = false;
        j["micSliderColorBackground"] = {255, 0, 0, 255};
        j["micSliderColorSlider"] = {0, 255, 255, 128};

        j["animSliderPos"] = {10, 704};
        j["animSliderSize"] = {700, 16};
        j["animSliderRotation"] = slider::horizontal;
        j["animSliderSolidBackground"] = true;
        j["animSliderColorBackground"] = {255, 0, 0, 255};
        j["animSliderColorSlider"] = {0, 255, 255, 128};
        std::string jsonstr = j.dump();
        savefile.write(jsonstr.data(),  static_cast<long>(jsonstr.size()));
    }

    std::ifstream savefile(SAVE_PATH);
    if(!savefile.is_open())
        return CANNOT_ACCESS;
    savefile >> this->prefs;
    return OK;
}

template<typename T>
T saveManager::getEntry(const char* name) {
    return this->prefs[name];
}

template<typename T>
int saveManager::setEntry(const char* name, T val) {
    this->prefs[name] = val;
    if(!std::filesystem::exists(this->SAVE_PATH))
        return CANNOT_ACCESS;
    std::ofstream savefile(this->SAVE_PATH);
    if(!savefile.is_open())
        return CANNOT_ACCESS;
    std::string jsonstr = this->prefs.dump();
    savefile.write(jsonstr.data(), (long)jsonstr.size());
    return OK;
}