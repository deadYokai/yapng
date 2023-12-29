//
// Created by kitsune on 12/29/23.
//

#include "saveManager.h"
#include <pwd.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fstream>

int saveManager::load(const char* filepath){
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
        j["loaded_save"] = "";
        j["mic_sensitivity"] = 50;
        std::string jsonstr = j.dump();
        savefile.write(jsonstr.data(), (long)jsonstr.size());
    }

    std::ifstream savefile(SAVE_PATH);
    if(!savefile.is_open())
        return CANNOT_ACCESS;
    savefile >> this->prefs;
    return OK;
}

template<class T>
T saveManager::getEntry(const char* name) {
    return prefs[name];
}

template<class T>
int saveManager::setEntry(const char* name, T val) {
    prefs[name] = val;
    if(!std::filesystem::exists(SAVE_PATH))
        return CANNOT_ACCESS;
    std::ofstream savefile(SAVE_PATH);
    if(!savefile.is_open())
        return CANNOT_ACCESS;
    std::string jsonstr = prefs.dump();
    savefile.write(jsonstr.data(), (long)jsonstr.size());
    return OK;
}