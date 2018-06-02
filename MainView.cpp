#include "MainView.h"
#include "Model/ImageObject.h"
#include "Model/VectorObject.h"

#include "SFML/Graphics.hpp"

#include <functional>

using namespace GMT;

namespace
{
	const int LogWidth = 1920;
	const int LogHeight = 1080;
	const int GridSize = 50;
	//const int GridSize = int(LogHeight / (530 / 25.4f));
}

MainView::MainView() : m_grid(LogWidth, LogHeight, GridSize)
{
	m_objects.push_back(std::make_unique<Model::ImageObject>());
	m_objects.push_back(std::make_unique<Model::VectorObject>());
}

MainView::~MainView() = default;

void MainView::Draw(sf::RenderWindow& window) const
{
	RenderContext rc(window, m_grid);
	
	for (auto& object : m_objects)
		object->Draw(rc);

	const sf::FloatRect rect = GetLogRect();
	
	sf::VertexArray border(sf::LinesStrip, 3);
	border[0].position = { LogWidth - 1, 0 };
	border[1].position = { LogWidth - 1, LogHeight - 1 };
	border[2].position = { 0, LogHeight - 1 };

	for (int i = 0; i < 3; ++i)
		border[i].color = sf::Color::White;

	sf::VertexArray gridLines(sf::Lines);
	for (int x = 0; x <= m_grid.GetCellCountX(); ++x)
	{
		gridLines.append(sf::Vertex(m_grid.GetPoint({ x, 0 }), sf::Color::Red));
		gridLines.append(sf::Vertex(m_grid.GetPoint({ x, m_grid.GetCellCountY() }), sf::Color::Red));
	}

	for (int y = 0; y <= m_grid.GetCellCountY(); ++y)
	{
		gridLines.append(sf::Vertex(m_grid.GetPoint({ 0, y }), sf::Color::Red));
		gridLines.append(sf::Vertex(m_grid.GetPoint({ m_grid.GetCellCountX(), y }), sf::Color::Red));
	}

	window.draw(border);
	window.draw(gridLines);
}

sf::FloatRect MainView::GetLogRect() const
{
	return sf::FloatRect(0, 0, (float)LogWidth, (float)LogHeight);
}

sf::FloatRect MainView::GetClipRect() const
{
	sf::FloatRect clipRect = View::GetClipRect();

	sf::Vector2u devSize(m_rect.width, m_rect.height);
	if (devSize.x <= 0 || devSize.y <= 0)
		return sf::FloatRect();

	sf::FloatRect logRect = GetLogRect();

	const float devAspect = devSize.x / float(devSize.y);
	const float logAspect = logRect.width / float(logRect.height);

	if (devAspect > logAspect)
		clipRect.width = clipRect.height * logAspect;
	else
		clipRect.height = clipRect.width / logAspect;

	return clipRect;
}
