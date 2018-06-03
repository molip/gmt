#pragma once

#include "Object.h"

#include <SFML/System.hpp>

#include <vector>

namespace GMT::Model
{
	class VectorObject : public Object
	{
	public:
		VectorObject();
		virtual ~VectorObject();

		virtual void Draw(RenderContext& rc) const override;

		bool IsClosed() const;
		std::vector<sf::Vector2i>& GetPoints() { return m_points; }

	private:
		std::vector<sf::Vector2i> m_points;
	};
}
