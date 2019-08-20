#pragma once

#include "common.hpp"
class Icon;

class Mouse : public Renderable
{
	// Shared between all mouse, no need to load one for each instance
	static Texture mouse_texture;

public:
	// Creates all the associated render resources and default transform
	bool init();

	// Releases all the associated resources
	void destroy();

	// Renders the mouse
	// projection is the 2D orthographic projection matrix
	void draw(const mat3& projection)override;

	// Returns the current mouse position
	vec2 get_position()const;

	// Sets the new mouse position
	void set_position(vec2 position);

	// Returns the mouse' bounding box for collision detection, called by collides_with()
	vec2 get_bounding_box()const;

	bool collides_with(const Icon& icon);

private:
	vec2 m_position; // Window coordinates
	vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
	float m_rotation; // in radians
};