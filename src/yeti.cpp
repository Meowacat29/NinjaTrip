// Header
#include "yeti.hpp"
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

const float IMMUNE_TIME = 1000.f;
const float DYING_TIME = 400.f;
const float TURN_TIME = 10000.f;
const float TURN_DELAY = 500.f;
const float ANGRY_SPEED = 180.f;

Texture Yeti::yeti_movement;
Texture Yeti::yeti_attack;
Texture Yeti::yeti_attack2;
Texture Yeti::yeti_run;
Texture Yeti::yeti_hurt;
Texture Yeti::yeti_dying;
Texture Yeti::yeti_dead;
Texture Yeti::yeti_deceased;
extern bool in_basicMode;

bool Yeti::init(vec2 enemy_position, vec2 player_position)
{
	// std::cout << "INIT: " << enemy_position.x << std::endl;
	// Load shared texture
	if (!yeti_movement.is_valid())
	{
		if (!yeti_run.load_from_file(textures_path("yeti/yeti-run.png")) ||
			!yeti_attack.load_from_file(textures_path("yeti/yeti-attack1.png")) ||
			!yeti_attack2.load_from_file(textures_path("yeti/yeti-attack2.png")) ||
			!yeti_hurt.load_from_file(textures_path("yeti/yeti-hurt.png")) ||
			!yeti_movement.load_from_file(textures_path("yeti/yeti-run.png")))
		{
			fprintf(stderr, "Failed to load enemy movement texture!");
			return false;
		}
	}

	if (!yeti_dead.is_valid())
	{
		if (!yeti_dying.load_from_file(textures_path("yeti/yeti-die.png")) ||
			!yeti_deceased.load_from_file(textures_path("yeti/yeti-dead.png")) ||
			!yeti_dead.load_from_file(textures_path("yeti/yeti-dead.png")))
		{
			fprintf(stderr, "Failed to load enemy dead texture!");
			return false;
		}
	}

	m_division = 6;
	// The position corresponds to the center of the texture
	float wr = yeti_movement.width * SIZE_MODIFIER / m_division;
	float hr = yeti_movement.height * SIZE_MODIFIER;
	m_size = {wr, hr};

	TexturedVertex vertices[4];
	vertices[0].position = { -wr, +hr, -0.02f };
	vertices[0].texcoord = { 0.f, 1.f };
	vertices[1].position = { +wr, +hr, -0.02f };
	vertices[1].texcoord = { 1.f, 1.f };
	vertices[2].position = { +wr, -hr, -0.02f };
	vertices[2].texcoord = { 1.f, 0.f };
	vertices[3].position = { -wr, -hr, -0.02f };
	vertices[3].texcoord = { 0.f, 0.f };

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
	if (!effect.load_from_file(shader_path("textured.vs.glsl"), shader_path("motion.fs.glsl")))
		return false;

	// Setting initial values, scale is negative to make it face the opposite way
	// 1.0 would be as big as the original texture
	m_is_alive = true;
	m_scale.x = 3.f;
	m_scale.y = 3.f;
	m_rotation = 0.f;
	m_turn_countdown = TURN_TIME;
	m_direction = -1;
	m_position = enemy_position;
	m_init_position = enemy_position;
	m_is_patroling = true;
	m_is_chasing = false;
	m_patrol_range = 1000;
	m_sight_range = 1000;
	m_patrol_speed = 40;
	m_chase_speed = 80;
	m_health = 6;
	m_immune = false;
	m_immunity_timer = IMMUNE_TIME;
	m_dying_timer = 0.f;
	m_turn_delay = 0.f;

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
void Yeti::destroy()
{
	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &mesh.ibo);
	glDeleteBuffers(1, &mesh.vao);

	// glDeleteShader(effect.vertex);
	// glDeleteShader(effect.fragment);
	// glDeleteShader(effect.program);

	effect.release(); // according to https://piazza.com/class/jp1oa2b3dx04b4?cid=257
}

void Yeti::clean(){
	m_root->clean();
	m_overlapping_cells.clear();
}

void Yeti::update(float ms, vec2 player_position, bool player_is_hidden)
{
	if (m_dying_timer > 0.f) {
		m_dying_timer -= ms;
		yeti_dead.id = yeti_dying.id;
		m_division = 9;
	} else if (!m_is_alive) {
		m_immune = false;
		yeti_dead.id = yeti_deceased.id;
		m_division = 1;
	} else {
		if (m_immunity_timer <= 0.f) {
			m_immune = false;
			m_immunity_timer = IMMUNE_TIME;
		}
		if (m_immune) {
			m_immunity_timer -= ms;
			yeti_movement.id = yeti_hurt.id;
			m_division = 3;
			return;
		}

		if (m_turn_delay > 0) {
			m_turn_delay -= ms;
		}

		m_chaseTask->set_player_position(player_position);
		m_patrolTask->set_player_position(player_position);

		m_chaseTask->set_enemy_position(m_position);
		m_patrolTask->set_enemy_position(m_position);

		m_patrolTask->set_time(ms);

		if (m_is_patroling) {
			yeti_movement.id = yeti_run.id;
			m_division = 6;
			patrol(ms, player_position, player_is_hidden);
		}
		else if (m_is_chasing) {
			if (m_health <= 3) {
				m_chase_speed = ANGRY_SPEED;
				yeti_movement.id = yeti_attack2.id;
				m_division = 7;
			} else {
				yeti_movement.id = yeti_attack.id;
				m_division = 6;
			}
			chase(ms, player_position, player_is_hidden);
		}
	}
}

bool Yeti::patrol(float ms, vec2 player_position, bool player_is_hidden)
{
	if (isWithinSightRange(player_position) && !player_is_hidden) {
		// std::cout << "Found player!!!" << std::endl;
		m_is_patroling = false;
		m_patrolTask->find_player(true);
		m_is_chasing = true;
		return true;
	}

	// std::cout << "Patroling:  " << m_position.x << std::endl;
	float step = m_patrol_speed * (ms / 1000);
	m_position.x += m_direction * step;

	if (m_turn_countdown <= 0) {
		// std::cout << "Turning:  " << m_position.x << std::endl;
		m_turn_countdown = TURN_TIME;
		m_direction = -m_direction;
		// if ((m_direction > 0 && m_scale.x > 0)  || (m_direction < 0 && m_scale.x < 0)) {
		// 	m_scale.x *= -1.f;
		// }
		m_scale.x *= -1.f;

	}
	else {
		m_turn_countdown -= ms;
	}
	return false;
}

bool Yeti::chase(float ms, vec2 player_position, bool player_is_hidden)
{
	float step = m_chase_speed * (ms / 1000);
	m_turn_countdown -= m_chase_speed/m_patrol_speed * ms;
	if (m_turn_countdown <= 0) {
		// std::cout << "Turning:  " << m_position.x << std::endl;
		m_turn_countdown = TURN_TIME;
		m_direction = -m_direction;
		// if ((m_direction > 0 && m_scale.x > 0)  || (m_direction < 0 && m_scale.x < 0)) {
		// 	m_scale.x *= -1.f;
		// }
		m_scale.x *= -1.f;

	}
	if (isWithinSightRange(player_position) && !player_is_hidden) {
		// std::cout << "Chasing player:  " << m_position.x << std::endl;
		if (m_position.x > player_position.x)
			m_position.x += m_direction * step;
		else if (m_position.x < player_position.x)
			m_position.x += m_direction * step;
		if (abs(player_position.x - m_position.x) <= 1) { // MEANINGLESS CODE FOR NOW
			//m_chaseTask->catch_player(true);
			//m_patrolTask->find_player(true);
			//m_is_chasing = false;
			//m_is_patroling = false;
		}
	}
	else {
		if (m_turn_delay <= 0) {
			m_is_chasing = false;
			m_is_patroling = true;
			m_patrolTask->find_player(false);
			m_chaseTask->catch_player(false);
			m_direction = -m_direction;
			m_scale.x *= -1.f;
			m_turn_delay = TURN_DELAY;
		}
	}

	return false;
}

bool Yeti::isWithinSightRange(vec2 player_position) {
	return ((abs(m_position.x - player_position.x) < m_sight_range) && (player_position.x - m_position.x) * m_direction > 0);
}

bool Yeti::isWithinActRange() {
	return (m_init_position.x - m_patrol_range < m_position.x && m_position.x < m_init_position.x + m_patrol_range);
}

void Yeti::draw(const mat3& projection)
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
	GLuint division_uloc = glGetUniformLocation(effect.program, "division");
	GLuint time_uloc = glGetUniformLocation(effect.program, "time");
	GLuint is_immune_uloc = glGetUniformLocation(effect.program, "is_immune");
	glEnableVertexAttribArray(in_position_loc);
	glEnableVertexAttribArray(in_texcoord_loc);
	glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)0);
	glVertexAttribPointer(in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)sizeof(vec3));
	glUniform1f(time_uloc, (float)(glfwGetTime() * 5.0f));
	glUniform1f(division_uloc, (float)(m_division * 1.0f));
	glUniform1i(is_immune_uloc, (int)(m_immune));

	// Enabling and binding texture to slot 0
	glActiveTexture(GL_TEXTURE0);
	if (m_is_alive) {
		glBindTexture(GL_TEXTURE_2D, yeti_movement.id);
	} else {
		glBindTexture(GL_TEXTURE_2D, yeti_dead.id);
	}

	// Setting uniform values to the currently bound program
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform);
	float color[] = { 1.f, 1.f, 1.f };
	glUniform3fv(color_uloc, 1, color);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);

	// Drawing!
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
}

vec2 Yeti::get_position()const
{
	return m_position;
}

void Yeti::set_position(vec2 position)
{
	m_position = position;
}

float Yeti::get_countdown()
{
	return m_turn_countdown;
}

void Yeti::set_countdown(float countdown)
{
	m_turn_countdown = countdown;
}


float Yeti::get_direction()
{
	return m_direction;
}

void Yeti::set_direction(float dir)
{
	m_direction = dir;
}

float Yeti::get_scale_x()
{
	return m_scale.x;
}

void Yeti::set_scale_x(float x)
{
	m_scale.x = x;
}

// Returns the local bounding coordinates scaled by the current size of the turtle
vec2 Yeti::get_bounding_box()const
{
	// fabs is to avoid negative scale due to the facing direction
	return { std::fabs(m_scale.x) * m_size.x, std::fabs(m_scale.y) * m_size.y };
}

void Yeti::set_is_patroling(bool rvalue)
{
	m_is_patroling = rvalue;
}

void Yeti::set_is_chasing(bool rvalue)
{
	m_is_chasing = rvalue;
}

void Yeti::set_patrol_range(float rv)
{
	m_patrol_range = rv;
}

void Yeti::set_sight_range(float rv)
{
	m_sight_range = rv;
}

std::vector<vec2> Yeti::get_overlapping_cells(float tileSize, int width, int height) {
	m_overlapping_cells.clear();
	vec2 center = { static_cast<float>(ceil(m_position.x / tileSize)), static_cast<float>(ceil(m_position.y / tileSize)) };
	m_overlapping_cells.push_back(center);
	m_overlapping_cells.push_back({ center.x + 1, center.y });
	m_overlapping_cells.push_back({ center.x - 1, center.y });
	m_overlapping_cells.push_back({ center.x + 1, center.y });
	m_overlapping_cells.push_back({ center.x,     center.y + 1 });
	m_overlapping_cells.push_back({ center.x,	  center.y - 1 });
	m_overlapping_cells.push_back({ center.x + 1, center.y + 1 });
	m_overlapping_cells.push_back({ center.x + 1, center.y - 1 });
	m_overlapping_cells.push_back({ center.x - 1, center.y + 1 });
	m_overlapping_cells.push_back({ center.x - 1, center.y - 1 });
	return m_overlapping_cells;
}

bool Yeti::is_alive()const
{
	return m_is_alive;
}

void Yeti::kill()
{
	m_dying_timer = DYING_TIME;
	m_is_alive = false;
	m_division = 1;
}

int Yeti::get_health()
{
	return m_health;
}

bool Yeti::take_damage()
{
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

vec2 Yeti::get_scale()const
{
	return m_scale;
}

bool Yeti::is_chasing()const
{
	return m_is_chasing;
}

bool Yeti::is_immune()const
{
	return m_immune;
}