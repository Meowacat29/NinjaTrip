// Header
#include "world.hpp"
#include "grid_back.hpp"

// stlib
#include <string.h>
#include <cassert>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <vector>
#include <fstream>
#include <unordered_map>
#include <SDL_filesystem.h>

// #define _CRTDBG_MAP_ALLOC
// #include <stdlib.h>
// #include <crtdbg.h>

// Same as static in c, local to compilation unit
namespace
{
	const bool CAMERA_ZOOM_OFF = true;
	const size_t TURTLE_DELAY_MS = 2000;
	const size_t MAX_PARTICLE = 100;
	const size_t FPS_DELAY = 250;
	const int LEVEL_COUNT = 6;
	const vec2 MAP_SIZE = {14, 31}; // as a matrix in the map with maximum size MAP_SIZE
	const char DISPLAY_ORDER[] = {'t','T','p','c','b','e','y','d','k','r','l','a','s'};  // the order of building elements
	const char *levels[LEVEL_COUNT] = {"../data/levelDesign/level0.txt", "../data/levelDesign/level1.txt","../data/levelDesign/level2.txt",
		"../data/levelDesign/level3.txt","../data/levelDesign/level4.txt","../data/levelDesign/level5.txt"};
	const char *savePath = "../data/saveLoad/";
	int level_index = 0;
	bool is_pause = false;
	namespace
	{
		void glfw_err_cb(int error, const char* desc)
		{
			fprintf(stderr, "%d: %s", error, desc);
		}
	}
}

World::World() :
	m_points(0),
	m_next_turtle_spawn(0.f),
	m_next_terrain_spawn(0.f)

{
	// Seeding rng with random device
	m_rng = std::default_random_engine(std::random_device()());
}

World::~World()
{
	for (int i = 0; i<LEVEL_COUNT; i++){
		levels[i] = NULL;
	}
	savePath = NULL;
	delete gridBack;
};

/*

 * REPRESENTATIONS:

;(semicolon) = end of current line, any words behind will be ignored.
space = nothing (default)
t = terrain
T = terrain_black
p = pitfall
c = coin
b = bush
e = enemy
d = door
k = key
l = ladder
a = archer
r = rope
s = blade trap

NOTE: particles has a different distribution in the game, so they are not
considered in the current representation.
*/

// display level design from a given txt file following the rules above,
// works for any map smaller or equal to MAP_SIZE, limited by background img size.
// Any larger map will have segment fault.
bool World::display_level(const char* fileName){
	std::ifstream f(fileName);
	std::string line;
	const char *p;	// as a temp finder
	std::unordered_map<char, std::vector<vec2Int>> umap; // as a dataset
	umap.reserve(12);

	if (f.is_open()){
		//process data to dataset
		int row = 0;
		while (getline(f,line))
    	{
			// fprintf(stderr, "%s\n", line.c_str());
			for(int col = 0; col < line.size(); ++col) {
    			if (line[col] == ';') {
						break;
				}else{
					p = std::find (DISPLAY_ORDER, DISPLAY_ORDER + sizeof(DISPLAY_ORDER), line[col]);
					if (p != std::end(DISPLAY_ORDER)) {
						// if found, add to dataset; else ignore
						umap[line[col]].push_back(vec2Int{col, row});
						// fprintf(stderr, "%c, %d, %d, PROCESSING\n",line[col], row, col);
						if (row > MAP_SIZE.x || col > MAP_SIZE.y) {
							fprintf(stderr, "WARNING: map need to be in MAP_SIZE, but you have row %d, col %d \n", row, col);
							return false;
						}
					}
				}
			}
			row++;
    	}
    	f.close();

		//handle data from dataset
		for (char ele: DISPLAY_ORDER) {
			switch(ele){
				case (char)'t':
					for (vec2Int coord: umap[ele]){
						// fprintf(stderr, "%d, %d, TERRAIN\n",coord.x, coord.y);
						gridBack->add_tile(coord.x, coord.y,TERRAIN);
					}
					umap[ele].clear();
					break;
				case (char)'T':
					for (vec2Int coord: umap[ele]){
						// fprintf(stderr, "%d, %d, TERRAIN\n",coord.x, coord.y);
						gridBack->add_tile(coord.x, coord.y,TERRAIN_BLACK);
					}
					umap[ele].clear();
					break;
				case (char)'p':
					for (vec2Int coord: umap[ele]){
						// fprintf(stderr, "%d, %d, PITFALL\n",coord.x, coord.y);
						gridBack->add_tile(coord.x, coord.y,PITFALL);
					}
					umap[ele].clear();
					break;
				case (char)'s':
					for (vec2Int coord : umap[ele]) {
						// fprintf(stderr, "%d, %d, Bladetrap\n", coord.x, coord.y);
						gridBack->add_tile(coord.x, coord.y, BLADETRAP);
					}
					umap[ele].clear();
					break;
				case (char)'c':
					for (vec2Int coord: umap[ele]){
						gridBack->add_tile(coord.x, coord.y,COIN);
					}
					umap[ele].clear();
					break;
				case (char)'b':
					for (vec2Int coord: umap[ele]){
						gridBack->add_tile(coord.x, coord.y,BUSH);
					}
					umap[ele].clear();
					break;
				case (char)'e':
					for (vec2Int coord: umap[ele]){
						gridBack->add_tile(coord.x, coord.y,ENEMY);
					}
					umap[ele].clear();
					break;
				case (char)'y':
					for (vec2Int coord: umap[ele]){
						gridBack->add_tile(coord.x, coord.y,YETI);
					}
					umap[ele].clear();
					break;
				case (char)'d':
					for (vec2Int coord: umap[ele]){
						gridBack->add_tile(coord.x, coord.y,DOOR);
					}
					umap[ele].clear();
					break;
				case (char)'k':
					for (vec2Int coord: umap[ele]){
						gridBack->add_tile(coord.x, coord.y,KEY);
					}
					umap[ele].clear();
					break;
				case (char)'r':
					for (vec2Int coord: umap[ele]){
						gridBack->add_tile(coord.x, coord.y,ROPE);
					}
					umap[ele].clear();
					break;
				case (char)'l':
					for (vec2Int coord: umap[ele]){
						gridBack->add_tile(coord.x, coord.y,LADDER);
					}
					umap[ele].clear();
					break;
				case (char)'a':
					for (vec2Int coord: umap[ele]){
						gridBack->add_tile(coord.x, coord.y,ARCHER);
					}
					umap[ele].clear();
					break;
			}
		}
		umap.clear();

		return true;
	}

	fprintf(stderr, "FAIL in loading  file for display_level\n");
	return false;
}

// World initialization
bool World::init(vec2 screen) {
	//-------------------------------------------------------------------------
	// GLFW / OGL Initialization
	// Core Opengl 3.
	glfwSetErrorCallback(glfw_err_cb);
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW");
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 1);
#if __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	glfwWindowHint(GLFW_RESIZABLE, 0);

	m_window = glfwCreateWindow((int)screen.x, (int)screen.y, "Team 8 Project", nullptr, nullptr);
	if (m_window == nullptr)
		return false;

	glfwMakeContextCurrent(m_window);
	glfwSwapInterval(1); // vsync

	// Load OpenGL function point
	gl3w_init();

	// Setting callbacks to member functions (that's why the redirect is needed)
	// Input is handled using GLFW, for more info see
	// http://www.glfw.org/docs/latest/input_guide.html
	glfwSetWindowUserPointer(m_window, this);
	auto key_redirect = [](GLFWwindow* wnd, int _0, int _1, int _2, int _3) { ((World*)glfwGetWindowUserPointer(wnd))->on_key(wnd, _0, _1, _2, _3); };
	auto cursor_pos_redirect = [](GLFWwindow* wnd, double _0, double _1) { ((World*)glfwGetWindowUserPointer(wnd))->on_mouse_move(wnd, _0, _1); };
	auto mouse_click_event = [](GLFWwindow* wnd, int _0, int _1, int _2) { ((World*)glfwGetWindowUserPointer(wnd))->on_mouse_click(wnd, _0, _1, _2); };

	glfwSetKeyCallback(m_window, key_redirect);
	glfwSetCursorPosCallback(m_window, cursor_pos_redirect);
	glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	glfwSetMouseButtonCallback(m_window, mouse_click_event);

	// Create a frame buffer
	m_frame_buffer = 0;
	glGenFramebuffers(1, &m_frame_buffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_frame_buffer);

	// Initialize the screen texture
	m_screen_tex.create_from_screen(m_window);

	// Current game speed
	m_current_speed = 0.4f;
	int w, h;
	glfwGetFramebufferSize(m_window, &w, &h);
	gridBack = new grid_back(w,h);

	set_GameMode("MENU");

	// Loading music and sounds
	if (!initiate_music()) {
		fprintf(stderr, "Failed to initiate music and sound!\n");
		return false;
	}

	if (!m_player.init()) {
		fprintf(stderr, "Failed to initiate player!\n");
		return false;
	}

	char const *levelFile =levels[level_index];
	if(!display_level(levelFile)){
		fprintf(stderr, "Failed to load level design file!\n");
		return false;
	};

	if (!initiate_game_entities()) {
		fprintf(stderr, "Failed to initiate game entities!\n");
		return false;
	}

	if(!initiate_ui_components()){
		fprintf(stderr, "Failed to initiate ui components!\n");
		return false;
	}

	updateUIcomponents();

	if (!m_mouse.init()) {
		fprintf(stderr, "Failed to initiate mouse!\n");
		return false;
	}

	if (!m_background.init(screen)) {
		fprintf(stderr, "Failed to initiate background!\n");
		return false;
	}

	if (!m_water.init()) {
		fprintf(stderr, "Failed to initiate water!\n");
		return false;
	}

	// Initialize values for FPS calculation
	m_start_time = clock();
	m_frames = 0;
	m_fps = 0;

	return true;
}

bool World::initiate_ui_components(){
	//initiate ui background
	if(!bg_main.init('m') || !bg_pause.init('p') || !bg_instruction.init('i')
		|| !bg_death.init('d') || !bg_stageClear.init('c') || !bg_save.init('s')
			|| !bg_load.init('l'))
	{
		fprintf(stderr, "Failed to initialize ui backgrounds! ");
		return false;
	}

	//initiate level selection box
	for(int i = 0; i != LEVEL_COUNT; i++) {
	  	Icon i_level;
	  	i_level.init((char)('0' + i));
	    m_level_icons.emplace_back(i_level);
	}

	//initiate health point 0-3 using ASCII W(87) to Z(90)
	for(int i = 87; i < 91; i++){
		Icon i_health;
		if(!i_health.init(char(i)))
		{
			fprintf(stderr, "Failed to initialize score numbers! ");
			return false;
		}
		m_health_icons.emplace_back(i_health);
	}

	//initiate score 0-9 using ASCII A(65) to J(74)
	for(int i = 65; i != 75; i++) {
	  	Icon i_score;
	  	if(!i_score.init(char(i)))
		{
			fprintf(stderr, "Failed to initialize score numbers! ");
			return false;
		}
	  	m_score_icons.emplace_back(i_score);
	}

	//initiate ui icons; static + dynamic
	if (!i_start.init('s') || !i_quit.init('q') || !i_back.init('b') ||
		!i_pause.init('p') || !i_info.init('i') || !i_resume.init('c') ||
		!i_menu.init('m') || !i_ok.init('o') || !i_restart.init('r') ||
		!i_yes.init('y') || !i_no.init('n') || !i_save.init('v') ||
		!i_key.init('k') || !i_goldcoin.init('g'))
	{
		fprintf(stderr, "Failed to initialize ui icons! ");
		return false;
	}

	return true;
}

bool World::initiate_music() {
	if (SDL_Init(SDL_INIT_AUDIO) < 0)
	{
		fprintf(stderr, "Failed to initialize SDL Audio");
		return false;
	}

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1)
	{
		fprintf(stderr, "Failed to open audio device");
		return false;
	}

	// if (level_index <= 2) {
	// 	m_background_music = Mix_LoadMUS(audio_path("storm.wav"));
	// }
	// else if (level_index <= 4) {
	// 	m_background_music = Mix_LoadMUS(audio_path("new_music.wav"));
	// }
	// else
		m_background_music = Mix_LoadMUS(audio_path("new_music.wav"));

	m_player_dead_sound = Mix_LoadWAV(audio_path("player_dead.wav"));
	m_player_eat_sound = Mix_LoadWAV(audio_path("player_eat.wav"));
	m_player_hurt = Mix_LoadWAV(audio_path("player_hurt.wav"));
	m_player_get_key = Mix_LoadWAV(audio_path("key.wav"));
	m_click_sound = Mix_LoadWAV(audio_path("click.wav"));
	m_door_open_sound = Mix_LoadWAV(audio_path("door.wav"));


	m_yeti_hurt = Mix_LoadWAV(audio_path("yeti_hurt.wav"));
	m_yeti_death = Mix_LoadWAV(audio_path("yeti_dying.wav"));

	if (m_background_music == nullptr || m_player_dead_sound == nullptr || m_player_eat_sound == nullptr || m_click_sound == nullptr
		|| m_yeti_hurt == nullptr || m_yeti_death == nullptr)
	{
		fprintf(stderr, "Failed to load sounds\n %s\n %s\n %s\n %s\n %s\n %s\n make sure the data directory is present",
			audio_path("new_music.wav"),
			audio_path("player_dead.wav"),
			audio_path("player_hurt.wav"),
			audio_path("yeti_dying.wav"),
			audio_path("yeti_hurt.wav"),
			audio_path("player_eat.wav"));
		return false;
	}

	// Playing background music undefinitely
	Mix_PlayMusic(m_background_music, -1);

	fprintf(stderr, "Loaded music\n");
	return true;
}

void World::destroy() {
	glDeleteFramebuffers(1, &m_frame_buffer);

	if (m_background_music != nullptr)
		Mix_FreeMusic(m_background_music);
	if (m_player_dead_sound != nullptr)
		Mix_FreeChunk(m_player_dead_sound);
	if (m_player_eat_sound != nullptr)
		Mix_FreeChunk(m_player_eat_sound);
	if (m_click_sound != nullptr)
		Mix_FreeChunk(m_click_sound);
	if (m_yeti_hurt != nullptr)
		Mix_FreeChunk(m_yeti_hurt);
	if (m_yeti_death != nullptr)
		Mix_FreeChunk(m_yeti_death);
	if (m_player_hurt != nullptr)
		Mix_FreeChunk(m_player_hurt);

	Mix_CloseAudio();

	m_player.destroy();
	m_door.destroy();
	m_background.destroy();
	m_mouse.destroy();
	for (auto& particle : m_particle)
		particle.destroy();
	for (auto& key : m_key)
		key.destroy();
	for (auto& turtle : m_enemy){
		turtle.clean();
		turtle.destroy();
	}
	for (auto& yeti : m_yeti){
		yeti.clean();
		yeti.destroy();
	}
	for (auto& archer : m_archers){
		archer.clean();
		archer.destroy();
	}
	for (auto& arrow : m_arrows)
		arrow.destroy();
	for (auto& coin : m_coin)
		coin.destroy();
	for (auto& pitfall : m_pitfall)
		pitfall.destroy();
	for (auto& terrain : m_terrain)
		terrain.destroy();
	for (auto& ladder : m_ladder)
		ladder.destroy();
	for (auto& rope : m_ropes)
		rope.destroy();
	for (auto& bush : m_bush)
		bush.destroy();
	m_particle.clear();
	m_archers.clear();
	m_arrows.clear();
	m_coin.clear();
	m_pitfall.clear();
	m_terrain.clear();
	m_ladder.clear();
	m_ropes.clear();
	glfwDestroyWindow(m_window);
}

// Update our game world
bool World::update(float elapsed_ms) {
	check_health_on_change();
	if (is_pause){
		// do nothing, end this round
		return true;
	}

	if (!m_player.is_alive()){
		return false;
	}

	int w, h;
	glfwGetFramebufferSize(m_window, &w, &h);
	vec2 screen = { (float)w, (float)h };

	// Checking if player falls below the window
	if (!check_player_out_of_bound(h)) {
		fprintf(stdout,"Player is out of bound\n");
		return false;
	}

	if (m_player.get_position().x == INFINITY || m_player.get_position().y == INFINITY) {
		// TODO: fix
		//fprintf(stdout,"invalid position values because of player is rendered in x = +inf, no idea why!\n");
		//player_death();
		return false;
	}

	if (!spawn_updatable_entities()) {
		fprintf(stdout,"Fail to spawn updatable entities!\n");
		return false;
	}

	if (!check_collision()) {
		fprintf(stdout,"Character died!\n");
		return false;
	}

	check_player_on_ground();

	// Updating all entities, making the turtle faster based on current
	update_game_entities(elapsed_ms);

	if (!do_particles()) {
		fprintf(stdout,"Fail to process particles!\n");
		return false;
	}

	// Calculate FPS every 0.25 seconds
	// m_next_fps_calculation -= elapsed_ms;
	m_next_fps_calculation += (clock() - m_start_time) / CLOCKS_PER_SEC;
	if (m_next_fps_calculation >= 1) {
		m_fps = m_frames / m_next_fps_calculation;
		m_start_time = clock();
		m_frames = 0;
		m_next_fps_calculation = 0;
	}

	return true;
}

// Render our game world
// http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-14-render-to-texture/
void World::draw() {
	// Clearing error buffer
	gl_flush_errors();

	// Getting size of window
	int w, h;
    glfwGetFramebufferSize(m_window, &w, &h);

	// Updating window title with points and FPS
	std::stringstream title_ss;
	if(check_GameMode("GAME")){
		title_ss << "Points: " << m_points;
		title_ss << " FPS: " << m_fps;
		title_ss << " Health: " << m_player.get_health();
	}
	else
		title_ss << "NINJA TRIP";
	glfwSetWindowTitle(m_window, title_ss.str().c_str());

	/////////////////////////////////////
	// First render to the custom framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, m_frame_buffer);

	// Clearing backbuffer
	glViewport(0, 0, w, h);
	glDepthRange(0.00001, 10);
	const float clear_color[3] = { 0.75f, 0.75f, 0.75f };
	glClearColor(clear_color[0], clear_color[1], clear_color[2], 1.0);
	glClearDepth(1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    float time = glfwGetTime();
	// Fake projection matrix, scales with respect to window coordinates
	// PS: 1.f / w in [1][1] is correct.. do you know why ? (:
	float left = 0.f;// *-0.5; The value is 0
	float top = 0.f;// (float)h * -0.5; The value is 0
	float right = (float)w/2;// *0.5; The value is 2400/2
	float bottom = (float)h/2;// *0.5; the value is 1200/2

	float sx = 2.f / (right - left);
	float sy = 2.f / (top - bottom);
	float tx = -(right + left) / (right - left);
	float ty = -(top + bottom) / (top - bottom);

	mat3 projection_2D_ui{ { sx, 0.f, 0.f },{ 0.f, sy, 0.f},{ tx, ty, 1.f } };
	mat3 projection_2D;

	//TODO: need to make it so that we can fit more tiles in our screen
	// size of the projection
    //currently loading the whole screen
  	//fprintf(stdout,"%d  %d, \n",w, h);
  	//the -50.f is because of the original offset of 50 similar to the offset for y of 100
  	// fprintf(stdout,"%f  %f, \n",right, bottom);

	if(check_GameMode("GAME")){
		float temp_right = right;
		float temp_left = left;
		float temp_bottom = bottom;
		float temp_top = top;
		//TODO i need to get the screen to zoom in say either to screensize/3 or screensize/4.
		//After that i need to make each block size smaller, which also entails me adding all
		//the different entities to the gridback which also entails me changing the gridback struct.
		//yay. Need to check that the actual number of tiles is increased through it. I might need
		// to modify original positions but i don't think i do need to

		//fprintf(stdout,"Yes: %f %f %f %f\n",left,right, top, bottom);
	    //the -50.f is because of the original offset of 50 similar to the offset for y of 100
		left += (m_player.get_position().x-50.f) - 500.f;
		right += (m_player.get_position().x-50.f) - 500.f;
		top += (m_player.get_position().y-100.f) - 200.f;
		bottom += (m_player.get_position().y-100.f) - 200.f;

		if(left <= 0.f){ //if the screen is limited by the left side
			left = temp_left;
			right = temp_right;
		}
		//fprintf(stdout,"Next: %f %f %f %f\n",left,right, top, bottom);
		if(right >= w) {//if the screen is limited by the right side
			left = w/2;
			right = w;
		}
		if(top <= 0.f){//if the screen is limited by the top side
			top = temp_top;
			bottom = temp_bottom;
		}
		if(bottom >= h){//if the screen is limited by the bottom side
			top = h/2;
			bottom = h;
		}

		// For cancelling camera zoom-in, only used for testing
		if (CAMERA_ZOOM_OFF) {
			left = 0.f;
			top = 0.f;
			right = (float)w;
			bottom = (float)h;
		}

		sx = 2.f / (right - left);
		sy = 2.f / (top - bottom);
		tx = -(right + left) / (right - left); //-1 tx translates the screen left and right
		ty = -(top + bottom) / (top - bottom); //1 ty translates the screen up and down
		// fprintf(stdout,"Player's positions: %f, %f\n",m_player.get_position().x,m_player.get_position().y);
		projection_2D = { { sx, 0.f, 0.f },{ 0.f, sy, 0.f },{ tx, ty, 1.f } };

		//drawing game background
		m_background.draw(projection_2D);

		// Drawing entities
		m_door.draw(projection_2D);
		for (auto& particle : m_particle)
			particle.draw(projection_2D);
		for (auto& archer : m_archers)
			archer.draw(projection_2D);
		for (auto& arrow : m_arrows)
			arrow.draw(projection_2D);
		for (auto& terrain : m_terrain)
			terrain.draw(projection_2D);
		for (auto& pitfall : m_pitfall)
			pitfall.draw(projection_2D);
		for (auto& key : m_key)
			key.draw(projection_2D);
		for (auto& ladder : m_ladder)
			ladder.draw(projection_2D);
		for (auto& rope : m_ropes)
			rope.draw(projection_2D);
		for (auto& coin : m_coin)
			coin.draw(projection_2D);
		m_player.draw(projection_2D);
		for (auto& bush : m_bush)
			bush.draw(projection_2D);
		for (auto& blade_trap : m_blade_traps)
			blade_trap.draw(projection_2D);
		for (auto& turtle : m_enemy)	//put enemy in front of bush
			turtle.draw(projection_2D);
		for (auto& yeti : m_yeti)
			yeti.draw(projection_2D);
	}
	else
	{
		//Drawing ui background
		m_bg.draw(projection_2D_ui);
	}

	//drawing ui icons
	for (auto& icon : m_icons){
		icon.draw(projection_2D_ui);
	}

	//Drawing mouse cursor
	m_mouse.draw(projection_2D_ui);

	/////////////////////
	// Truely render to the screen
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Clearing backbuffer
	glViewport(0, 0, w, h);
	glDepthRange(0, 10);
	glClearColor(0, 0, 0, 1.0);
	glClearDepth(1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_screen_tex.id);

	m_water.draw(projection_2D);

	//////////////////
	// Presenting
	glfwSwapBuffers(m_window);

	// Increment frames everytime glfwSwapBuffers is called
	m_frames++;
}

// Should the game be over ?
bool World::is_over()const {
	return glfwWindowShouldClose(m_window);
}

// Creates a new particle and if successfull adds it to the list of particles
bool World::spawn_particle() {
	Particle particle;
	if (particle.init())
	{
		m_particle.emplace_back(particle);
		return true;
	}
	fprintf(stderr, "Failed to spawn particle");
	return false;
}

bool World::spawn_enemy(vec2 enemy_position) {
	moving_enemy turtle;
	if (turtle.init(enemy_position, m_player.get_position()))
	{
		m_enemy.emplace_back(turtle);
		return true;
	}
	fprintf(stderr, "Failed to spawn turtle");
	return false;
}

bool World::spawn_yeti(vec2 yeti_position) {
	Yeti y;
	if (y.init(yeti_position, m_player.get_position()))
	{
		m_yeti.emplace_back(y);
		return true;
	}
	fprintf(stderr, "Failed to spawn yeti");
	return false;
}

// Creates a new archer and if successfull adds it to the list of archers
bool World::spawn_archer(vec2 archer_position) {
	Archer archer;
	if (archer.init(archer_position, m_player.get_position()))
	{
		m_archers.emplace_back(archer);
		return true;
	}
	fprintf(stderr, "Failed to spawn archer");
	return false;
}

// Creates a new arrow and if successfull adds it to the list of arrows
bool World::spawn_arrow() {
	Arrow arrow;
	if (arrow.init())
	{
		m_arrows.emplace_back(arrow);
		return true;
	}
	fprintf(stderr, "Failed to spawn arrow");
	return false;
}

// Creates a new coin and if successfull adds it to the list of coin
bool World::spawn_coin() {
	Coin coin;
	if (coin.init())
	{
		m_coin.emplace_back(coin);
		return true;
	}
	fprintf(stderr, "Failed to spawn coin");
	return false;
}

// Creates a new ladder and if successfull adds it to the list of ladder
bool World::spawn_ladder() {
	Ladder ladder;
	if (ladder.init())
	{
		m_ladder.emplace_back(ladder);
		return true;
	}
	fprintf(stderr, "Failed to spawn ladder");
	return false;
}

// Creates a new pitfall and if successfull adds it to the list of pitfalls
bool World::spawn_pitfall() {
	Pitfall pitfall;
	if (pitfall.init())
	{
		m_pitfall.emplace_back(pitfall);
		return true;
	}
	fprintf(stderr, "Failed to spawn pitfall");
	return false;
}

bool World::spawn_blade_trap() {
	BladeTrap blade_trap;
	if (blade_trap.init())
	{
		m_blade_traps.emplace_back(blade_trap);
		return true;
	}
	fprintf(stderr, "Failed to spawn blade trap");
	return false;
}

// Creates a new terrain and if successfull adds it to the list of terrain
bool World::spawn_terrain(char type) {

	Terrain terrain;
	if (terrain.init(type))
	{
		m_terrain.emplace_back(terrain);
		return true;
	}
	fprintf(stderr, "Failed to spawn terrain");
	return false;
}

// Creates a new key and if successfull adds it to the list of keys
bool World::spawn_key() {
	Key key;
	if (key.init())
	{
		m_key.emplace_back(key);
		return true;
	}
	fprintf(stderr, "Failed to spawn key");
	return false;
}

// Creates a new bush and if successfull adds it to the list of bushes
bool World::spawn_bush() {
	Bush bush;
	if (bush.init())
	{
		m_bush.emplace_back(bush);
		return true;
	}
	fprintf(stderr, "Failed to spawn bush");
	return false;
}

// // Creates a new invisible walls and if successfull adds it to the list of terrain
// bool World::spawn_invisible() {
//     Invisible_Wall invisible_wall;
//     if (invisible_wall.init())
//     {
//         m_invisible_wall.emplace_back(invisible_wall);
//         return true;
//     }
//     fprintf(stderr, "Failed to spawn invisible walls");
//     return false;
// }

// Creates a new rope and if successfull adds it to the list of ropes
bool World::spawn_rope() {
	Rope rope;
	if (rope.init())
	{
		m_ropes.emplace_back(rope);
		return true;
	}
	fprintf(stderr, "Failed to spawn rope");
	return false;
}

// On key callback
void World::on_key(GLFWwindow*, int key, int, int action, int mod) {
	if(!check_GameMode("GAME"))
		return;
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// HANDLE player MOVEMENT HERE
	// key is of 'type' GLFW_KEY_
	// action can be GLFW_PRESS GLFW_RELEASE GLFW_REPEAT
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	// Control the current speed with `<` `>`
	if (action == GLFW_RELEASE && (mod & GLFW_MOD_SHIFT) &&  key == GLFW_KEY_COMMA)
		m_current_speed -= 0.1f;
	if (action == GLFW_RELEASE && (mod & GLFW_MOD_SHIFT) && key == GLFW_KEY_PERIOD)
		m_current_speed += 0.1f;
	m_current_speed = fmax(0.f, m_current_speed);


	//moving the player
	glfwSetInputMode(m_window, GLFW_STICKY_KEYS, 1);
	if(action == GLFW_PRESS){
		switch (key){
			case GLFW_KEY_UP:
				if(m_player.on_ladder){
					m_player.stickonLadder();
					m_player.stopActions();
				}
				if(m_player.on_rope)
					m_player.moveUp = true;
				if (m_player.has_key() && m_player.overlap_with(m_door)) {
					Mix_PlayChannel(-1, m_door_open_sound, 0);
					printf("OPEN DOOR \n");
					if(get_level() == LEVEL_COUNT - 1){
						set_GameMode("STAGECLEAR");
						updateUIcomponents();
					}
					else{
						set_level(get_level()+1);
						load_level();
					}
				}
				break;
			case GLFW_KEY_DOWN:
				if(m_player.on_ladder)
				 	m_player.moveDown = true;
				if(m_player.on_rope)
				 	m_player.moveDown = true;
				break;
			case GLFW_KEY_LEFT:
				if(m_player.on_rope)
				{
				 	m_player.moveLeft = false;
					m_player.swingLeft = true;
				}
				else {
					m_player.disableMomentum = false;
					m_player.addAcceleration(vec2{ -0.45f,0.f });
					m_player.startAccelerating();
					m_player.moveLeft = true;
					//m_player.move({-5.f,0.f});
				}
				break;
			case GLFW_KEY_RIGHT:
				if(m_player.on_rope)
				{
				 	m_player.moveRight = false;
				 	m_player.swingRight = true;
				}
				else {
					m_player.disableMomentum = false;
					m_player.addAcceleration(vec2{ 0.45f,0.f });
					m_player.startAccelerating();
					m_player.moveRight = true;
					//m_player.move({5.f,0.f});
				}
				break;
			case GLFW_KEY_SPACE:
				if(m_player.jump == true)
					m_player.doubleJump = true;
				else if(m_player.on_rope)
				{
				 	m_player.on_rope = false;
				 	m_player.jump = true;
				}
				else
					m_player.jump = true;
				break;
			case GLFW_KEY_C:
				m_player.attacking = true;
				break;
			// case GLFW_KEY_S: //just for testing
			// 	if(!save_level(level_index)){
			// 		fprintf(stderr, "Failed to load SAVE file!\n");
			// 	};
			// 	break;
			// case GLFW_KEY_L: //just for testing
			// 	if(!reload_level(level_index)){
			// 		fprintf(stderr, "Failed to load SAVE file!\n");
			// 	};
			// 	break;
			// case GLFW_KEY_F: //just for testing
			// 	m_player.fall = true;
			// 	break;
			// case GLFW_KEY_A: //just for testing, simulate the collision from above
			// 	m_player.collision_from_above = true;
			// 	break;
			// case GLFW_KEY_B: //just for testing, simulate the collision from bellow, eg. land on a platform
			// 	m_player.collision_from_below = true;
		}
	}
	if (action == GLFW_RELEASE) {
		switch (key) {
		case GLFW_KEY_UP: //leave for testing
			 m_player.moveUp = false;
			break;
		case GLFW_KEY_DOWN: //leave for testing
		 	m_player.moveDown = false;
			break;
		case GLFW_KEY_LEFT:
			m_player.moveLeft = false;
			m_player.stopAccelerating();
			break;
		case GLFW_KEY_RIGHT:
			m_player.moveRight = false;
			m_player.stopAccelerating();
			break;
		case GLFW_KEY_C:
			m_player.attacking = false;
			break;
		}
	}
}

void World::set_level(int level){
	level_index = level;
}
int World::get_level(){
	return level_index;
}

void World::load_level(){
	int w, h;
	reset_world();
	resume_level();
	glfwGetWindowSize(m_window, &w, &h);
	if(!display_level(levels[get_level()])){
		fprintf(stderr, "Failed to load level design file!\n");
	};
	initiate_game_entities();
	updateUI_key(false);
}

void World::updateUI_key(bool hasKey){
	i_key.update_key_status(hasKey);
}

void World::on_mouse_move(GLFWwindow* window, double xpos, double ypos) {
	m_mouse.set_position({(float)(xpos), (float)(ypos)});

	//swith hover status of icons
    for(auto& icon : m_icons){
        if(m_mouse.collides_with(icon))
        	icon.mouse_hover_on();
        else
        	icon.mouse_hover_off();
   	}
}

void World::on_mouse_click(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS){
		for (const auto& icon : m_icons) {
			if (!m_mouse.collides_with(icon))
				continue;
			else
				Mix_PlayChannel(-1, m_click_sound, 0);


        	//check the type of the btn
        	char type = icon.get_type();
        	switch(type){
        		case (char)'0': //level 0-4
        		case (char)'1':
        		case (char)'2':
        		case (char)'3':
        		case (char)'4':
        		case (char)'5':
        			set_level((int)type - 48);//-48 to conver to int;
        			if(has_saved_game((int)type - 48)){
						load_level();
        				set_GameMode("LOAD");
        			}else{
        				load_level();
        				if(get_level() == 0){
							set_GameMode("INFO");
						}else{
							set_GameMode("GAME");
						}
        			}
        			break;
        		case (char)'s': //start
        			set_GameMode("LEVELS");
        			break;
        		case (char)'q': //quit
        			set_GameMode("QUIT");
        			glfwSetWindowShouldClose(m_window, GL_TRUE);
					exit(0);
        			break;
        		case (char)'b':	//back
        			set_GameMode("MENU");
        			break;
        		case (char)'p': //pause
					pause_level();
        			set_GameMode("PAUSE");
        			break;
        		case (char)'i': //info
					pause_level();
        			set_GameMode("INFO");
        			break;
        		case (char)'c': //resume/continue
					resume_level();
        			set_GameMode("GAME");
        			break;
        		case (char)'m': //menu
        			set_GameMode("LEVELS");
        			break;
        		case (char)'o': //ok
					resume_level();
        			set_GameMode("GAME");
        			break;
        		case (char)'r': //restart
        			load_level();
        			set_GameMode("GAME");
        			break;
        		case (char)'y': //load game - yes
					reload_level(level_index);
        			set_GameMode("GAME");
        			break;
        		case (char)'n': //load game -no
        			load_level();
        			set_GameMode("GAME");
        			break;
        		case (char)'v': //save
					save_level(level_index);
        			set_GameMode("SAVE");
        			break;
        		}
        	turnoff_icons_hover();
        	updateUIcomponents();
        	break;
        }
    }


}

//check if this level has a saved game
bool World::has_saved_game(int levelIndex){
	//TODO
	std::string p(savePath);
	std::string fileName = p + std::to_string(get_level()) + ".txt";
	std::fstream fileStream;
  	fileStream.open(fileName);
	if (fileStream.fail()) {
		// file could not be opened
		return false;
	}
	fileStream.close();
	return true;
}

// check if health status is changed
void World::check_health_on_change(){
	int cur_health = m_player.get_health();
	if (cur_health != health){
		health_on_change();
		health = cur_health;
	}
}

// called when health status is chagned
void World::health_on_change(){
	updateUIcomponents();
}

// turn icons to hover_off status
void World::turnoff_icons_hover(){
	//reset icon hover status
	for(auto& i : m_icons){
		i.mouse_hover_off();
	}
}

//switch UI components according to gamemode
void World::updateUIcomponents() {
	if(check_GameMode("QUIT"))
		return;

	m_icons.clear();

	if(check_GameMode("GAME"))
	{
		//add pause, info, key(UI), goldcoin(UI) icons
		m_icons.emplace_back(i_pause);
		m_icons.emplace_back(i_info);
		m_icons.emplace_back(i_key);
		m_icons.emplace_back(i_goldcoin);

		// add score icons
		// use A - J (ASCII) to represent score 0 to 9.
		std::string points = std::to_string(m_points);
		for (int i = 0; i < points.size(); i++) {
			char c = points[i];
			int digit = (int)c - 48;
			m_score_icons[digit].set_digit_position(i);
			m_icons.emplace_back(m_score_icons[digit]);
		}

		// add health icons
		int health = m_player.get_health();
		m_icons.emplace_back(m_health_icons[health]);
	}

	else if(check_GameMode("MENU"))
	{
		m_bg = bg_main;
		m_icons.emplace_back(i_start);
		m_icons.emplace_back(i_quit);
	}

	else if(check_GameMode("LEVELS"))
	{
		m_bg = bg_main;
		m_icons.emplace_back(i_back);
		for(const auto& i : m_level_icons){
			m_icons.emplace_back(i);
		}
	}

	else if(check_GameMode("PAUSE"))
	{
		m_bg = bg_pause;
		m_icons.emplace_back(i_save);
		m_icons.emplace_back(i_resume);
		m_icons.emplace_back(i_restart);
		m_icons.emplace_back(i_menu);
	}

	else if(check_GameMode("INFO"))
	{
		m_bg = bg_instruction;
		m_icons.emplace_back(i_ok);
	}

	else if(check_GameMode("DEATH"))
	{
		m_bg = bg_death;
		m_icons.emplace_back(i_menu);
		m_icons.emplace_back(i_restart);
	}

	else if(check_GameMode("STAGECLEAR"))
	{
		m_bg = bg_stageClear;
		m_icons.emplace_back(i_menu);
	}

	else if(check_GameMode("LOAD"))
	{
		m_bg = bg_load;
		m_icons.emplace_back(i_yes);
		m_icons.emplace_back(i_no);
	}

	else if(check_GameMode("SAVE"))
	{
		m_bg = bg_save;
		m_icons.emplace_back(i_resume);
		m_icons.emplace_back(i_restart);
		m_icons.emplace_back(i_menu);
	}
}

bool World::check_GameMode(std::string gameMode){
	if(GAME_MODE.compare(gameMode) == 0)
		return true;
	return false;
}

void World::set_GameMode(std::string gameMode){
	GAME_MODE = gameMode;
}

void World::player_death() {
    m_player.destroy();
    m_player.init();
    m_enemy.clear();

    for (Archer ar: m_archers){
		ar.clean();
	}
	m_archers.clear();
    m_arrows.clear();
	m_ropes.clear();
    //m_points = 0.f;
    set_GameMode("DEATH");
    updateUIcomponents();
}

void World::reset_world() {
	gridBack->remove_all_for_every_tile();

	m_terrain.clear();
	m_key.clear();
	m_bush.clear();

	for (moving_enemy e: m_enemy){
		e.clean();
	}
	m_enemy.clear();

	for (Yeti y: m_yeti){
		y.clean();
	}
	m_yeti.clear();

	for (Archer ar: m_archers){
		ar.clean();
	}
	m_archers.clear();

	m_arrows.clear();
	//m_points = 0.f;
	m_player.destroy();
	m_door.destroy();
	m_player.init();
	m_coin.clear();
	m_blade_traps.clear();
	m_ladder.clear();
	m_ropes.clear();
	m_pitfall.clear();
	m_particle.clear();
	m_water.reset_player_dead_time();
	m_current_speed = 1.f;
}

void World::pause_level() {
	is_pause = true;
	// fprintf(stdout,"set PAUSE as %s\n", is_pause? "true":"false");
}

void World::resume_level() {
	is_pause = false;
	// fprintf(stdout,"set RESUME as %s\n", is_pause? "true":"false");
}

bool World::save_level(int level) {
	std::string fileName = savePath + std::to_string(level) + ".txt";
	std::ofstream f(fileName);

	if (f.is_open()){
		//for points
		f << "\n o ";
		f << std::to_string(m_points) + " ";

		//for player
		f << "\n p ";
		f << std::to_string(m_player.get_position().x) + " ";
		f << std::to_string(m_player.get_position().y) + " ";

		//for key
		f << ("\n k ");
		if (m_player.has_key()){
			f << std::to_string(1) + " ";
		} else {
			f << std::to_string(0) + " ";
			for (Key k: m_key){
				f << std::to_string(k.get_position().x) + " ";
				f << std::to_string(k.get_position().y) + " ";
			}
		}

		//for enemies
		f << ("\n e " + std::to_string(m_enemy.size())) + " ";
		for (moving_enemy e: m_enemy){
			f << std::to_string(e.get_position().x) + " ";
			f << std::to_string(e.get_position().y) + " ";
			f << std::to_string(e.get_countdown()) + " ";
			f << std::to_string(e.get_direction()) + " ";
			f << std::to_string(e.get_scale_x()) + " ";
			f << (e.is_alive()? std::to_string(1) : std::to_string(0));
			f << " ";
		}

		//for yeti
		f << ("\n y " + std::to_string(m_yeti.size())) + " ";
		for (Yeti y: m_yeti){
			f << std::to_string(y.get_position().x) + " ";
			f << std::to_string(y.get_position().y) + " ";
			f << std::to_string(y.get_countdown()) + " ";
			f << std::to_string(y.get_direction()) + " ";
			f << std::to_string(y.get_scale_x()) + " ";
			f << (y.is_alive()? std::to_string(1) : std::to_string(0));
			f << " ";
		}

		//for archers
		f << ("\n h " + std::to_string(m_archers.size())) + " ";
		for (Archer ar: m_archers){
			f << (ar.is_alive()? std::to_string(1) : std::to_string(0));
			f << " ";
			f << std::to_string(ar.get_position().x) + " ";
			f << std::to_string(ar.get_position().y) + " ";
		}

		//for coins
		f << "\n c ";
		f << std::to_string(m_coin.size()) + " ";
		for (Coin c: m_coin){
			f << std::to_string(c.get_position().x) + " ";
			f << std::to_string(c.get_position().y) + " ";
		}

		f.close();
		fprintf(stdout,"save the level\n");
		return true;
	}
	return false;
}

bool World::reload_level(int level) {
	resume_level();
	std::string fileName = savePath + std::to_string(level) + ".txt";
	std::ifstream f(fileName);
	std::stringstream strStream;
	strStream << f.rdbuf();
	std::string line = strStream.str();
	// fprintf(stdout,"the line is %s\n", line.c_str()); // test line

	if (f.is_open()){
		//process data to dataset
		std::vector<std::string> splitLine = split_line(line, ' ');
		int count = 0, i, elementMax, alive;
		float dir, countdown, scale_x;
		std::vector<vec2> coin_positions = gridBack->get_coin();

		while (count < splitLine.size())
    	{
			switch(splitLine[count].at(0)){

				case 'o': //points
					m_points = stoi(splitLine[++count]);
					break;

				case 'p': //player
					m_player.set_position({stof(splitLine[++count]), stof(splitLine[++count])});
					count++;
					break;

				case 'k': //key
					elementMax = stoi(splitLine[++count]);
					m_player.set_key_status(elementMax);
					updateUI_key(elementMax);
					if (elementMax == 1){ //has key
						m_key.clear();
					}
					count++;
					break;

				case 'e': //enemy
					for (auto& turtle : m_enemy){
						turtle.clean();
						turtle.destroy();
					}
					m_enemy.clear();
					elementMax = stoi(splitLine[++count]);
					for (i = 0; i<elementMax; i++){
						spawn_enemy({stof(splitLine[++count]), stof(splitLine[++count])});
						countdown = (stof(splitLine[++count]));
						dir = (stof(splitLine[++count]));
						scale_x = (stof(splitLine[++count]));
						m_enemy[m_enemy.size()-1].set_countdown(countdown);
						m_enemy[m_enemy.size()-1].set_direction(dir);
						m_enemy[m_enemy.size()-1].set_scale_x(scale_x);
						alive = stoi(splitLine[++count]);
						if (alive == 0 && m_enemy.size() >= 1)
							m_enemy[m_enemy.size()-1].kill();
					}
					count ++;
					break;

				case 'y': //enemy
					for (auto& yeti : m_yeti){
						yeti.clean();
						yeti.destroy();
					}
					m_yeti.clear();
					elementMax = stoi(splitLine[++count]);
					for (i = 0; i<elementMax; i++){
						spawn_yeti({stof(splitLine[++count]), stof(splitLine[++count])});
						countdown = (stof(splitLine[++count]));
						dir = (stof(splitLine[++count]));
						scale_x = (stof(splitLine[++count]));
						m_yeti[m_yeti.size()-1].set_countdown(countdown);
						m_yeti[m_yeti.size()-1].set_direction(dir);
						m_yeti[m_yeti.size()-1].set_scale_x(scale_x);
						alive = stoi(splitLine[++count]);
						if (alive == 0 && m_yeti.size() >= 1)
							m_yeti[m_yeti.size()-1].kill();
					}
					count ++;
					break;

				case 'h': //archers
					for (auto& archer : m_archers){
						archer.clean();
						archer.destroy();
					}
					m_archers.clear();
					elementMax = stoi(splitLine[++count]);
					for (i = 0; i<elementMax; i++){
						alive = stoi(splitLine[++count]);
						//fprintf(stdout,"we have a = %s, b = %s, alive = %d\n", splitLine[count+1].c_str(), splitLine[count+2].c_str(), alive);
						spawn_archer({stof(splitLine[++count]), stof(splitLine[++count])});
						if (alive == 0 && m_archers.size() >= 1)
							m_archers[m_archers.size()-1].kill();

					}
					count ++;
					break;

				case 'c': //coins
					for (auto& coin : m_coin)
						coin.destroy();
					m_coin.clear();
					elementMax = stoi(splitLine[++count]);
					for (i = 0; i<elementMax; i++){
						// TODO: finish data processing of coins
						// -----------------------------------------------------------
						// fprintf(stdout,"we have a = %s, b = %s\n", splitLine[count+1].c_str(), splitLine[count+2].c_str());
						// if (!spawn_coin()){
						// 	fprintf(stdout, "FAIL TO SPAWN COIN in reload_level()");
						// }
						// m_coin[m_coin.size()-1].set_position({stof(splitLine[++count]), stof(splitLine[++count])});
						// Coin& new_coin = m_coin.back();
						// new_coin.set_position({stof(splitLine[++count]), stof(splitLine[++count])});
						// fprintf(stdout, "m_coin size size is %lu\n", m_coin.size());
						// fprintf(stdout, "coin positions size is %lu\n", coin_positions.size());
						count += 2;
					}
					count ++;
					break;

				default:
					count ++;
					break;
			}
    	}
		splitLine.clear();
		coin_positions.clear();
    	f.close();

		//fprintf(stdout,"reload the level\n");
		return true;
	} else {
		return false;
	}

	//TODO: process data

}

// split a line of string by c
// used in reload_level
std::vector<std::string> World::split_line(std::string line, char c){
	std::vector<std::string> vec = {};
	int a, b; // temp variable
	float temp1, temp2;

	a = line.find(c);
	b = line.find(c,a+1);

	while (b < line.size()) {
		vec.push_back(line.substr(a+1,b-a));
		a = b;
		b = line.find(c,a+1);
	}
	return vec;
}

bool World::check_collision() {
	for (const auto& enemy : m_enemy)
	{
		if (enemy.is_alive() && m_player.collides_with(enemy))
		{
			if (m_player.take_damage()) {
				Mix_PlayChannel(-1, m_player_hurt, 0);
			}

			if (m_player.get_health() <= 0) {
				Mix_PlayChannel(-1, m_player_dead_sound, 0);
				player_death();
				return false;
			}
		}
	}

	// Check if enemy has been killed
	auto enemy_it = m_enemy.begin();
	while (enemy_it != m_enemy.end())
	{
		if (enemy_it->is_alive() && m_player.attacking && m_player.is_in_range(*enemy_it))
		{
			enemy_it->kill();
			continue;
		}
		++enemy_it;
	}

	for (const auto& yeti : m_yeti)
	{
		if (yeti.is_alive() && m_player.collides_with(yeti))
		{
			if (m_player.take_damage()) {
				Mix_PlayChannel(-1, m_player_hurt, 0);
			}

			if (m_player.get_health() <= 0) {
				Mix_PlayChannel(-1, m_player_dead_sound, 0);
				player_death();
				return false;
			}
		}
	}

	// Check if yeti has been killed
	auto yeti_it = m_yeti.begin();
	while (yeti_it != m_yeti.end())
	{
		if (yeti_it->is_alive() && m_player.attacking && m_player.is_in_range(*yeti_it))
		{
			if (yeti_it->take_damage()) {
				Mix_PlayChannel(-1, m_yeti_hurt, 0);
			}

			if (yeti_it->get_health() <= 0) {
				Mix_PlayChannel(-1, m_yeti_death, 0);
				vec2 yetiPosition = yeti_it->get_position();
				yeti_it->kill();
				// Spawn key at dead yeti
				if (!spawn_key())
					return false;
				Key& new_key = m_key.back();
				new_key.set_position(yetiPosition);

				continue;
			}
		}
		++yeti_it;
	}


	for (const auto& bladeTrap : m_blade_traps)
	{
		if (m_player.collides_with(bladeTrap))
		{
			if (m_player.take_damage()) {
				Mix_PlayChannel(-1, m_player_hurt, 0);
			}

			if (m_player.get_health() <= 0) {
				Mix_PlayChannel(-1, m_player_dead_sound, 0);
				player_death();
				return false;
			}
		}
	}

	// Check if archer has been killed
	auto archer_it = m_archers.begin();
	while (archer_it != m_archers.end())
	{
		if (archer_it->is_alive() && m_player.attacking && m_player.is_in_range(*archer_it))
		{
			archer_it->kill();
			continue;
		}
		++archer_it;
	}

	// Checking Player - Arrow collisions
	for (const auto& arrow : m_arrows)
	{
		if (m_player.collides_with(arrow))
		{
			if (m_player.take_damage()) {
				Mix_PlayChannel(-1, m_player_hurt, 0);
			}

			if (m_player.get_health() <= 0) {
				Mix_PlayChannel(-1, m_player_dead_sound, 0);
				player_death();
				return false;
			}
		}
	}

	// Checking Player - Coin collisions
	auto coin_it = m_coin.begin();
	while (coin_it != m_coin.end())
	{
		if (m_player.is_alive() && m_player.collides_with(*coin_it))
		{
			coin_it = m_coin.erase(coin_it);
			Mix_PlayChannel(-1, m_player_eat_sound, 0);
			++m_points;
			//update UI with new score
			updateUIcomponents();
		}
		else
			++coin_it;
	}
	// Checking Player - invisible wall collisions
	//TODO: need to change this so it is used for the invisible wall
	//TODO: should do it for the right and left but probably not the top and bottom.
	auto terrain_it = m_terrain.begin();
	while (terrain_it != m_terrain.end())
	{
		vec2 collision_direction = m_player.collides_with(*terrain_it);
		if (m_player.is_alive() && (abs(collision_direction.x) > 0 || abs(collision_direction.y) > 0)) {
			vec2 terrainPosition = terrain_it->get_position();
			vec2 terrainSize = terrain_it->get_bounding_box();
			vec2 playerPosition = m_player.get_position();
			vec2 playerSize = m_player.get_bounding_box();
			// Cancels movement in current direction, moves character slightly so no longer colliding
			if (collision_direction.x > 0) {
				float vel_x = std::max(0.f, m_player.get_velocity().x);
				m_player.set_velocity({ vel_x, });
				m_player.set_position({ terrainPosition.x + terrainSize.x + (playerSize.x * 0.5f), playerPosition.y });
			}
			else if (collision_direction.x < 0) {
				float vel_x = std::min(0.f, m_player.get_velocity().x);
				m_player.set_velocity({ vel_x, });
				m_player.set_position({ terrainPosition.x - terrainSize.x - (playerSize.x * 0.5f), playerPosition.y });
			}
			else if (collision_direction.y > 0) {
				m_player.collision_from_above = true;
				m_player.moveUp = false;
				m_player.set_position({ playerPosition.x, terrainPosition.y + terrainSize.y + playerSize.y });
			}
			else if (collision_direction.y < 0) {
				m_player.collision_from_below = true;
				m_player.moveDown = false;
				m_player.set_position({ playerPosition.x, terrainPosition.y - terrainSize.y - playerSize.y });
			}
			++terrain_it;
		} else {
			++terrain_it;
		}
	}

	// Checking Player - key collisions
	for (const auto& key : m_key)
	{
		if (m_player.collides_with(key) && m_player.is_alive())
		{
			Mix_PlayChannel(-1, m_player_get_key, 0);
			m_key.clear();
			m_player.set_key_status(true);
			updateUI_key(true);
		}
	}

	// Checking Player - Bush collisions
	for (const auto& bush : m_bush)
	{
		if (m_player.overlap_with(bush) && m_player.is_alive()) {
			m_player.set_is_hidden(true);
			break;
		}
		else {
			m_player.set_is_hidden(false);
		}
	}

	// Checking Player - Pitfall collisions
	for (const auto& pitfall : m_pitfall)
	{
		if (m_player.collides_with(pitfall))
		{
			if (m_player.take_damage()) {
				Mix_PlayChannel(-1, m_player_hurt, 0);
			}

			if (m_player.get_health() <= 0) {
				Mix_PlayChannel(-1, m_player_dead_sound, 0);
				player_death();
				return false;
			}
		}
	}

	// Checking player - ladder collisions
	for (const auto& ladder : m_ladder)
	{
		if (m_player.is_alive())
		{
			if (m_player.collides_with(ladder))
				m_player.on_ladder = true;
			else
				m_player.on_ladder = false;
		}
	}

	// Checking Player - Rope collisions
	for (auto& rope : m_ropes)
	{
		if (m_player.collides_with(rope) || m_player.on_rope)
		{
			m_player.on_rope = true;
			rope.set_initial_velocity(-m_player.get_velocity().x);
			rope.set_swinging(true);

			vec2 ropeSize = rope.get_bounding_box();
			if (rope.get_swingDistance() == 0){ // if this is the first collision with this rope
				rope.set_swingDistance(abs(m_player.get_position().y - rope.get_position().y));
			}
			vec2 newPosition = rope.calculate_current_position(rope.get_swingDistance());
			m_player.set_position(newPosition);
		}
		else
		{
			rope.set_swinging(false);
			rope.set_swingDistance(0);
			m_player.on_rope = false;
		}
		break;
	}

	//check if the player is landed on terrain
	bool on_land = false;
	m_player.move({ 0.f, 1.f });
	auto terrain_it0 = m_terrain.begin();
	while (terrain_it0 != m_terrain.end())
	{
		vec2 collision_direction = m_player.collides_with(*terrain_it0);
		if (m_player.is_alive() && (abs(collision_direction.x) > 0 || abs(collision_direction.y) > 0)) {
			if (collision_direction.y < 0)
				on_land = true;
		}
		++terrain_it0;
	}
	m_player.move({ 0.f,-1.f });

	if (on_land)
		m_player.on_land = true;
	else
		m_player.on_land = false;

	return true;
}

bool World::initiate_game_entities() {

	//Spawn coins
	std::vector<vec2> coin_positions = gridBack->get_coin();
	if(m_coin.size() < coin_positions.size()){
		for (int i = 0; i < coin_positions.size(); i++) {
			if (!spawn_coin())
				return false;
			Coin& new_coin = m_coin.back();
			new_coin.set_position(coin_positions[i]);
		}
	}
	coin_positions.clear();

	//Spawn terrains
	std::vector<vec2> terrains = gridBack->get_terrain();
	for(int i = 0; i < terrains.size(); i++){
		if(!spawn_terrain('t')){
			return false;
		}
		Terrain& new_terrain = m_terrain.back();
        new_terrain.set_position(terrains[i]);
	}
	terrains.clear();

	//Spawn terrains_black
	std::vector<vec2> terrains_black = gridBack->get_terrain_black();
	for(int i = 0; i < terrains_black.size(); i++){
		if(!spawn_terrain('T')){
			return false;
		}
		Terrain& new_terrain = m_terrain.back();
        new_terrain.set_position(terrains_black[i]);
	}
	terrains_black.clear();

	// //Spawn invisible
    // std::vector<vec2> invisibles = gridBack->get_invisible();
    // for(int i = 0; i < invisibles.size(); i++){
    //     if(!spawn_invisible()){
    //         return false;
    //     }
    //     Invisible_Wall& new_invisible_wall = m_invisible_wall.back();
    //     new_invisible_wall.set_position(invisibles[i]);
    // }
	// invisibles.clear();

	//Spawn pitfalls
	std::vector<vec2> pitfall = gridBack->get_pitfall();
	for(int i = 0; i < pitfall.size(); i++){
	    if(!spawn_pitfall()){
	        return false;
	    }
	    Pitfall& new_pitfall = m_pitfall.back();
	    new_pitfall.set_position(pitfall[i]);
	}
	pitfall.clear();

	//Spawn ladders
	std::vector<vec2> ladder = gridBack->get_ladder();
	for(int i = 0; i < ladder.size(); i++){
		if(!spawn_ladder()){
			return false;
		}
		Ladder& new_ladder = m_ladder.back();
		new_ladder.set_position(ladder[i]);
	}
	ladder.clear();

	// Spawn bush
	std::vector<vec2> bush = gridBack->get_bush();
	for (int i = 0; i < (bush.size()); i++) {
		if (!spawn_bush())
			return false;
		Bush& new_bush = m_bush.back();
		new_bush.set_position(bush[i]);
	}
	bush.clear();

  //Spawn doors
  	vec2 door = gridBack->get_door();
	Door new_door;
	new_door.init();
	new_door.set_position(door);
	m_door = new_door;

// Spawn pariticles
	for(int i = 0; i < MAX_PARTICLE; i++){
		if(!spawn_particle()){
			return false;
		}
		Particle& new_particle = m_particle.back();
		new_particle.set_position({(float)(rand() % 2300), (float)(rand() % 1400)});
	}

// Spawn blades
	std::vector<vec2> bladeTrap = gridBack->get_blade_trap();
	for (int i = 0; i < bladeTrap.size(); i++) {
		if (!spawn_blade_trap()) {
			return false;
		}
		BladeTrap&  new_bladeTrap = m_blade_traps.back();
		new_bladeTrap.set_position(bladeTrap[i]);
	}
	bladeTrap.clear();

	return true;
}

bool World::update_game_entities(float elapsed_ms) {

	m_player.update(elapsed_ms);

	// Moving Enemy update
	for (auto& turtle : m_enemy)
		turtle.update(elapsed_ms * m_current_speed, m_player.get_position(), m_player.is_hidden());

	auto turtle_it = m_enemy.begin();
	while (turtle_it != m_enemy.end())
	{
		float w = turtle_it->get_bounding_box().x / 2;
		if (turtle_it->get_position().x + w < 0.f)
		{
			turtle_it = m_enemy.erase(turtle_it);
			continue;
		}
		++turtle_it;
	}

	// Yeti update
	for (auto& yeti : m_yeti)
		yeti.update(elapsed_ms * m_current_speed, m_player.get_position(), m_player.is_hidden());

	auto yeti_it = m_yeti.begin();
	while (yeti_it != m_yeti.end())
	{
		float w = yeti_it->get_bounding_box().x / 2;
		if (yeti_it->get_position().x + w < 0.f)
		{
			yeti_it = m_yeti.erase(yeti_it);
			continue;
		}
		++yeti_it;
	}

	// Update archers, remove if off screen
	for (auto& archer : m_archers) {
		archer.update(elapsed_ms * m_current_speed, m_player.get_position(), m_player.is_hidden());

		if (archer.get_shoot_arrow() && archer.get_next_arrow_spawn() <= 0.f) {
			vec2 archerPosition = archer.get_position();
			if (!spawn_arrow())
				return false;
			archer.reset_next_arrow_spawn();
			Arrow& new_arrow = m_arrows.back();
			new_arrow.set_position({archerPosition.x, archerPosition.y - 20.f});
		}
	}

	auto archer_it = m_archers.begin();
	while (archer_it != m_archers.end())
	{
		float w = archer_it->get_bounding_box().x / 2;
		if (archer_it->get_position().x + w < 0.f)
		{
			archer_it = m_archers.erase(archer_it);
			continue;
		}

		++archer_it;
	}

	for (auto& blade_trap : m_blade_traps) {
		blade_trap.update(elapsed_ms);
	}
	auto blade_trap_it = m_blade_traps.begin();

	// Update arrows, remove if off screen
	for (auto& arrow : m_arrows)
		arrow.update(elapsed_ms * m_current_speed);

	auto arrow_it = m_arrows.begin();
	while (arrow_it != m_arrows.end())
	{
		float w = arrow_it->get_bounding_box().x / 2;
		if (arrow_it->get_position().x + w < 0.f)
		{
			arrow_it = m_arrows.erase(arrow_it);
			continue;
		}

		++arrow_it;
	}

	// Update rope
	for (auto& rope : m_ropes)
		rope.update(elapsed_ms * m_current_speed);

	return true;
}

bool World::spawn_updatable_entities() {
	// Spawn key
	std::vector<vec2> key = gridBack->get_key();
	if((m_key.size() < key.size()) && !m_player.has_key()) {
		for (int i = 0; i < key.size(); i++) {
			if (!spawn_key())
				return false;
			Key& new_key = m_key.back();
			new_key.set_position({key[i]});
		}
	}
	key.clear();

	//Spawn enemies
	std::vector<vec2> enemies = gridBack->get_enemy();
	if (m_enemy.size() < enemies.size()) {
		for (int i = 0; i < enemies.size(); i++) {
			if (!spawn_enemy(enemies[i]))
				return false;
			moving_enemy& new_enemy = m_enemy.back();
			new_enemy.set_position(enemies[i]);
		}
	}
	enemies.clear();

	//Spawn yeti
	std::vector<vec2> yetis = gridBack->get_yeti();
	if (m_yeti.size() < yetis.size()) {
		for (int i = 0; i < yetis.size(); i++) {
			if (!spawn_yeti(yetis[i]))
				return false;
			Yeti& new_yeti = m_yeti.back();
			new_yeti.set_position(yetis[i]);
		}
	}
	yetis.clear();

	//Spawn archers
	std::vector<vec2> archer_positions = gridBack->get_archer();
	if(m_archers.size() <= archer_positions.size()){
		for (int i = 0; i < (archer_positions.size()); i++) {
			if (!spawn_archer(archer_positions[i]))
				return false;
			Archer& new_archer = m_archers.back();
			new_archer.set_position(archer_positions[i]);
		}
	}
	archer_positions.clear();

	// Spawn rope
	std::vector<vec2> rope_positions = gridBack->get_rope();
	if(m_ropes.size() < rope_positions.size()){
		for (int i = 0; i < (rope_positions.size()); i++) {
			if (!spawn_rope())
				return false;
			Rope& new_rope = m_ropes.back();
			new_rope.set_position(rope_positions[i]);
		}
	}
	rope_positions.clear();
	return true;
}

bool World::do_particles() {
	//spawn particles
	particles_timer ++;
	if(particles_timer%20 == 0 && m_particle.size() < MAX_PARTICLE){
		if(!spawn_particle())
			return false;
		Particle& new_particle = m_particle.back();
	}

	//get screen size
	int w, h;
	glfwGetFramebufferSize(m_window, &w, &h);
	vec2 screen = { (float)w, (float)h };

	//update particles
	auto particle_it = m_particle.begin();
	while (particle_it != m_particle.end())
		{
		particle_it->update();
			if (particle_it->get_position().y > screen.y)
		{
			particle_it = m_particle.erase(particle_it);
			continue;
		}
			++particle_it;
		}
		return true;
}

void World::check_player_on_ground() {
	//check if the player is landed on terrain
	bool on_land = false;
	m_player.move({0.f, 1.f});
	auto terrain_it0 = m_terrain.begin();
	while (terrain_it0 != m_terrain.end())
	{
		vec2 collision_direction = m_player.collides_with(*terrain_it0);
		if (m_player.is_alive() && (abs(collision_direction.x) > 0 || abs(collision_direction.y) > 0)) {
		 if (collision_direction.y < 0)
		 		on_land = true;
		 }
		++terrain_it0;
	}
	m_player.move({0.f,-1.f});

	if(on_land)
		m_player.on_land = true;
	else
		m_player.on_land = false;
}

bool World::check_player_out_of_bound(float h) {
	if (m_player.is_alive() && m_player.get_position().y > h) {
		fprintf(stdout,"die because of falling \n");
		player_death();
		return false;
	} else {
			return true;
	}
}
