
#ifndef COMMON_HPP
#define COMMON_HPP



#include "raylib.h"


struct Direction {      // direction relative to player's orientation
    float medial;
    float lateral;
};
struct Displacement {   // vectors calculated by applying the players rotation to the axis directions
    Vector2 medial;
    Vector2 lateral;
};



#endif
