#pragma once

#include "common.hpp"

class Door : public Renderable
{
	// Shared between all door, no need to load one for each instance
	static Texture door_texture;

public:
	// Creates all the associated render resources and default transform
	bool init();

	// Releases all the associated resources
	void destroy();

	// Renders the door
	// projection is the 2D orthographic projection matrix
	void draw(const mat3& projection)override;

	// Returns the current door position
	vec2 get_position()const;

	// sets the current door position
	void set_position(vec2 position);
	// Returns the door' bounding box for collision detection, called by collides_with()
	vec2 get_bounding_box()const;

private:
	vec2 m_position; // Window coordinates
	vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
	float m_rotation; // in radians
};