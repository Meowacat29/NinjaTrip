// Header
#include "archer.hpp"
#include <cmath>
#include <math.h>
#include "node.hpp"

#if _WIN32
const double SIZE_MODIFIER = 0.25f;
#elif __APPLE__
const double SIZE_MODIFIER = 0.5f;
#elif __linux__
// linux
#endif

Texture Archer::archer_texture_alive;
Texture Archer::archer_texture_dead;
extern bool in_basicMode;
const float ARROW_DELAY = 2000.f;

bool Archer::init(vec2 enemy_position, vec2 player_position)
{
	// Load shared texture
	if (!archer_texture_alive.is_valid())
	{
		if (!archer_texture_alive.load_from_file(textures_path("archer.png")))
		{
			fprintf(stderr, "Failed to load alive archer texture!");
			return false;
		}
	}

	// Load shared texture
	if (!archer_texture_dead.is_valid())
	{
		if (!archer_texture_dead.load_from_file(textures_path("archer-dead.png")))
		{
			fprintf(stderr, "Failed to load dead archer texture!");
			return false;
		}
	}

	// The position corresponds to the center of the texture
	float wr = archer_texture_alive.width * SIZE_MODIFIER;
	float hr = archer_texture_alive.height *SIZE_MODIFIER;
	m_size = {wr, hr};

	TexturedVertex vertices[4];
	vertices[0].position = { -wr, +hr, -0.02f };
	vertices[0].texcoord = { 1.f, 1.f };
	vertices[1].position = { +wr, +hr, -0.02f };
	vertices[1].texcoord = { 0.f, 1.f };
	vertices[2].position = { +wr, -hr, -0.02f };
	vertices[2].texcoord = { 0.f, 0.f };
	vertices[3].position = { -wr, -hr, -0.02f };
	vertices[3].texcoord = { 1.f, 0.f };

	// counterclockwise as it's the default opengl front winding direction
	uint16_t indices[] = { 0, 3, 1, 1, 3, 2 };

	// Clearing errors
	gl_flush_errors();

	// Vertex Buffer creation
	glGenBuffers(1, &mesh.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TexturedVertex) * 4, vertices, GL_STATIC_DRAW);

	// Index Buffer creation
	glGenBuffers(1, &mesh.ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * 6, indices, GL_STATIC_DRAW);

	// Vertex Array (Container for Vertex + Index buffer)
	glGenVertexArrays(1, &mesh.vao);
	if (gl_has_errors())
		return false;

	// Loading shaders
	if (!effect.load_from_file(shader_path("textured.vs.glsl"), shader_path("textured.fs.glsl")))
		return false;

	// Setting initial values, scale is negative to make it face the opposite way
	// 1.0 would be as big as the original texture
	m_scale.x = 1.5f;
	m_scale.y = 1.5f;
	m_rotation = 0.f;
	m_turn_countdown = 3500.f;
	m_direction = -1;
	m_position = enemy_position;
	m_init_position = enemy_position;
	m_is_patroling = true;
	m_is_chasing = false;
	m_patrol_range = 200;
	m_sight_range = 240;
	m_patrol_speed = 60;
	m_chase_speed = 80;
	m_shoot_arrow = false;
	m_next_arrow_spawn = 0.f;
	m_is_alive = true;

	// Generate decision tree
	m_root = new SequenceNode();
	SequenceNode* sequence = new SequenceNode();
	SelectorNode* selector = new SelectorNode();
	PlayerStatus* playerStatus = new PlayerStatus();
	playerStatus->m_player_is_dead = false;
	playerStatus->m_player_position = player_position;
	playerStatus->m_enemy_position = m_position;
	playerStatus->m_enemy_direction = m_direction;

	m_checkPlayerIsDeadTask = new CheckPlayerIsDeadTask(playerStatus);
	m_patrolTask = new PatrolTask(playerStatus, this);
	m_chaseTask = new ChaseTask(playerStatus, this);

	m_root->addChild(selector);
	selector->addChild(m_checkPlayerIsDeadTask);
	selector->addChild(sequence);
	sequence->addChild(m_patrolTask);
	sequence->addChild(m_chaseTask);
	return true;
}

// Call if init() was successful
// Releases all graphics resources
void Archer::destroy()
{
	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &mesh.ibo);
	glDeleteBuffers(1, &mesh.vao);

	// glDeleteShader(effect.vertex);
	// glDeleteShader(effect.fragment);
	// glDeleteShader(effect.program);

	effect.release(); // according to https://piazza.com/class/jp1oa2b3dx04b4?cid=257

}

void Archer::update(float ms, vec2 player_position, bool player_is_hidden)
{
	if (m_is_alive) {
		if (m_next_arrow_spawn > 0.f)
			m_next_arrow_spawn -= ms;
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
}

void Archer::clean(){
	m_root->clean();
	//m_overlapping_cells.clear();
}

bool Archer::patrol(float ms, vec2 player_position, bool player_is_hidden)
{
	if (isWithinSightRange(player_position) && !player_is_hidden) {
		m_is_patroling = false;
		m_patrolTask->find_player(true);
		m_is_chasing = true;
		return true;
	}
	return false;
}

bool Archer::chase(float ms, vec2 player_position, bool player_is_hidden)
{
	if (isWithinSightRange(player_position) && !player_is_hidden) {
		m_shoot_arrow = true;
	}
	else {
		m_is_chasing = false;
		m_is_patroling = true;
		m_patrolTask->find_player(false);
		m_chaseTask->catch_player(false);
		m_shoot_arrow = false;
	}

	return false;
}

bool Archer::isWithinSightRange(vec2 player_position) {
	return ((abs(m_position.x - player_position.x) < m_sight_range) && (player_position.x - m_position.x) * m_direction > 0);
}

bool Archer::isWithinActRange() {
	return (m_init_position.x - m_patrol_range < m_position.x && m_position.x < m_init_position.x + m_patrol_range);
}

void Archer::draw(const mat3& projection)
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
	if (m_is_alive) {
		glBindTexture(GL_TEXTURE_2D, archer_texture_alive.id);
	} else {
		glBindTexture(GL_TEXTURE_2D, archer_texture_dead.id);
	}

	// Setting uniform values to the currently bound program
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform);
	float color[] = { 1.f, 1.f, 1.f };
	glUniform3fv(color_uloc, 1, color);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);

	// Drawing!
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
}

vec2 Archer::get_position()const
{
	return m_position;
}

void Archer::set_position(vec2 position)
{
	m_position = position;
}

// Returns the local bounding coordinates scaled by the current size of the archer
vec2 Archer::get_bounding_box()const
{
	// fabs is to avoid negative scale due to the facing direction
	return { std::fabs(m_scale.x) * m_size.x, std::fabs(m_scale.y) * m_size.y };
}

void Archer::set_is_patroling(bool rvalue)
{
	m_is_patroling = rvalue;
}

void Archer::set_is_chasing(bool rvalue)
{
	m_is_chasing = rvalue;
}

void Archer::set_patrol_range(float rv)
{
	m_patrol_range = rv;
}

void Archer::set_sight_range(float rv)
{
	m_sight_range = rv;
}

bool Archer::get_shoot_arrow()
{
	return m_shoot_arrow;
}

void Archer::set_shoot_arrow(bool shoot)
{
	m_shoot_arrow = shoot;
}

float Archer::get_next_arrow_spawn()
{
	return m_next_arrow_spawn;
}

void Archer::reset_next_arrow_spawn()
{
	m_next_arrow_spawn = ARROW_DELAY;
}

bool Archer::is_alive()const
{
	return m_is_alive;
}

void Archer::kill()
{
	m_is_alive = false;
}