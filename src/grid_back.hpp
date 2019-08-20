//
// Created by Andy Liu on 8/2/19.
//

#ifndef INC_436D_GRID_BACK_H
#define INC_436D_GRID_BACK_H

#include "common.hpp"
#include <vector>

enum tile_type{
    TERRAIN, TERRAIN_BLACK, AIR, PITFALL, INVISIBLE, LADDER, DOOR, KEY, BUSH, COIN, ENEMY, YETI, ARCHER, ROPE, BLADETRAP
};
class grid_back {


public:
    grid_back(int width, int height);
    std::vector<std::vector<std::vector<tile_type>>> get_grid();
    std::vector<tile_type> get_tile(int x, int y);
    void add_tile(int x, int y,tile_type type);
    void remove_all_for_every_tile();
    void remove_all(int y, int x);
    bool remove(int y, int x, tile_type type);
    bool remove_type(tile_type type);
	std::vector<vec2> get_blade_trap();
    std::vector<vec2> get_terrain(); //gets the positions that the terrain that are loaded in
    std::vector<vec2> get_terrain_black(); //gets the positions that the terrain (balck) that are loaded in
    std::vector<vec2> get_invisible(); // gets the positions of the invisible walls placed on the left and right edges of the screen
    std::vector<vec2> get_pitfall(); //gets the positions of the pitfall that are loaded in
    std::vector<vec2> get_ladder(); //gets the positions of the ladder that are loaded in
    std::vector<vec2> get_key(); //gets the positions of the ladders
    std::vector<vec2> get_bush(); //gets the positions of the bushes
    std::vector<vec2> get_coin(); //gets the positions of the coins
    std::vector<vec2> get_enemy(); //gets the positions of the enemies
    std::vector<vec2> get_archer(); //gets the positions of the archers
    std::vector<vec2> get_rope(); //gets the positions of the ropes
    std::vector<vec2> get_yeti(); //gets the positions of the yeti
    vec2 get_door(); //gets the positions of the door that are loaded in
    float get_tile_size();
    int get_grid_height(); //as in how many tiles are in the height
    int get_grid_width(); //how many tiles are in the width
    ~grid_back();

private:
    std::vector<std::vector<std::vector<tile_type>>> tiles;
    float tile_size;
    int grid_length;
    int grid_height;
};


#endif //INC_436D_GRID_BACK_H
