//
// Created by kitsune on 12/29/23.
//

#ifndef YAPNG_SAVEMANAGER_H
#define YAPNG_SAVEMANAGER_H

#include <nlohmann/json.hpp>

using json = nlohmann::json;

class saveManager {
public:
    template<typename T>
    T getEntry(const char* name);
    template<typename T>
    int setEntry(const char* name, T val);
    int load();
    enum{
        CANNOT_ACCESS,
        OK = 100
    };
private:
    char* SAVE_PATH;
    json prefs;
};

#endif //YAPNG_SAVEMANAGER_H
