// Header
#include "enemy.hpp"
#include <cmath>
#include <math.h>
#include "node.hpp"

#if _WIN32
const double SIZE_MODIFIER = 0.225;
#elif __APPLE__
const double SIZE_MODIFIER = 0.45;
#elif __linux__
// linux
#endif

Texture Enemy::enemy_texture;
extern bool in_basicMode;

bool Enemy::init(vec2 enemy_position, vec2 player_position)
{
	return false;
}

// Call if init() was successful
// Releases all graphics resources
void Enemy::destroy()
{
	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &mesh.ibo);
	glDeleteBuffers(1, &mesh.vao);

	// glDeleteShader(effect.vertex);
	// glDeleteShader(effect.fragment);
	// glDeleteShader(effect.program);
	
	effect.release(); // according to https://piazza.com/class/jp1oa2b3dx04b4?cid=257

}

void Enemy::update(float ms, vec2 player_position, bool player_is_hidden)
{
	m_chaseTask->set_player_position(player_position);
	m_patrolTask->set_player_position(player_position);

	m_chaseTask->set_enemy_position(m_position);
	m_patrolTask->set_enemy_position(m_position);

	m_patrolTask->set_time(ms);

	if (m_is_patroling) {
		patrol(ms, player_position, player_is_hidden);
	}
	else if (m_is_chasing) {
		chase(ms, player_position, player_is_hidden);
	}
}

bool Enemy::patrol(float ms, vec2 player_position, bool player_is_hidden)
{
	return false;
}

bool Enemy::chase(float ms, vec2 player_position, bool player_is_hidden)
{
	return false;
}

bool Enemy::isWithinSightRange(vec2 player_position) {
	return ((abs(m_position.x - player_position.x) < m_sight_range) && (player_position.x - m_position.x) * m_direction > 0);
}

bool Enemy::isWithinActRange() {
	return (m_init_position.x - m_patrol_range < m_position.x && m_position.x < m_init_position.x + m_patrol_range);
}

void Enemy::draw(const mat3& projection)
{
	// Transformation code, see Rendering and Transformation in the template specification for more info
	// Incrementally updates transformation matrix, thus ORDER IS IMPORTANT
	transform_begin();
	transform_translate(m_position);
	transform_rotate(m_rotation);
	transform_scale(m_scale);
	transform_end();

	// Setting shaders
	glUseProgram(effect.program);

	// Enabling alpha channel for textures
	glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);

	// Getting uniform locations for glUniform* calls
	GLint transform_uloc = glGetUniformLocation(effect.program, "transform");
	GLint color_uloc = glGetUniformLocation(effect.program, "fcolor");
	GLint projection_uloc = glGetUniformLocation(effect.program, "projection");

	// Setting vertices and indices
	glBindVertexArray(mesh.vao);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);

	// Input data location as in the vertex buffer
	GLint in_position_loc = glGetAttribLocation(effect.program, "in_position");
	GLint in_texcoord_loc = glGetAttribLocation(effect.program, "in_texcoord");
	glEnableVertexAttribArray(in_position_loc);
	glEnableVertexAttribArray(in_texcoord_loc);
	glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)0);
	glVertexAttribPointer(in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)sizeof(vec3));

	// Enabling and binding texture to slot 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, enemy_texture.id);

	// Setting uniform values to the currently bound program
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform);
	float color[] = { 1.f, 1.f, 1.f };
	glUniform3fv(color_uloc, 1, color);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);

	// Drawing!
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
}

vec2 Enemy::get_position()const
{
	return m_position;
}

void Enemy::set_position(vec2 position)
{
	m_position = position;
}

// Returns the local bounding coordinates scaled by the current size of the enemy
vec2 Enemy::get_bounding_box()const
{
	// fabs is to avoid negative scale due to the facing direction
	return { std::fabs(m_scale.x) * enemy_texture.width, std::fabs(m_scale.y) * enemy_texture.height };
}

void Enemy::set_is_patroling(bool rvalue)
{
	m_is_patroling = rvalue;
}

void Enemy::set_is_chasing(bool rvalue)
{
	m_is_chasing = rvalue;
}

void Enemy::set_patrol_range(float rv)
{
	m_patrol_range = rv;
}

void Enemy::set_sight_range(float rv)
{
	m_sight_range = rv;
}