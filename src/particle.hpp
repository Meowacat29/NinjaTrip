#pragma once

#include "common.hpp"

class Particle : public Renderable
{
	// Shared between all particle, no need to load one for each instance
	static Texture particle_texture;

public:
	// Creates all the associated render resources and default transform
	bool init();

	// Releases all the associated resources
	void destroy();
	
	// Update particle due to current
	// ms represents the number of milliseconds elapsed from the previous update() call
	void update();

	// Renders the particle
	// projection is the 2D orthographic projection matrix
	void draw(const mat3& projection)override;

	// Returns the current particle position
	vec2 get_position()const;

	// Sets the new particle position
	void set_position(vec2 position);

	// Returns the particle' bounding box for collision detection, called by collides_with()
	vec2 get_bounding_box()const;

private:
	float step;
	float timer;
	vec2 m_position; // Window coordinates
	vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
	float m_rotation; // in radians
};