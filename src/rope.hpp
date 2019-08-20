#pragma once

#include "common.hpp"

// Player food
class Rope : public Renderable
{
	// Shared between all rope, no need to load one for each instance
	static Texture rope_texture;

public:
	// Creates all the associated render resources and default transform
	bool init();

	// Releases all the associated resources
	void destroy();

	// Update rope due to current
	// ms represents the number of milliseconds elapsed from the previous update() call
	void update(float ms);

	// Renders the rope
	// projection is the 2D orthographic projection matrix
	void draw(const mat3& projection)override;

	// Returns the current rope position
	vec2 get_position()const;

	// Sets the new rope position
	void set_position(vec2 position);

	// Returns the rope' bounding box for collision detection, called by collides_with()
	vec2 get_bounding_box()const;

	// Calculate current position depending on rotation
	vec2 calculate_current_position(float y)const;

	// Returns swinging state of rope
	bool is_swinging()const;

	// Sets the swinging state
	void set_swinging(bool swing);

	// Sets the swinging state
	void set_initial_velocity(float v);

	// get distance of player on rope when first collision happens
	float get_swingDistance();

	// set distance of player on rope when first collision happens
	void set_swingDistance(float f);

private:
	vec2 m_position; // Window coordinates
	vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
	float m_rotation; // in radians
	vec2 m_size;
	vec2 m_rotation_range; // rotation range
	float m_rotation_velocity; // rotation speed
	bool m_swinging; // is the rope swinging
	float m_rotation_acceleration; // rotational acceleration
	float m_timer;
	float m_initial_velocity; // Initial velocity dependent on player's velocity
	float m_swingDistance;
};