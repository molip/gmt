#pragma once

#include "Object.h"

#include "Jig/VectorFwd.h"

#include <SFML/System.hpp>

#include <optional>
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

		bool Tesselate();

	private:
		using WallPoints = std::optional<std::pair<Jig::Vec2f, Jig::Vec2f>>;

		WallPoints GetWallPointsLog(int index) const;
		
		std::vector<sf::Vector2i> m_points;
		std::unique_ptr<std::vector<sf::Vector2i>> m_tesselated;
	};
}
