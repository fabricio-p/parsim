#ifndef PARSIM_BODY_CSV_READER_H
#define PARSIM_BODY_CSV_READER_H
#include <iostream>
#include <map>

#include "simulation.hpp"

class BodyCSVReader {
public:
    BodyCSVReader(std::istream& _in);
    void readInto(Simulation& simulation);
private:
    std::istream& in;
    std::map<std::string_view, Color> colorMap = {
        {"lightgray", LIGHTGRAY},
        {"gray", GRAY},
        {"darkgray", DARKGRAY},
        {"yellow", YELLOW},
        {"gold", GOLD},
        {"orange", ORANGE},
        {"pink", PINK},
        {"red", RED},
        {"maroon", MAROON},
        {"green", GREEN},
        {"lime", LIME},
        {"darkgreen", DARKGREEN},
        {"skyblue", SKYBLUE},
        {"blue", BLUE},
        {"darkblue", DARKBLUE},
        {"purple", PURPLE},
        {"violet", VIOLET},
        {"darkpurple", DARKPURPLE},
        {"beige", BEIGE},
        {"brown", BROWN},
        {"darkbrown", DARKBROWN},

        {"white", WHITE},
        {"black", BLACK},
        {"blank", BLANK},
        {"magenta", MAGENTA},
        {"raywhite", RAYWHITE},
    };
};

#endif /* PARSIM_BODY_CSV_READER_H */