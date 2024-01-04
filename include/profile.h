//
// Created by kitsune on 1/4/24.
//

#ifndef YAPNG_PROFILE_H
#define YAPNG_PROFILE_H

#include <fstream>
#include <filesystem>
#include "nlohmann/json.hpp"
#include "base64.hpp"
#include "sprite.h"

class Profile {
public:
    Profile(const char *SAVE_PATH, const char* path, SDL_Renderer* renderer);
    std::vector<Sprite> sprites;
};


#endif //YAPNG_PROFILE_H
