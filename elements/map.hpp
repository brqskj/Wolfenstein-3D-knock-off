
#ifndef MAP_HPP
#define MAP_HPP



#include "raylib.h"
#include "common.hpp"
#include <vector>
#include <string>
#include <fstream>


class Map {
public:    
    Vector2 spawn;

private:
    std::vector <std::vector <bool>> field;
    unsigned int columns;
    unsigned int rows;
    std::vector <float> distances;

    const float cellSize = 50.0f;
    const float cellHeight = 30.f;
    const unsigned int rayCount = 120;

public:
    void loadFile(std::string name) {
        // open file
        name = "levels/" + name + ".bin";
        std::ifstream file((name), std::ios::binary);
        if (!file.is_open()) return; 

        // read dimensions
        unsigned char one, two;
        file.read(reinterpret_cast<char*>(&one), 1);
        file.read(reinterpret_cast<char*>(&two), 1);
        rows = static_cast<unsigned int>(one);
        columns = static_cast<unsigned int>(two);

        // read spawn pos
        file.read(reinterpret_cast<char*>(&one), 1);
        file.read(reinterpret_cast<char*>(&two), 1);
        spawn = {
            (static_cast<float>(one) * cellSize) + (cellSize * 0.5f),
            (static_cast<float>(two) * cellSize) + (cellSize * 0.5f)
        };

        // populate field
        field.clear();
        field.resize(rows);
        for (int i = 0; i < field.size(); i++) field[i].resize(columns);

        unsigned char byte;
        int bitCounter = 8;

        for (int r = 0; r < rows; r++) {
            for (int c = 0; c < columns; c++) {

                if (bitCounter == 8) {
                    if (!file.read(reinterpret_cast<char*>(&byte), 1)) return;
                    bitCounter = 0;
                }

                field[r][c] = (byte >> (7 - bitCounter)) & 1;
                bitCounter++;
            }
        }
    }

    Direction checkCollision(Displacement displacement, Vector2 pos) {
        Direction verdict = { false, false };

        // Controllo Medial
        float testMedialX = pos.x + displacement.medial.x;
        float testMedialY = pos.y + displacement.medial.y;
        int cellMedialX = (int)(testMedialX / cellSize);
        int cellMedialY = (int)(testMedialY / cellSize);

        if (cellMedialX < 0 || cellMedialX >= (int)columns || cellMedialY < 0 || cellMedialY >= (int)rows) {
            verdict.medial = true;
        } else {
            verdict.medial = field[cellMedialY][cellMedialX];
        }

        // Controllo Lateral
        float testLateralX = pos.x + displacement.lateral.x;
        float testLateralY = pos.y + displacement.lateral.y;
        int cellLateralX = (int)(testLateralX / cellSize);
        int cellLateralY = (int)(testLateralY / cellSize);

        if (cellLateralX < 0 || cellLateralX >= (int)columns || cellLateralY < 0 || cellLateralY >= (int)rows) {
            verdict.lateral = true;
        } else {
            verdict.lateral = field[cellLateralY][cellLateralX];
        }

        return verdict;
    }

    /*
    i vibecoded this function cuz wtf is this, fuck math
    i WILL learn how to do this properly tho
    */
    void rayCast(Vector2 pos, float angle, float fov) {
        distances.resize(rayCount);

        // every ray
        for (int i = 0; i < rayCount; i++) {
            float rayAngle = (angle - fov * 0.5f) + (fov * i) / (float)rayCount;
            Vector2 direction = { cosf(rayAngle), sinf(rayAngle) };

            // current cell
            int mapX = (int)(pos.x / cellSize);
            int mapY = (int)(pos.y / cellSize);

            // distance from next cell border
            Vector2 distance = {
                (direction.x == 0.0f) ? 1e30f : fabsf(1.0f / direction.x),
                (direction.y == 0.0f) ? 1e30f : fabsf(1.0f / direction.y)
            };


            float sideX, sideY;
            int stepX, stepY;

            // Direzione di avanzamento e distanza al primo bordo
            if (direction.x < 0) {
                stepX = -1;
                sideX = ((pos.x / cellSize) - mapX) * distance.x;
            } else {
                stepX = 1;
                sideX = (mapX + 1.0f - (pos.x / cellSize)) * distance.x;
            }

            if (direction.y < 0) {
                stepY = -1;
                sideY = ((pos.y / cellSize) - mapY) * distance.y;
            } else {
                stepY = 1;
                sideY = (mapY + 1.0f - (pos.y / cellSize)) * distance.y;
            }

            // DDA
            bool hit = false;
            bool sideHit = false; // false = vertical, true = horizontal

            while (!hit) {
                if (sideX < sideY) {
                    sideX += distance.x;
                    mapX += stepX;
                    sideHit = false;
                } else {
                    sideY += distance.y;
                    mapY += stepY;
                    sideHit = true;
                }

                if (mapX < 0 || mapX >= (int)columns || mapY < 0 || mapY >= (int)rows) break;
                if (field[mapY][mapX]) hit = true;
            }

        
            if (!hit) {
                distances[i] = 1000.0f;
            } else {
                float perpDist = (!sideHit) ? (sideX - distance.x) : (sideY - distance.y);
                distances[i] = perpDist * cellSize;
            }
        }
    }

    void draw(Vector2 window) {
        // background
        DrawRectangle(0, 0, window.x, window.y * 0.5f, DARKGRAY);
        DrawRectangle(0, window.y * 0.5f, window.x, window.y * 0.5f, LIGHTGRAY);
        
        // walls
        float thickness = window.x / rayCount;

        for (int i = 0; i < rayCount; i++) {
            if (distances[i] < 0.1f) distances[i] = 0.1f;

            // distance > height
            float height = (cellHeight * window.y) / distances[i];
            if (height > window.y) height = window.y;

            // distance > color
            float maxVisibility = 400.0f;
            float visibility = 1.0f - (distances[i] / maxVisibility);
            if (visibility < 0.0f) visibility = 0.0f;
            if (visibility > 1.0f) visibility = 1.0f;

            Color color = {
                0,
                0,
                static_cast<unsigned char>(255 * visibility),
                255
            };      
            
            // draw
            float posY = (window.y * 0.5f) - (height * 0.5f);
            DrawRectangle(i * thickness, posY, thickness + 1.f, height, color);
        }
    }
};



#endif
