
#ifndef PLAYER_HPP
#define PLAYER_HPP



#include "raylib.h"
#include "common.hpp"
#include <cmath>


class Player {
public:
    Vector2 pos;
    Displacement displacement;
    float angle;
    const float fov = 60.0f * PI / 180.0f;

private:
    const float speed = 5.f;
    const float sens = 0.4f * PI / 180.0f;

public:
    void spawn(Vector2 point) {
        pos = point;
        angle = 0.f;
    }

    void turn(float delta) {
        angle += delta * sens;
        angle = fmodf(angle, 2 * PI);
        if (angle < 0) angle += 2 * PI;        
    }

    void virtualMove(Direction direction) {
        Vector2 forwardDir = { (float)cos(angle), (float)sin(angle) };
        Vector2 rightDir = { -forwardDir.y, forwardDir.x }; 

        displacement.medial.x = forwardDir.x * direction.medial * speed;
        displacement.medial.y = forwardDir.y * direction.medial * speed;

        displacement.lateral.x = rightDir.x * direction.lateral * speed;
        displacement.lateral.y = rightDir.y * direction.lateral * speed;
    }

    void realMove(Direction collision) {
        if (!collision.medial) {
            pos.x += displacement.medial.x;
            pos.y += displacement.medial.y;
        }
        if (!collision.lateral) {
            pos.x += displacement.lateral.x;
            pos.y += displacement.lateral.y;
        }
    }

    void draw() {
        DrawCircleV(pos, 10.f, BLUE);
    }
};



#endif