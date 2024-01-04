//
// Created by kitsune on 1/4/24.
//

#ifndef YAPNG_LOG_H
#define YAPNG_LOG_H

#include "SDL.h"
#include <cstdlib>

namespace Log{
    // Function for logging errors
    static void SDL_Fail(const char* errText){
        SDL_LogError(SDL_LOG_CATEGORY_CUSTOM, "%s", errText);
        exit(1);
    }
};

#endif //YAPNG_LOG_H
