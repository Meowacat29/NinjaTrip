// Header
#include "terrain.hpp"

#include <cmath>

Texture Terrain::terrain_texture_0;
Texture Terrain::terrain_texture_1;
Texture Terrain::terrain_texture_2;
Texture Terrain::terrain_texture_3;
Texture Terrain::terrain_texture_4;

Texture Terrain::terrain_texture_black;

#if _WIN32
const double SIZE_MODIFIER = 0.25;
#elif __APPLE__
const double SIZE_MODIFIER = 0.5;
#elif __linux__
// linux
#endif

bool Terrain::init(char type)
{
	if(type == 't'){
		int ran =(rand()%5);
		m_sprite = ran;
		// Load shared texture
		switch(ran){
				case 0:		
					if (!terrain_texture_0.load_from_file(textures_path("brick0.png")))
					{
						fprintf(stderr, "Failed to load terrain texture!");
						return false;
					}
					break;
				case 1:		
					if (!terrain_texture_1.load_from_file(textures_path("brick1.png")))
					{
						fprintf(stderr, "Failed to load terrain texture!");
						return false;
					}
					break;
				case 2:		
					if (!terrain_texture_2.load_from_file(textures_path("brick2.png")))
					{
						fprintf(stderr, "Failed to load terrain texture!");
						return false;
					}
					break;
				case 3:		
					if (!terrain_texture_3.load_from_file(textures_path("brick3.png")))
					{
						fprintf(stderr, "Failed to load terrain texture!");
						return false;
					}
					break;
				case 4:		
					if (!terrain_texture_4.load_from_file(textures_path("brick4.png")))
					{
						fprintf(stderr, "Failed to load terrain texture!");
						return false;
					}
					break;
			}
	}else if(type == 'T'){
		// Load shared texture
		if (!terrain_texture_black.is_valid())
		{
			if (!terrain_texture_black.load_from_file(textures_path("brick_black.png")))
			{
				fprintf(stderr, "Failed to load terrain texture!");
				return false;
			}
		}		
	}
	m_type = type;
	
	// The position corresponds to the center of the texture
	// all terrain have same size, so use terrain_texture_0 to represent all
	float wr = terrain_texture_0.width * SIZE_MODIFIER;
	float hr = terrain_texture_0.height * SIZE_MODIFIER;
	m_size = { wr, hr };


	TexturedVertex vertices[4];
	vertices[0].position = { -wr, +hr, -0.01f };
	vertices[0].texcoord = { 0.f, 1.f };
	vertices[1].position = { +wr, +hr, -0.01f };
	vertices[1].texcoord = { 1.f, 1.f,  };
	vertices[2].position = { +wr, -hr, -0.01f };
	vertices[2].texcoord = { 1.f, 0.f };
	vertices[3].position = { -wr, -hr, -0.01f };
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
	if (!effect.load_from_file(shader_path("textured.vs.glsl"), shader_path("textured.fs.glsl")))
		return false;

	// Setting initial values, scale is negative to make it face the opposite way
	// 1.0 would be as big as the original texture
	m_scale.x = 2.4f;
	m_scale.y = 2.4f;
	m_rotation = 0.f;

	return true;
}

// Call if init() was successful
// Releases all graphics resources
void Terrain::destroy()
{
	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &mesh.ibo);
	glDeleteBuffers(1, &mesh.vao);

	// glDeleteShader(effect.vertex);
	// glDeleteShader(effect.fragment);
	// glDeleteShader(effect.program);

	effect.release(); // according to https://piazza.com/class/jp1oa2b3dx04b4?cid=257
}

void Terrain::update(float ms)
{
}

void Terrain::draw(const mat3& projection)
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
	if(get_type()=='t'){
		if(m_sprite == 0)
			glBindTexture(GL_TEXTURE_2D, terrain_texture_0.id);
		else if(m_sprite == 1)
			glBindTexture(GL_TEXTURE_2D, terrain_texture_1.id);
		else if(m_sprite == 2)
			glBindTexture(GL_TEXTURE_2D, terrain_texture_2.id);
		else if(m_sprite == 3)
			glBindTexture(GL_TEXTURE_2D, terrain_texture_3.id);
		else if(m_sprite == 4)
			glBindTexture(GL_TEXTURE_2D, terrain_texture_4.id);
	}
	else if(get_type()=='T')
		glBindTexture(GL_TEXTURE_2D, terrain_texture_black.id);


	// Setting uniform values to the currently bound program
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform);
	float color[] = { 1.f, 1.f, 1.f };
	glUniform3fv(color_uloc, 1, color);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);

	// Drawing!
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
}

char Terrain::get_type()const
{
	return m_type;
}

vec2 Terrain::get_position()const
{
	return m_position;
}

void Terrain::set_position(vec2 position)
{
	m_position = position;
}

// Returns the local bounding coordinates scaled by the current size of the terrain
vec2 Terrain::get_bounding_box()const
{
	// fabs is to avoid negative scale due to the facing direction
	return { std::fabs(m_scale.x) * m_size.x * 0.8f, std::fabs(m_scale.y) * m_size.y * 0.85f};
}