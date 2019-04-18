#include "MainView.h"
#include "App.h"

#include "Model/ImageObject.h"
#include "Model/VectorObject.h"
#include "Model/Model.h"

#include "Tools/AddWallThingTool.h"
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
	return LogWidth / (float)m_rect.width;
}

void MainView::Arrange(const sf::IntRect& rect)
{
	m_rect = rect;

	sf::Vector2u devSize(m_rect.width, m_rect.height);
	if (devSize.x <= 0 || devSize.y <= 0)
		return;

	sf::FloatRect logRect({}, { (float)LogWidth, (float)LogHeight });

	const float devAspect = devSize.x / float(devSize.y);
	const float targetAspect = logRect.width / float(logRect.height);

	if (devAspect > targetAspect)
		m_rect.width = int(m_rect.height * targetAspect);
	else
		m_rect.height = int(m_rect.width / targetAspect);
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
			{
				App::Redo();
				m_tool->Update();
			}
		}
		else
		{
			if (App::CanUndo())
			{
				App::Undo();
				m_tool->Update();
			}
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
	{
		App::Load();
		m_tool->Update();
	}
	else if (event.control && event.code == sf::Keyboard::N)
	{
		App::New();
		m_tool->Update();
	}
	else if (event.code == sf::Keyboard::F1)
		m_tool = std::make_unique<Tools::SelectTool>(*this);
	else if (event.code == sf::Keyboard::F2)
		m_tool = std::make_unique<Tools::VectorTool>(*this);
	else if (event.code == sf::Keyboard::F3)
		m_tool = std::make_unique<Tools::DeleteTool>(*this);
	else if (event.code == sf::Keyboard::F4)
		m_tool = std::make_unique<Tools::AddWallThingTool>(*this);
	else
		m_tool->OnKeyPressed(event);
}
