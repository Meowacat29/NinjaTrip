#pragma once

#include "common.hpp"
#include "node.hpp"
// Player enemy
class Enemy : public Renderable
{
	// Shared between all turtles, no need to load one for each instance
	static Texture enemy_texture;

public:
	// Creates all the associated render resources and default transform
	bool init(vec2 turtle_positon, vec2 player_position);

	// Releases all the associated resources
	void destroy();

	// Update turtle due to current
	// ms represents the number of milliseconds elapsed from the previous update() call
	void update(float ms, vec2 player_position, bool player_is_hidden);

	// Renders the salmon
	// projection is the 2D orthographic projection matrix
	void draw(const mat3& projection)override;

	// Returns the current turtle position
	vec2 get_position()const;

	// Sets the new turtle position
	void set_position(vec2 position);

	// Returns the turtle' bounding box for collision detection, called by collides_with()
	vec2 get_bounding_box()const;

	// Player interaction
	bool isWithinSightRange(vec2 player_position);
	bool isWithinActRange();

	// Behavior Tree
	bool patrol(float ms, vec2 player_position, bool player_is_hidden);
	bool chase(float ms, vec2 player_position, bool player_is_hidden);
	void set_is_patroling(bool);
	void set_is_chasing(bool);
	void set_patrol_range(float);
	void set_sight_range(float);

private:
	// Decision tree fields
	SequenceNode* m_root;
	CheckPlayerIsDeadTask* m_checkPlayerIsDeadTask;
	PatrolTask* m_patrolTask;
	ChaseTask* m_chaseTask;
	PlayerStatus* m_playerStatus;

	bool m_is_patroling;
	bool m_is_chasing;
	bool m_caughtPlayer = false;

	// Property fields
	float m_turn_countdown;
	float m_direction;
	vec2 m_init_position;
	vec2 m_position; // Window coordinates
	vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
	float m_rotation; // in radians
	float m_patrol_range;
	float m_sight_range;
	float m_patrol_speed;
	float m_chase_speed;
};