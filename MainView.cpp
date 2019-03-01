#include "MainView.h"
#include "App.h"

#include "Model/ImageObject.h"
#include "Model/VectorObject.h"
#include "Model/Model.h"

#include "Tools/DeleteTool.h"
#include "Tools/SelectTool.h"
#include "Tools/VectorTool.h"

#include "SFML/Graphics.hpp"

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
	m_tool = std::make_unique<Tools::SelectTool>(*this);
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
	sf::Color gridColour = sf::Color::Yellow;
	gridColour.a = 128;
	for (int x = 0; x <= m_grid.GetCellCountX(); ++x)
	{
		gridLines.append(sf::Vertex(m_grid.GetPoint(sf::Vector2i(x, 0)), gridColour));
		gridLines.append(sf::Vertex(m_grid.GetPoint(sf::Vector2i(x, m_grid.GetCellCountY())), gridColour));
	}

	for (int y = 0; y <= m_grid.GetCellCountY(); ++y)
	{
		gridLines.append(sf::Vertex(m_grid.GetPoint(sf::Vector2i(0, y)), gridColour));
		gridLines.append(sf::Vertex(m_grid.GetPoint(sf::Vector2i(m_grid.GetCellCountX(), y)), gridColour));
	}

	window.draw(border);

	App::GetModel().Draw(rc);

	window.draw(gridLines);

	m_tool->Draw(rc);
}

float MainView::GetDevToLogScale() const
{
	return m_rect.width / (float)LogWidth;
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

void MainView::OnMouseMoved(const sf::Vector2i& point)
{
	m_tool->OnMouseMoved(point);
}

void MainView::OnMouseDown(sf::Mouse::Button button, const sf::Vector2i& point)
{
	m_tool->OnMouseDown(button, point);
}

void MainView::OnMouseUp(sf::Mouse::Button button, const sf::Vector2i& point)
{
	m_tool->OnMouseUp(button, point);
}

void MainView::OnKeyPressed(const sf::Event::KeyEvent event) 
{
	if (event.control && event.code == sf::Keyboard::Z)
	{
		if (event.shift)
		{
			if (App::CanRedo())
				App::Redo();
		}
		else
		{
			if (App::CanUndo())
				App::Undo();
		}
	}
	else if (event.control && event.code == sf::Keyboard::S)
	{
		if (event.shift)
			App::SaveAs();
		else
			App::Save();
	}
	else if (event.control && event.code == sf::Keyboard::O)
		App::Load();
	else if (event.control && event.code == sf::Keyboard::N)
		App::New();
	else if (event.code == sf::Keyboard::F1)
		m_tool = std::make_unique<Tools::SelectTool>(*this);
	else if (event.code == sf::Keyboard::F2)
		m_tool = std::make_unique<Tools::VectorTool>(*this);
	else if (event.code == sf::Keyboard::F3)
		m_tool = std::make_unique<Tools::DeleteTool>(*this);
	else
		m_tool->OnKeyPressed(event);
}
