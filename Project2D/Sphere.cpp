#include "Sphere.h"

void Sphere::draw()
{
	// segments = lines that make up the circle perimeter (6 = hexagon)
	aie::Gizmos::add2DCircle(m_position, m_radius, 24, m_colour);

	glm::vec2 end = glm::vec2(std::cos(m_orientation), std::sin(m_orientation)) * m_radius;
	//if (!getIsHole())
	//	aie::Gizmos::add2DLine(m_position, m_position + end, glm::vec4(1, 0, 0, 1));
	//printf("\nDrawing sphere at %f %f\n", m_position.x, m_position.y);
}
