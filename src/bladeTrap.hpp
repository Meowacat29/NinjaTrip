#pragma once

#include "common.hpp"

class BladeTrap : public Renderable
{
	private: 
		static Texture trap_texture;
		vec2   m_position;
		vec2  m_scale;
		float  m_rotation;
		size_t m_num_indices;
		vec3 m_color;

	public:
		bool  init();
		void  destroy();
		void  update(float);
		void  draw(const mat3&)override;
		vec2  get_position() const;
		void  set_position(vec2 position);
		float get_radius() const;
};	