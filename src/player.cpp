#pragma

// Header
#include "player.hpp"
#include "grid_back.hpp"

// For using MI_PI
#define _USE_MATH_DEFINES

// internal
#include "archer.hpp"
#include "moving_enemy.hpp"
#include "coin.hpp"
#include "ladder.hpp"
#include "terrain.hpp"
#include "pitfall.hpp"
#include "door.hpp"
#include "key.hpp"
#include "bush.hpp"
#include "arrow.hpp"
#include "rope.hpp"
#include "bladeTrap.hpp"
#include "yeti.hpp"

// stlib
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
#include <math.h>
#include <iostream>


// define directions
#define RIGHT 1
#define LEFT 0
#define BOX_WIDTH 0.5f
#define ENEMY_WIDTH 0.4f
#define SLASH_WIDTH 0.7f
#define IMMUNE_TIME 1000.f
#define YETI_WIDTH 0.4f

Texture Player::char_texture_stand;
Texture Player::ad_run;
Texture Player::ad_stand;
Texture Player::ad_fall;
Texture Player::ad_climb;
Texture Player::ad_jump;
Texture Player::ad_smr;
Texture Player::ad_upslash;
Texture Player::ad_climb_stand;

#if _WIN32
const double SIZE_MODIFIER = 0.75;
#elif __APPLE__
const double SIZE_MODIFIER = 1.5;
#elif __linux__
// linux
#endif

bool Player::init()
{
	std::vector<uint16_t> indices;
	//adventurer-climb-stand
	if (!char_texture_stand.is_valid())
	{
		if (!ad_run.load_from_file(textures_path("adventurer-run.png")) ||
			(!ad_climb_stand.load_from_file(textures_path("adventurer-climb-stand.png"))) ||
			(!char_texture_stand.load_from_file(textures_path("adventurer-stand.png"))) ||
			(!ad_fall.load_from_file(textures_path("adventurer-fall.png"))) ||
			(!ad_climb.load_from_file(textures_path("adventurer-climb.png"))) ||
			(!ad_jump.load_from_file(textures_path("adventurer-jump-03.png"))) ||
			(!ad_smr.load_from_file(textures_path("adventurer-smr.png"))) ||
			(!ad_upslash.load_from_file(textures_path("adventurer-upslash.png"))) ||
			(!ad_stand.load_from_file(textures_path("adventurer-sword-idle.png"))))
		{
			fprintf(stderr, "Failed to load adventure texture!");
			return false;
		}
	}

	m_division = 1;
	// std::cout << "INIT: " << ad_run.id << std::endl;
	// std::cout << "INIT: " << ad_stand.id << std::endl;
	// std::cout << "INIT: " << ad_fall.id << std::endl;
	// std::cout << "INIT: " << ad_climb.id << std::endl;
	// std::cout << "INIT: " << ad_jump.id << std::endl;
	// std::cout << "INIT: " << ad_smr.id << std::endl;
	// std::cout << "INIT: " << ad_upslash.id << std::endl;

	// The position corresponds to the center of the texture

	float wr = char_texture_stand.width * SIZE_MODIFIER / m_division;
	float hr = char_texture_stand.height * SIZE_MODIFIER;

	m_size = {wr, hr};

 	TexturedVertex texturedVertices[4];
	texturedVertices[0].position = { -wr, +hr, -0.02f };
	texturedVertices[0].texcoord = { 0.f, 1.f };
	texturedVertices[1].position = { +wr, +hr, -0.02f };
	texturedVertices[1].texcoord = { 1.f, 1.f };
	texturedVertices[2].position = { +wr, -hr, -0.02f };
	texturedVertices[2].texcoord = { 1.f, 0.f };
	texturedVertices[3].position = { -wr, -hr, -0.02f };
	texturedVertices[3].texcoord = { 0.f, 0.f };

	// counterclockwise as it's the default opengl front winding direction
	indices = { 0, 3, 1, 1, 3, 2 };

	// Clearing errors
	gl_flush_errors();

	// Vertex Buffer creation
	glGenBuffers(1, &mesh.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TexturedVertex) * 4, texturedVertices, GL_STATIC_DRAW);

	// Index Buffer creation
	glGenBuffers(1, &mesh.ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * indices.size(), indices.data(), GL_STATIC_DRAW);

	// Vertex Array (Container for Vertex + Index buffer)
	glGenVertexArrays(1, &mesh.vao);
	if (gl_has_errors())
		return false;

	// Loading shaders
	if (!effect.load_from_file(shader_path("textured.vs.glsl"), shader_path("motion.fs.glsl")))
		return false;

	// Setting initial values
	moveUp = false;
	moveDown = false;
	moveLeft = false;
	moveRight = false;

	m_velocity = { 0.f, 0.f };
	m_acceleration = { 0.f, 0.f};
	m_face = RIGHT;

	m_is_accelerating = false;
	m_scale.x = 1.f;
	m_scale.y = 1.f;
	m_is_alive = true;
	m_num_indices = indices.size();
	m_position = {140.0,100.0};
	m_rotation = 0.f;
	m_light_up_countdown_ms = -1.f;
	m_has_key = false;
	m_is_hidden = false;
	timer = 0;
	is_valid = true;
	on_rope = false;
	m_overlapping_cells = {};
	m_health = 3;
	m_immunity_timer = IMMUNE_TIME;
	m_immune = false;

	jump = false;

	jump_down = false;

	doubleJump = false;

	fall = false;

	on_land = false;

	on_ladder = false;

	disableMomentum = false;

	collision_from_above = false;

	collision_from_left = false;

	collision_from_right = false;

	collision_from_below = false;

	swingRight = false;

	swingLeft = false;

	attacking = false;



	return true;
}

Player::~Player(){
	// m_overlapping_cells.clear();
}

// Releases all graphics resources
void Player::destroy()
{
	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &mesh.ibo);
	glDeleteBuffers(1, &mesh.vao);

	// glDeleteShader(effect.vertex);
	// glDeleteShader(effect.fragment);
	// glDeleteShader(effect.program);

	effect.release(); // according to https://piazza.com/class/jp1oa2b3dx04b4?cid=257
}


float v;

// Called on each frame by World::update()
void Player::update(float ms)
{
	if (m_immune) {
		m_immunity_timer -= ms;
	}
	if (m_immunity_timer <= 0.f) {
		m_immune = false;
		m_immunity_timer = IMMUNE_TIME;
	}

	/* Movement along x axis*/
	const float SPEED = 200.f;
	float step = SPEED * (ms / 1000);
	float distance = m_velocity.x * step;

	if(m_is_accelerating)
		addVelocity(m_acceleration);

	if (m_is_alive){
		//prevent useless collision
		if(collision_from_below && timer == 0){
			collision_from_below = false;
			collision_from_above = false;
		}

		//it't not clear that why sometimes collision_from_above happens when the player falls
		//but this seems to temperarly fix the bug.
		if(collision_from_above && (fall || timer == 0)){
			collision_from_above = false;
		}

		//trigger fall
		if (!on_land && !jump && !on_ladder && !doubleJump)
		    fall = true;

		//revert jump_down when jump motion is finished
		if (!jump)
			jump_down = false;

		if (!disableMomentum)
	    	move(vec2{distance, 0 });

	}

	reduceSpeed(ms);

	if (m_is_alive)
	{

		// NOTE: use multiplication below instead of setting up the value directly
		// the m_scale can remain at the same value by multiplication.
		if(moveUp || moveDown){
			if (moveUp)
				move({0.f,-step});
			else
				move({0.f,step});
			//char_texture_stand.load_from_file(textures_path("adventurer-climb.png"));
			char_texture_stand.id  = ad_climb.id;
			m_division = 5;
			if (m_scale.y < 0)
				m_scale.y *= -1.f;
		}

		if(moveLeft && !on_ladder){
			//char_texture_stand.load_from_file(textures_path("adventurer-run.png"));
			char_texture_stand.id = ad_run.id;
			m_division = 6;
			if (m_scale.x > 0)
				m_scale.x *= -1.f;
		} else if (moveLeft){
			if (m_scale.x > 0)
				m_scale.x *= -1.f;
		} else if(moveRight && !on_ladder){
			//char_texture_stand.load_from_file(textures_path("adventurer-run.png"));
			char_texture_stand.id = ad_run.id;
			m_division = 6;
			if (m_scale.x < 0)
				m_scale.x *= -1.f;
		} else if (moveRight) {
			if (m_scale.x < 0)
				m_scale.x *= -1.f;
		}else if(on_ladder && !moveLeft && !moveRight && !moveUp && !moveDown){
			char_texture_stand.id  = ad_climb_stand.id;
			m_division = 1;
		}

		if (jump || jump_down) {
			jump_();
			if (jump_down) {
				//char_texture_stand.load_from_file(textures_path("adventurer-fall.png"));
				char_texture_stand.id = ad_fall.id;
				m_division = 2;
			} else {
				//char_texture_stand.load_from_file(textures_path("adventurer-jump-03.png"));
				char_texture_stand.id = ad_jump.id;
				m_division = 1;
			}
		}

		if (fall) {
			fall_();
			//char_texture_stand.load_from_file(textures_path("adventurer-fall.png"));
			char_texture_stand.id = ad_fall.id;
			m_division = 2;

		}

		if (doubleJump) {
			doubleJump_();
			//char_texture_stand.load_from_file(textures_path("adventurer-smr.png"));
			char_texture_stand.id = ad_smr.id;
			m_division = 4;
		}

		// TODO: fix a bug in the condition
		if (not_moving()){
			//char_texture_stand.load_from_file(textures_path("adventurer-stand.png"));
			char_texture_stand.id = ad_stand.id;

			m_division = 4;
		}

		if (attacking) {
			//char_texture_stand.load_from_file(textures_path("adventurer-upslash.png"));
			char_texture_stand.id = ad_upslash.id;
			m_division = 5;
		}
	}

	if (!m_is_alive)
		{
			// If dead we make it face upwards and sink deep down
			set_rotation(3.1415f);
			move({ 0.f, 200 });
		}

	if (m_light_up_countdown_ms > 0.f)
		m_light_up_countdown_ms -= ms;
}

bool Player::not_moving(){
	return (get_velocity().x == 0 && get_velocity().y == 0) &&	//position stay still
	 !(moveUp || moveRight || moveLeft || moveDown) &&  // not in any motions
	 !fall && !doubleJump && !jump && !on_ladder;
}

void Player::fall_(){
	float windowBotton = 2000.0; //hardcoded, the lowest point it can fall to

    if(timer == 0)
		v = 0.f;

	if(jump){
		fall = false;
		timer = 0;
		return;
	}

	//check collision status
	if(collision_from_below || m_position.y >= windowBotton){
		timer = 0;
		fall = false;
	    collision_from_below = false;
	    return;
	}

	v -= jumpTimeStep * gravity;
	float jumpStep = jumpTimeStep * v;
	move({0.f,-jumpStep});
	timer++;
}

void Player::doubleJump_(){
	if(is_valid){
		timer = 0;
		is_valid = false;
	}
}
void Player::jump_(){
	if (timer == 0)
		v = jump_initialSpeed;

	//discard unless collisions
	if(v > 0.f && collision_from_below){
		collision_from_below =false;
	}else if(v < 0.f && collision_from_above){
		collision_from_above =false;
	}

	//check collision status
	if(collision_from_above){
		timer = 0;
		v = - v;
		collision_from_above = false;
	}
	if(collision_from_below){
		jump = false;
		doubleJump = false;
		is_valid = true;
		timer = 0.f;
	    collision_from_below = false;
	    return;
	}

	v -= jumpTimeStep * gravity;
	float jumpStep = jumpTimeStep * v;
	move({0.f,-jumpStep});
	timer++;

	// fprintf(stdout,"current v in jump funtion is %f\n", v);
	if (v<0) {
		jump_down = true;
	} else {
		jump_down = false;
	}
}

//break from all actions in progress
void Player::stopActions(){
	moveUp = true;
	fall = false;
	jump = false;
	jump_down = false;
	timer = 0;
	doubleJump = false;
	disableMomentum = true;
}
void Player::draw(const mat3& projection)
{
	transform_begin();
	transform_translate({m_position});
	transform_rotate(m_rotation);
	transform_scale(m_scale);
	transform_end();

	// Setting shaders
	glUseProgram(effect.program);

	// Enabling alpha channel for textures
	glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);

	// Getting uniform locations
	GLint transform_uloc = glGetUniformLocation(effect.program, "transform");
	GLint color_uloc = glGetUniformLocation(effect.program, "fcolor");
	GLint projection_uloc = glGetUniformLocation(effect.program, "projection");
	GLint light_up_uloc = glGetUniformLocation(effect.program, "light_up");

	// Setting vertices and indices
	glBindVertexArray(mesh.vao);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);

	// Input data location as in the vertex buffer
	GLint in_position_loc = glGetAttribLocation(effect.program, "in_position");
	GLint in_texcoord_loc = glGetAttribLocation(effect.program, "in_texcoord");
	GLuint division_uloc = glGetUniformLocation(effect.program, "division");
	GLuint time_uloc = glGetUniformLocation(effect.program, "time");
	GLuint is_immune_uloc = glGetUniformLocation(effect.program, "is_immune");
	glEnableVertexAttribArray(in_position_loc);
	glEnableVertexAttribArray(in_texcoord_loc);
	glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)0);
	glVertexAttribPointer(in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)sizeof(vec3));
	glUniform1f(time_uloc, (float)(glfwGetTime() * 10.0f));
	glUniform1f(division_uloc, (float)(m_division * 1.0f));
	glUniform1i(is_immune_uloc, (int)(m_immune));

	// Enabling and binding texture to slot 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, char_texture_stand.id);

	// Setting uniform values to the currently bound program
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform);

	// !!! Player Color
	float color[] = { 1.f, 1.f, 1.f };

	if(m_is_alive == false){
		color[1] = 0.f;
		color[2] = 0.f;
	}
	glUniform3fv(color_uloc, 1, color);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);

	int light_up = 0;
	if (m_light_up_countdown_ms>0)
	{
		light_up = 1;
	}

	glUniform1iv(light_up_uloc, 1, &light_up);


	// Drawing!
	glDrawElements(GL_TRIANGLES,(GLsizei)m_num_indices, GL_UNSIGNED_SHORT, nullptr);
}

// Simple bounding box collision check
bool Player::collides_with(const moving_enemy& enemy)
{
	if (m_is_hidden)
		return false;
	vec2 enemyPosition = enemy.get_position();
	vec2 enemySize = {enemy.get_bounding_box().x * 0.5f, enemy.get_bounding_box().y *0.5f};
	vec2 playerSize = get_bounding_box();
	bool collisionX = m_position.x + (playerSize.x * ENEMY_WIDTH) <= enemyPosition.x - enemySize.x
										|| m_position.x - (playerSize.x * ENEMY_WIDTH) >= enemyPosition.x + enemySize.x;
	bool collisionY = m_position.y + playerSize.y <= enemyPosition.y
										|| m_position.y - playerSize.y >= enemyPosition.y + enemySize.y;
	if (!collisionX && !collisionY) {
		return true;
	}
	return false;
}

// Simple bounding box collision check
bool Player::collides_with(const Yeti& yeti)
{
	if (m_is_hidden)
		return false;
	vec2 yetiPosition = yeti.get_position();
	vec2 yetiSize = {yeti.get_bounding_box().x * 0.5f, yeti.get_bounding_box().y *0.5f};
	vec2 yetiScale = yeti.get_scale();
	vec2 playerSize = get_bounding_box();

	// If yeti is chasing increase collision range in direction it is attacking
	bool collisionX = false;
	if (yeti.is_chasing() && !yeti.is_immune()) {
		if (yetiScale.x > 0) {
			collisionX = m_position.x + (playerSize.x * ENEMY_WIDTH) <= yetiPosition.x - yetiSize.x * YETI_WIDTH * 2.f
				|| m_position.x - (playerSize.x * ENEMY_WIDTH) >= yetiPosition.x + yetiSize.x * YETI_WIDTH;
		} else {
		collisionX = m_position.x + (playerSize.x * ENEMY_WIDTH) <= yetiPosition.x - yetiSize.x * YETI_WIDTH
			|| m_position.x - (playerSize.x * ENEMY_WIDTH) >= yetiPosition.x + yetiSize.x * YETI_WIDTH * 2.f;
		}
	} else {
		collisionX = m_position.x + (playerSize.x * ENEMY_WIDTH) <= yetiPosition.x - yetiSize.x * YETI_WIDTH
			|| m_position.x - (playerSize.x * ENEMY_WIDTH) >= yetiPosition.x + yetiSize.x * YETI_WIDTH;
	}
	bool collisionY = m_position.y + playerSize.y <= yetiPosition.y
		|| m_position.y - playerSize.y >= yetiPosition.y + yetiSize.y;
	if (!collisionX && !collisionY) {
		return true;
	}
	return false;
}

// Simple bounding box collision to check if enemy is in attack range
bool Player::is_in_range(const moving_enemy& enemy)
{
	vec2 enemyPosition = enemy.get_position();
	vec2 enemySize = enemy.get_bounding_box();
	vec2 playerSize = get_bounding_box();
	bool collisionX = false;
	if (m_scale.x >= 0) {
		collisionX = m_position.x + (playerSize.x * SLASH_WIDTH) >= enemyPosition.x - enemySize.x
			&& m_position.x + (playerSize.x * ENEMY_WIDTH) <= enemyPosition.x - enemySize.x;
	} else {
		collisionX = m_position.x - (playerSize.x * SLASH_WIDTH) <= enemyPosition.x + enemySize.x
			&& m_position.x - (playerSize.x * ENEMY_WIDTH) >= enemyPosition.x + enemySize.x;
	}
	bool collisionY = m_position.y + playerSize.y <= enemyPosition.y
		|| m_position.y - playerSize.y >= enemyPosition.y + enemySize.y;
	if (collisionX && !collisionY) {
		return true;
	}
	return false;
}

// Simple bounding box collision to check if enemy is in attack range
bool Player::is_in_range(const Archer& archer)
{
	vec2 archerPosition = archer.get_position();
	vec2 archerSize = archer.get_bounding_box();
	vec2 playerSize = get_bounding_box();
	bool collisionX = false;
	if (m_scale.x >= 0) {
		collisionX = m_position.x + (playerSize.x * SLASH_WIDTH) >= archerPosition.x - archerSize.x
			&& m_position.x + (playerSize.x * ENEMY_WIDTH) < archerPosition.x + archerSize.x;
	} else {
		collisionX = m_position.x - (playerSize.x * SLASH_WIDTH) <= archerPosition.x + archerSize.x
			&& m_position.x - (playerSize.x * ENEMY_WIDTH) > archerPosition.x - archerSize.x;
	}
	bool collisionY = m_position.y + playerSize.y <= archerPosition.y
		|| m_position.y - playerSize.y >= archerPosition.y + archerSize.y;
	if (collisionX && !collisionY) {
		return true;
	}
	return false;
}

// Simple bounding box collision to check if yeti is in attack range
bool Player::is_in_range(const Yeti& yeti)
{
	vec2 yetiPosition = yeti.get_position();
	vec2 yetiSize = yeti.get_bounding_box();
	vec2 playerSize = get_bounding_box();
	bool collisionX = false;
	if (m_scale.x >= 0) {
		collisionX = m_position.x + (playerSize.x * SLASH_WIDTH) >= yetiPosition.x - yetiSize.x * YETI_WIDTH
			&& m_position.x + (playerSize.x * ENEMY_WIDTH) <= yetiPosition.x - yetiSize.x * YETI_WIDTH;
	} else {
		collisionX = m_position.x - (playerSize.x * SLASH_WIDTH) <= yetiPosition.x + yetiSize.x * YETI_WIDTH
			&& m_position.x - (playerSize.x * ENEMY_WIDTH) >= yetiPosition.x + yetiSize.x * YETI_WIDTH;
	}
	bool collisionY = m_position.y + playerSize.y <= yetiPosition.y
		|| m_position.y - playerSize.y >= yetiPosition.y + yetiSize.y;
	if (collisionX && !collisionY) {
		return true;
	}
	return false;
}

bool Player::collides_with(const Coin& coin)
{
	float dx = m_position.x - coin.get_position().x;
	float dy = m_position.y - coin.get_position().y;
	float d_sq = dx * dx + dy * dy;
	float other_r = std::max(coin.get_bounding_box().x, coin.get_bounding_box().y);
	float my_r = std::max(m_scale.x, m_scale.y);
	float r = std::max(other_r, my_r);
	r *= 0.6f;
	if (d_sq < r * r)
		return true;
	return false;
}

//cehck if player is within ladder
bool Player::collides_with(const Ladder& ladder)
{

	float l_upperbound = ladder.get_position().y - ladder.get_bounding_box().y * 0.5f;
	float l_lowerbound = ladder.get_position().y + ladder.get_bounding_box().y * 0.5f;

	//if the player climbs up to the top of the ladder, stop it from continue climbing
	if( moveUp
		&& l_upperbound > m_position.y
		&& l_upperbound - m_position.y < 5.f
		&& abs(ladder.get_position().x - m_position.x) <= ladder.get_bounding_box().x * 0.25f){
		set_position({m_position.x, l_upperbound});
	}

	//player is on the ladder
	if(abs(ladder.get_position().x - m_position.x) <= ladder.get_bounding_box().x
		&& abs(ladder.get_position().y - m_position.y) <= ladder.get_bounding_box().y/2.f + 10.f){
		m_ladder_position = ladder.get_position();
		return true;
	}

	//player is at the very top part of the ladder, include a wider x range because the player need to walk onto platform
	else if(abs(ladder.get_position().x - (m_position.x+5.f)) <= ladder.get_bounding_box().x * 0.5f
			&& abs(l_upperbound - m_position.y) <= 10.f){
		return true;
	}

	return false;
}

//place player on the ladder
void Player::stickonLadder(){
	set_position({m_ladder_position.x, m_position.y});
}


// Simple bounding box collision check with pitall
bool Player::collides_with(const Pitfall& pitfall)
{
	float dx = m_position.x - pitfall.get_position().x;
	float dy = m_position.y - pitfall.get_position().y;
	float d_sq = dx * dx + dy * dy;
	float other_r = std::max(pitfall.get_bounding_box().x, pitfall.get_bounding_box().y);
	float my_r = std::max(m_scale.x, m_scale.y);
	float r = std::max(other_r, my_r);
	r *= 0.6f;
	if (d_sq < r * r)
		return true;
	return false;
}

vec2 Player::collides_with(const Terrain& terrain)
{

	float dx = m_position.x - terrain.get_position().x;
	float dy = m_position.y - terrain.get_position().y;
	vec2 terrainPosition = terrain.get_position();
	vec2 terrainSize = terrain.get_bounding_box();

	//detect wall before the actual collision happens. looking for a better solution
	float detect_range = 10.f;
	bool collisionX = m_position.x + (get_bounding_box().x * BOX_WIDTH + detect_range) <= terrainPosition.x - terrainSize.x
										|| m_position.x - (get_bounding_box().x * BOX_WIDTH + detect_range) >= terrainPosition.x + terrainSize.x;
	bool collisionY = m_position.y + get_bounding_box().y <= terrainPosition.y - terrainSize.y
										|| m_position.y - get_bounding_box().y >= terrainPosition.y + terrainSize.y;
	if (!collisionX && !collisionY && abs(dx) > abs(dy)){
		if (dx > 0) {
			set_position({ terrainPosition.x + terrainSize.x + (get_bounding_box().x * BOX_WIDTH + detect_range), m_position.y });
		} else if (dx < 0) {
			set_position({ terrainPosition.x - terrainSize.x - (get_bounding_box().x * BOX_WIDTH + detect_range), m_position.y });
		}
	}

	collisionX = m_position.x + (get_bounding_box().x * BOX_WIDTH) <= terrainPosition.x - terrainSize.x
										|| m_position.x - (get_bounding_box().x * BOX_WIDTH) >= terrainPosition.x + terrainSize.x;

	if (!collisionX && !collisionY)
	{
		if (abs(dx) > abs(dy)) {
			return {dx/abs(dx), 0};
		} else {
			return {0, dy/abs(dy)};
		}
	}
	return {0,0};
}

bool Player::collides_with(const Key& key)
{
	float dx = m_position.x - key.get_position().x;
	float dy = m_position.y - key.get_position().y;
	float d_sq = dx * dx + dy * dy;
	float other_r = std::max(key.get_bounding_box().x, key.get_bounding_box().y);
	float my_r = std::max(m_scale.x, m_scale.y);
	float r = std::max(other_r, my_r);
	r *= 0.6f;
	if (d_sq < r * r)
		return true;
	return false;
}

bool Player::collides_with(const Arrow& arrow)
{
	vec2 arrowPosition = arrow.get_position();
	vec2 arrowSize = arrow.get_bounding_box();
	vec2 playerSize = get_bounding_box();

	bool collisionX = m_position.x + (playerSize.x * BOX_WIDTH) <= arrowPosition.x - arrowSize.x
										|| m_position.x - (playerSize.x * BOX_WIDTH) >= arrowPosition.x + arrowSize.x;
	bool collisionY = m_position.y + playerSize.y <= arrowPosition.y - arrowSize.y
										|| m_position.y - playerSize.y >= arrowPosition.y + arrowSize.y;
	if (!collisionX && !collisionY)
		return true;
	return false;
}

bool Player::collides_with(const Rope& rope)
{
	vec2 ropePosition = rope.get_position();
	vec2 ropeSize = rope.get_bounding_box();
	vec2 playerSize = get_bounding_box();

	bool collisionX = m_position.x + (playerSize.x * BOX_WIDTH) <= ropePosition.x - ropeSize.x
										|| m_position.x - (playerSize.x * BOX_WIDTH) >= ropePosition.x + ropeSize.x;
	bool collisionY = m_position.y + playerSize.y <= ropePosition.y
										|| m_position.y - playerSize.y >= ropePosition.y + ropeSize.y;
	if (!collisionX && !collisionY) {
		jump = false;
		timer = 0;
		doubleJump = false;
		fall = false;
		return true;
	}
	return false;
}

bool Player::overlap_with(const Bush& bush)
{
	vec2 terrainPosition = bush.get_position();
	vec2 terrainSize = bush.get_bounding_box();
	vec2 playerSize = get_bounding_box();

	bool insideX = m_position.x + playerSize.x <= terrainPosition.x + terrainSize.x
										&& m_position.x - playerSize.x >= terrainPosition.x - terrainSize.x;
	bool insideY = m_position.y + playerSize.y <= terrainPosition.y + terrainSize.y
										|| m_position.y - playerSize.y >= terrainPosition.y - terrainSize.y;
	if (insideX && insideY)
		return true;
	return false;
}

bool Player::overlap_with(const Door& door)
{
	float BOX_WIDTH_RATIO = 0.2f;
	float dx = m_position.x - door.get_position().x;
	float dy = m_position.y - door.get_position().y;
	float d_sq = dx * dx + dy * dy;
	float other_r = std::max(door.get_bounding_box().x * BOX_WIDTH_RATIO, door.get_bounding_box().y * BOX_WIDTH_RATIO);
	float my_r = std::max(m_scale.x, m_scale.y);
	float r = std::max(other_r, my_r);
	r *= 1.1f;
	if (d_sq < r * r)
		return true;
	return false;
}

// Check collsion between player and blade trap, use circle bounding volumn
bool Player::collides_with(const BladeTrap& bladeTrap)
{
	float circleDistance_x = abs(bladeTrap.get_position().x - m_position.x);
	float circleDistance_y = abs(bladeTrap.get_position().y - m_position.y);

	if (circleDistance_x > (get_bounding_box().x + bladeTrap.get_radius())) { return false; }
	if (circleDistance_y > (get_bounding_box().y + bladeTrap.get_radius())) { return false; }

	if (circleDistance_x <= (get_bounding_box().x)) { return true; }
	if (circleDistance_y <= (get_bounding_box().y)) { return true; }

	// distance between the center of circle and the nearest point to the circle on the bounding box
	float cornerDistance_sq = pow(circleDistance_x - get_bounding_box().x, 2.0) +
		pow((circleDistance_y - get_bounding_box().y), 2);

	return (cornerDistance_sq <= pow(bladeTrap.get_radius(), 2));
}
void Player::move(vec2 off)
{
	m_position.x += off.x; m_position.y += off.y;
}

void Player::set_rotation(float radians)
{
	m_rotation = radians;
}

bool Player::is_alive()const
{
	return m_is_alive;
}

// Called when the salmon collides with a turtle
void Player::kill()
{
	m_is_alive = false;
	m_has_key = false;
}

// function not in use
void Player::light_up()
{
	m_light_up_countdown_ms = 1500.f;
}

void Player::addVelocity(vec2 offset) {
	if (m_velocity.x + offset.x > MAX_VELOCITY)
		m_velocity.x = MAX_VELOCITY;
	else if (m_velocity.x + offset.x < -MAX_VELOCITY)
		m_velocity.x = -MAX_VELOCITY;
	else
		m_velocity.x += offset.x;
}

void Player::addAcceleration(vec2 offset) {
	if (m_acceleration.x + offset.x > MAX_ACCELERATION)
		m_acceleration.x = MAX_ACCELERATION;
	else if (m_acceleration.x + offset.x < -MAX_ACCELERATION)
		m_acceleration.x = -MAX_ACCELERATION;
	else
		m_acceleration.x += offset.x;
}

void Player::reduceSpeed(float ms) {
	if (m_velocity.x > 0.f) {
		if (m_velocity.x - speedReductionRate * ms / 1000 >= 0) {
			m_velocity.x -= speedReductionRate * ms / 1000;
		}
		else
			m_velocity.x = 0;
	}

	else if (m_velocity.x < 0.f) {
		if (m_velocity.x + speedReductionRate * ms / 1000 <= 0) {
			m_velocity.x += speedReductionRate * ms / 1000;
		}
		else
			m_velocity.x = 0;
	}
}

void Player::startAccelerating()
{
	m_is_accelerating = true;
}

void Player::stopAccelerating()
{
	m_is_accelerating = false;
	m_acceleration.x = 0;
	m_acceleration.y = 0;
}

vec2 Player::get_velocity() {
	return m_velocity;
}

void Player::set_velocity(vec2 vel) {
	if (!std::isnan(vel.x)) {
		m_velocity.x = vel.x;
	}
	if (!std::isnan(vel.y)) {
		m_velocity.y = vel.y;
	}
}

vec2 Player::get_position() {
    return m_position;
}

void Player::set_position(vec2 position) {
    m_position = position;
}

bool Player::has_key()const
{
	return m_has_key;
}

void Player::set_key_status(bool status) {
	m_has_key = status;
}

vec2 Player::get_bounding_box()const
{
	// fabs is to avoid negative scale due to the facing direction
	return { std::fabs(m_scale.x) * m_size.x, std::fabs(m_scale.y) * m_size.y };
}

void Player::set_is_hidden(bool hidden) {
	m_is_hidden = hidden;
}

bool Player::is_hidden()const
{
	return m_is_hidden;
}

bool Player::take_damage() {
	if (!m_immune) {
		m_health -= 1;
		m_immune = true;
		// For different behaviour when dying
		if (m_health <= 0)
			return false;
		return true;
	}
	return false;
}

int Player::get_health() {
	return m_health;
}

std::vector<vec2> Player::update_overlapping_cells(float tileSize, int width, int height) {
	m_overlapping_cells.clear(); vec2 center = { static_cast<float>(ceil(m_position.x / tileSize)), static_cast<float>(ceil(m_position.y / tileSize)) };
	m_overlapping_cells.push_back(center);
	m_overlapping_cells.push_back({ center.x + 1, center.y });
	m_overlapping_cells.push_back({ center.x - 1, center.y });
	m_overlapping_cells.push_back({ center.x + 1, center.y });
	m_overlapping_cells.push_back({ center.x,     center.y + 1 });
	m_overlapping_cells.push_back({ center.x,	  center.y - 1});
	m_overlapping_cells.push_back({ center.x + 1, center.y + 1});
	m_overlapping_cells.push_back({ center.x + 1, center.y - 1});
	m_overlapping_cells.push_back({ center.x - 1, center.y + 1});
	m_overlapping_cells.push_back({ center.x - 1, center.y - 1});
	return m_overlapping_cells;
}

std::vector<vec2> Player::get_overlapping_cells() {
	return m_overlapping_cells;
}
