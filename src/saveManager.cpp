//
// Created by kitsune on 12/29/23.
//

#include "saveManager.h"
#include <pwd.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fstream>

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

    json j;
    if(std::filesystem::exists(SAVE_PATH)){
        std::ifstream readSaveFile(SAVE_PATH);
        if(!readSaveFile.is_open())
            return CANNOT_ACCESS;
        j = json::parse(readSaveFile);
    }
    std::ofstream writeSaveFile(SAVE_PATH);
    if(!writeSaveFile.is_open())
        return CANNOT_ACCESS;
    for (const auto& [key, value] : this->defVals) {
        if(j[key] == nullptr)
            j[key] = value;
    }
    std::string jsonstr = j.dump(4);
    writeSaveFile.write(jsonstr.data(),  static_cast<long>(jsonstr.size()));
    this->prefs = j;
    return OK;
}

nlohmann::basic_json<>::value_type saveManager::getEntry(const char* name) {
    return this->prefs[name];
}

int saveManager::setEntry(const char* name, nlohmann::basic_json<>::value_type val) {
    this->prefs[name] = val;
    if(!std::filesystem::exists(this->SAVE_PATH))
        return CANNOT_ACCESS;
    std::ofstream savefile(this->SAVE_PATH);
    if(!savefile.is_open())
        return CANNOT_ACCESS;
    std::string jsonstr = this->prefs.dump(4);
    savefile.write(jsonstr.data(), (long)jsonstr.size());
    return OK;
}