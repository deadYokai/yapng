//
// Created by kitsune on 12/29/23.
//

#ifndef YAPNG_SAVEMANAGER_H
#define YAPNG_SAVEMANAGER_H

#include <slider.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class saveManager {
public:
    nlohmann::basic_json<>::value_type getEntry(const char* name);
    int setEntry(const char* name, nlohmann::basic_json<>::value_type val);
    int load();
    enum{
        CANNOT_ACCESS,
        OK = 100
    };
private:
    char* SAVE_PATH;
    json prefs;
    const std::map<std::string, nlohmann::basic_json<>::value_type> defVals{
        {"loadedSave", ""},
        {"micSensitivity", 50},
        {"animForce", 50},
        {"voiceDelayMs", 200},
        {"blinkDelayMs", 500},
        {"micSliderPos", {10, 16}},
        {"micSliderSize", {700, 16}},
        {"micSliderRotation", slider::horizontal},
        {"micSliderColorBar", {255, 0, 0, 255}},
        {"micSliderSolidBackground", false},
        {"micSliderColorBackground", {255, 0, 0, 255}},
        {"micSliderColorSlider", {0, 255, 255, 128}},
        {"animSliderPos", {10, 704}},
        {"animSliderSize", {700, 16}},
        {"animSliderRotation", slider::horizontal},
        {"animSliderSolidBackground", true},
        {"animSliderColorBackground", {255, 0, 0, 255}},
        {"animSliderColorSlider", {0, 255, 255, 128}}
    };
};

#endif //YAPNG_SAVEMANAGER_H
