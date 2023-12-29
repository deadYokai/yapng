//
// Created by kitsune on 12/29/23.
//

#ifndef YAPNG_SAVEMANAGER_H
#define YAPNG_SAVEMANAGER_H

#include <nlohmann/json.hpp>

using json = nlohmann::json;

class saveManager {
public:
    template<class T>
    T getEntry(const char* name);
    template<class T>
    int setEntry(const char* name, T val);
    int load(const char* filepath);
private:
    char* SAVE_PATH;
    json prefs;
    enum{
        CANNOT_ACCESS,
        OK = 100
    };
};

#endif //YAPNG_SAVEMANAGER_H
