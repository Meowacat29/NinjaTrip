#pragma once

// internal
#include "common.hpp"
#include "player.hpp"
#include "moving_enemy.hpp"
#include "coin.hpp"
#include "ladder.hpp"
#include "terrain.hpp"
#include "water.hpp"
#include "pitfall.hpp"
#include "door.hpp"
#include "key.hpp"
#include "invisible_wall.hpp"
#include "bush.hpp"
#include "archer.hpp"
#include "arrow.hpp"
#include "grid_back.hpp"
#include "rope.hpp"
#include "bladeTrap.hpp"
#include "background.hpp"
#include "particle.hpp"
#include "yeti.hpp"
#include "ui/mouse.hpp"
#include "ui/icon.hpp"
#include "ui/ui_bg.hpp"

// stlib
#include <vector>
#include <random>
#include <time.h>

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mixer.h>

// Container for all our entities and game logic. Individual rendering / update is
// deferred to the relative update() methods
class World
{
public:
	World();
	~World();

	//display level design from txt file
	bool display_level(const char* fileName);

	// Creates a window, sets up events and begins the game
	bool init(vec2 screen);

	// Releases all associated resources
	void destroy();

	// Steps the game ahead by ms milliseconds
	bool update(float ms);

	bool update_game_entities(float);

	// Renders our scene
	void draw();

	// Should the game be over ?
	bool is_over()const;

	bool spawn_particle();

	bool spawn_background();

	// Generates a new turtle
	bool spawn_enemy(vec2);
	bool spawn_coin();
	bool spawn_terrain(char type);
	bool spawn_ladder();
	bool spawn_pitfall();
	bool spawn_key();
	bool spawn_bush();
	bool spawn_archer(vec2 archer_position);
	bool spawn_arrow();
	bool spawn_rope();
	bool spawn_invisible();
	bool spawn_blade_trap();
	bool spawn_yeti(vec2);
	// !!! INPUT CALLBACK FUNCTIONS
	void on_key(GLFWwindow*, int key, int, int action, int mod);
	void on_mouse_move(GLFWwindow* window, double xpos, double ypos);
	void on_mouse_click(GLFWwindow* window, int button, int action, int mods);
	void player_death();
	void reset_world();
	void pause_level();
	void resume_level();
	bool save_level(int level);
	bool reload_level(int level);
	bool check_collision();
	bool initiate_game_entities();
	bool initiate_ui_components();
	void updateUIcomponents();
	void updateUI_key(bool visibility);
	bool spawn_updatable_entities();
	bool spawn_game_entities();
	void initiate_level();
	bool initiate_music();
	bool do_particles();
	bool check_player_out_of_bound(float);
	void check_player_on_ground();
	void load_level();
	void set_GameMode(std::string gameMode);
	bool check_GameMode(std::string gameMode);

	std::vector<std::string> split_line(std::string s, char c);

	bool has_saved_game(int levelIndex);
	void set_level(int level);
	int get_level();
	void check_health_on_change();
	void health_on_change();
	void turnoff_icons_hover();
private:
	// Window handle
	GLFWwindow* m_window;

	// Screen texture
	// The draw loop first renders to this texture, then it is used for the water shader
	GLuint m_frame_buffer;
	Texture m_screen_tex;

	// Background effect
	Water m_water;

	// Number of coins eaten by the player, displayed in the window title
	unsigned int m_points;

	//The backend grid
	grid_back *gridBack;

	//mouse cursor
	Mouse m_mouse;

	// Game entities
	Background m_background;
	Player m_player;
	Door m_door;
	std::vector<Key> m_key;
	std::vector<moving_enemy> m_enemy;
	std::vector<Coin> m_coin;
	std::vector<Terrain> m_terrain;
	std::vector<Pitfall> m_pitfall;
	std::vector<Ladder> m_ladder;
	std::vector<Invisible_Wall> m_invisible_wall;
	std::vector<Bush> m_bush;
	std::vector<Archer> m_archers;
	std::vector<Arrow> m_arrows;
	std::vector<Rope> m_ropes;
	std::vector<BladeTrap> m_blade_traps;
	std::vector<Particle> m_particle;
	std::vector<Yeti> m_yeti;

	float m_current_speed;
	float m_next_turtle_spawn;
	float m_next_coin_spawn;
	float m_next_terrain_spawn;
	float m_next_pitfall_spawn;
	float m_next_ladder_spawn;

	clock_t m_start_time;
	int m_frames;
	int m_fps;
	float m_next_fps_calculation;

	Mix_Music* m_background_music;
	Mix_Chunk* m_player_dead_sound;
	Mix_Chunk* m_player_hurt;
	Mix_Chunk* m_player_eat_sound;
	Mix_Chunk* m_click_sound;
	Mix_Chunk* m_yeti_hurt;
	Mix_Chunk* m_yeti_death;
	Mix_Chunk* m_player_get_key;
	Mix_Chunk* m_door_open_sound;
	// C++ rng
	std::default_random_engine m_rng;
	std::uniform_real_distribution<float> m_dist; // default 0..1

	int particles_timer;
	bool climb_isable;
	bool is_pause;
	int health;
	std::string mode;

	//game mode
	std::string GAME_MODE;

	//UI backgrounds
	Ui_bg m_bg;
	Ui_bg bg_main;
	Ui_bg bg_pause;
	Ui_bg bg_instruction;
	Ui_bg bg_death;
	Ui_bg bg_stageClear;
	Ui_bg bg_save;
	Ui_bg bg_load;

	//UI icons
	std::vector<Icon> m_icons; //the icons that currently displayed
	std::vector<Icon> m_level_icons;
	std::vector<Icon> m_score_icons;
	std::vector<Icon> m_health_icons;
	Icon i_start;
	Icon i_quit;
	Icon i_back;
	Icon i_pause;
	Icon i_info;
	Icon i_resume;
	Icon i_menu;
	Icon i_ok;
	Icon i_restart;
	Icon i_yes;
	Icon i_no;
	Icon i_save;
	Icon i_key;
	Icon i_goldcoin;
};
