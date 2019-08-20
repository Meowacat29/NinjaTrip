#pragma once

#include "common.hpp"

class Invisible_Wall : public Renderable
{
    // Shared between all invisible_wall, no need to load one for each instance
    static Texture invisible_wall_texture;

public:
    // Creates all the associated render resources and default transform
    bool init();

    // Releases all the associated resources
    void destroy();

    // ms represents the number of milliseconds elapsed from the previous update() call
    void update(float ms);

    // Renders the invisible_wall
    // projection is the 2D orthographic projection matrix
    void draw(const mat3& projection)override;

    // Returns the current invisible_wall position
    vec2 get_position()const;

    // Sets the new invisible_wall position
    void set_position(vec2 position);

    // Returns the invisible_wall' bounding box for collision detection, called by collides_with()
    vec2 get_bounding_box()const;

private:
    vec2 m_position; // Window coordinates
    vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
    float m_rotation; // in radians
};