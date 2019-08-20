#pragma once

#include "common.hpp"
#include <vector>
class moving_enemy;
class Archer;
class Coin;
class Ladder;
class Terrain;
class Pitfall;
class Door;
class Key;
class Bush;
class Arrow;
class Rope;
class BladeTrap;
class Yeti;

class Player : public Renderable
{
public:
	static Texture char_texture_stand;
	static Texture char_texture_run;
	static Texture ad_run;
	static Texture ad_climb;
	static Texture ad_fall;
	static Texture ad_smr;
	static Texture ad_stand;
	static Texture ad_upslash;
	static Texture ad_jump;
	static Texture ad_climb_stand;

	~Player();

	void addVelocity(vec2 offset);

	void addAcceleration(vec2 offset);

	// Creates all the associated render resources and default transform
	bool init();

	// Releases all associated resources
	void destroy();

	// Update salmon position based on direction
	// ms represents the number of milliseconds elapsed from the previous update() call
	void update(float ms);

	// Renders the salmon
	void draw(const mat3& projection)override;

	// Collision routines for turtles, terrain, coin and pitfalls
	bool collides_with(const moving_enemy& turtle);
	bool collides_with(const Coin& coin);
	bool collides_with(const Pitfall& pitfall);
	vec2 collides_with(const Terrain& terrain);
	bool collides_with(const Ladder& ladder);
	bool collides_with(const Key& key);
	bool collides_with(const Arrow& arrow);
	bool collides_with(const Rope& rope);
	bool collides_with(const BladeTrap& bladeTrap);
	bool overlap_with(const Door& door);
	bool overlap_with(const Bush& bush);
	bool collides_with(const Yeti& yeti);

	bool is_in_range(const moving_enemy& enemy);
	bool is_in_range(const Archer& archer);
	bool is_in_range(const Yeti& yeti);

	// Moves the salmon's position by the specified offset
	void move(vec2 off);

	// gets the salmons position
	vec2 get_position();
	// Set salmon rotation in radians
	void set_rotation(float radians);

	// True if the salmon is alive
	bool is_alive()const;
	bool take_damage();
	int get_health();

	// Kills the salmon, changing its alive state and triggering on death events
	void kill();

	// starts lighting up the salmon
	void light_up();

	void startAccelerating();

	void stopAccelerating();

	void reduceSpeed(float deltaTime);

	void set_velocity(vec2 vel);

	vec2 get_velocity();

	std::vector<vec2> update_overlapping_cells(float, int ,int);
	std::vector<vec2> get_overlapping_cells();

	bool moveUp;
	bool moveDown;
	bool moveLeft;
	bool moveRight;

	bool jump;
	bool jump_down; // the falling process in jumping motion

	bool doubleJump;

	bool fall;

	bool on_land;

	bool on_ladder;

	bool on_rope;

	bool disableMomentum;

	bool collision_from_above;

	bool collision_from_left;

	bool collision_from_right;

	bool collision_from_below;

	bool swingRight;
	bool swingLeft;

	bool attacking;


	// True if the player has key
	bool has_key()const;

	void set_key_status(bool status);

	void set_position(vec2 position);

	vec2 get_bounding_box()const;

	void set_is_hidden(bool hidden);

	bool is_hidden()const;

	void jump_();

	void doubleJump_();

	bool not_moving();

	void fall_();

	void stopActions();

	void stickonLadder();
private:

	float m_light_up_countdown_ms; // Used to keep track for how long the salmon should be lit up

	bool m_is_alive; // True if the salmon is alive

	vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture

	float m_rotation; // in radians

	size_t m_num_indices; // passed to glDrawElements

	const float jump_initialSpeed = 33;

	const float gravity = 9.8 * 0.4;

	const float jumpTimeStep = 0.2;

	const float fallTimeStep = 0.2;

	float speedReductionRate = 2; // The rate that player character's speed reduce to 0

	bool m_is_accelerating; // True is the character is speeding up

	int m_face; // Direction the character is facing

	vec2 m_velocity; // Velocity of the character, 0 means still; >0 means towards right, <0 means towards left

	vec2 m_position; // Position of the character

	vec2 m_acceleration; // Acceleration applied on the character. 0 means still; >0 means towards right, <0 means towards left

	const float MAX_VELOCITY = 1.5;

	const float MAX_ACCELERATION = 1;

	bool m_has_key; // True if the player has key

	vec2 m_size;

	bool m_is_hidden; // True if player is hidden from enemies

	//std::vector<vec2> m_overlapping_cells;

	bool is_valid;

	float timer;

	std::vector<vec2> m_overlapping_cells;

	vec2 m_ladder_position;

	int m_division; // number of frames in the motion png

	int m_health;
	float m_immunity_timer;
	bool m_immune;
};
