//
// Created by Andy Liu on 8/2/19.
//

#include "grid_back.hpp"
#include <iostream>

std::vector<std::vector<std::vector<tile_type>>> grid_back::get_grid() {
    return tiles;
}

std::vector<tile_type> grid_back::get_tile(int x,int y) {
    return tiles[x][y];
}

float grid_back::get_tile_size() {
    return tile_size;
}

int grid_back::get_grid_width() {
    return 0;
}

int grid_back::get_grid_height() {
    return 0;
}

grid_back::grid_back(int width, int height) {

#if _WIN32
	tile_size = 40.f;			   // Under windows this should be less than 40.f
#elif __APPLE__
	tile_size = 80.f;			   // Under windows this should be less than 40.f
#elif __linux__
    // linux
#endif


    //Constructor for generating an empty grid
    //num_height/width give how many tiles there are in a width and height
    //TODO: need to change the height, length and the tile   size
    grid_length = width/tile_size; //a grid tile is of size 80 x 80, note that under Windows the tile_size should be less than half of this number
	// since the number of pixels is halved (so the size of rows & columns are halved) otherwise out of bound error would be caused.
    grid_height = height/tile_size;
	std::cout << " width: " << grid_length << "  height: " << grid_height << std::endl;
    std::vector<tile_type> tile;
    std::vector<std::vector<tile_type>> tile_column(grid_length,tile);
    std::vector<std::vector<std::vector<tile_type>>> tile_row(grid_height,tile_column);
    tiles = tile_row;
}

grid_back::~grid_back(){
    remove_all_for_every_tile();
    tiles.clear();
}

void grid_back::add_tile(int y, int x,tile_type type) {
		tiles[x][y].push_back(type);
}

void grid_back::remove_all_for_every_tile(){
    for (int i = 0; i < grid_length; i++) {
        for (int j = 0; j < grid_height; j++) {
            remove_all(i,j);
        }
    }
}

void grid_back::remove_all(int y, int x){
    unsigned long size = tiles[x][y].size();
    for(int i =0; i < size; i++){
        tiles[x][y].pop_back();
    }
    tiles[x][y].clear();
}

bool grid_back::remove_type(tile_type type){
    for (int k = 0; k < grid_length; k++) {
        for (int j = 0; j < grid_height; j++) {
            remove(k, j, type);
        }
    }
    // fprintf(stdout, "finish remove type");
    return true;
}

bool grid_back::remove(int y, int x, tile_type type){
    unsigned long size = tiles[x][y].size();
    for(int i = 0; i < size; i++){
        if(tiles[x][y].at(i) == type){
            tiles[x][y].erase(tiles[x][y].begin()+i);
        }
    }
    return false;
}
std::vector<vec2> grid_back::get_blade_trap()
{
	std::vector<vec2> blade_trap;
	for (int i = 0; i < grid_height; i++) {
		for (int j = 0; j < grid_length; j++) {
			for (auto it = tiles[i][j].begin(); it != tiles[i][j].end(); it++) {
				if (*it == BLADETRAP) {
					blade_trap.push_back({ (float)j*tile_size,(float)i*tile_size });
				}
			}
		}
	}
	return blade_trap;
}

std::vector<vec2> grid_back::get_terrain() {
    std::vector<vec2> terrains;
    for(int i = 0; i < grid_height; i++){
        for(int j = 0; j < grid_length; j++){
            for(auto it = tiles[i][j].begin(); it != tiles[i][j].end(); it++){
                if(*it == TERRAIN){
                    terrains.push_back({(float)j*tile_size,(float)i*tile_size});
                }
            }
        }
    }
    return terrains;
}

std::vector<vec2> grid_back::get_terrain_black() {
    std::vector<vec2> terrains_black;
    for(int i = 0; i < grid_height; i++){
        for(int j = 0; j < grid_length; j++){
            for(auto it = tiles[i][j].begin(); it != tiles[i][j].end(); it++){
                if(*it == TERRAIN_BLACK){
                    terrains_black.push_back({(float)j*tile_size,(float)i*tile_size});
                }
            }
        }
    }
    return terrains_black;
}

std::vector<vec2> grid_back::get_key() {
    std::vector<vec2> key;
    for(int i = 0; i < grid_height; i++){
        for(int j = 0; j < grid_length; j++){
            for(auto it = tiles[i][j].begin(); it != tiles[i][j].end(); it++){
                if(*it == KEY){
                    key.push_back({(float)j*tile_size,(float)i*tile_size});
                }
            }
        }
    }
    return key;
}

vec2 grid_back::get_door() {
    for(int i = 0; i < grid_height; i++){
        for(int j = 0; j < grid_length; j++){
            for(auto it = tiles[i][j].begin(); it != tiles[i][j].end(); it++){
                if(*it == DOOR){
                    return {(float)j*tile_size,(float)i*tile_size};
                }
            }
        }
    }
    return {};
}

std::vector<vec2> grid_back::get_pitfall() {
    std::vector<vec2> traps;
    for(int i = 0; i < grid_height; i++){
        for(int j = 0; j < grid_length; j++){
            for(auto it = tiles[i][j].begin(); it != tiles[i][j].end(); it++){
                if(*it == PITFALL){
                    traps.push_back({(float)j*tile_size,(float)i*tile_size});
                }
            }
        }
    }
    return traps;
}
std::vector<vec2> grid_back::get_ladder() {
    std::vector<vec2> ladders;
    for(int i = 0; i < grid_height; i++){
        for(int j = 0; j < grid_length; j++){
            for(auto it = tiles[i][j].begin(); it != tiles[i][j].end(); it++){
                if(*it == LADDER){
                    ladders.push_back({(float)j*tile_size,(float)i*tile_size});
                }
            }
        }
    }
    return ladders;
}
// std::vector<vec2> grid_back::get_invisible() {
//     std::vector<vec2> invisible;
//     for(int i = 0; i < grid_height; i++){
//         for(int j = 0; j < grid_length; j++){
//             for(auto it = tiles[i][j].begin(); it != tiles[i][j].end(); it++){
//                 if(*it == INVISIBLE){
//                     invisible.push_back({(float)j*tile_size,(float)i*tile_size});
//                 }
//             }
//         }
//     }
//     return invisible;
// }
std::vector<vec2> grid_back::get_bush() {
    std::vector<vec2> bush;
    for(int i = 0; i < grid_height; i++){
        for(int j = 0; j < grid_length; j++){
            for(auto it = tiles[i][j].begin(); it != tiles[i][j].end(); it++){
                if(*it == BUSH){
                    bush.push_back({(float)j*tile_size,(float)i*tile_size});
                }
            }
        }
    }
    return bush;
}
std::vector<vec2> grid_back::get_coin() {
    std::vector<vec2> coin;
    for(int i = 0; i < grid_height; i++){
        for(int j = 0; j < grid_length; j++){
            for(auto it = tiles[i][j].begin(); it != tiles[i][j].end(); it++){
                if(*it == COIN){
                    coin.push_back({(float)j*tile_size,(float)i*tile_size});
                }
            }
        }
    }
    return coin;
}
std::vector<vec2> grid_back::get_enemy() {
    std::vector<vec2> enemy;
    for(int i = 0; i < grid_height; i++){
        for(int j = 0; j < grid_length; j++){
            for(auto it = tiles[i][j].begin(); it != tiles[i][j].end(); it++){
                if(*it == ENEMY){
                    enemy.push_back({(float)j*tile_size,(float)i*tile_size});
                }
            }
        }
    }
    return enemy;
}
std::vector<vec2> grid_back::get_yeti() {
    std::vector<vec2> yeti;
    for(int i = 0; i < grid_height; i++){
        for(int j = 0; j < grid_length; j++){
            for(auto it = tiles[i][j].begin(); it != tiles[i][j].end(); it++){
                if(*it == YETI){
                    yeti.push_back({(float)j*tile_size,(float)i*tile_size - 0.5f * tile_size});
                }
            }
        }
    }
    return yeti;
}
std::vector<vec2> grid_back::get_archer() {
    std::vector<vec2> archer;
    for(int i = 0; i < grid_height; i++){
        for(int j = 0; j < grid_length; j++){
            for(auto it = tiles[i][j].begin(); it != tiles[i][j].end(); it++){
                if(*it == ARCHER){
                    archer.push_back({(float)j*tile_size,(float)i*tile_size});
                }
            }
        }
    }
    return archer;
}

std::vector<vec2> grid_back::get_rope() {
    std::vector<vec2> ropes;
    for(int i = 0; i < grid_height; i++){
        for(int j = 0; j < grid_length; j++){
            for(auto it = tiles[i][j].begin(); it != tiles[i][j].end(); it++){
                if(*it == ROPE){
                    ropes.push_back({(float)j*tile_size,(float)i*tile_size});
                }
            }
        }
    }
    return ropes;
}
