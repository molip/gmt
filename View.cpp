#include "View.h"

#include "SFML/Graphics.hpp"

using namespace GMT;

namespace
{
	sf::IntRect operator &(const sf::IntRect& lhs, const sf::IntRect& rhs)
	{
		int left = std::max(lhs.left, rhs.left);
		int top = std::max(lhs.top, rhs.top);
		int right = std::min(lhs.left + lhs.width, rhs.left + rhs.width);
		int bottom = std::min(lhs.top + lhs.height, rhs.top + rhs.height);

		if (left >= right)
			left = right = lhs.left;

		if (top >= bottom)
			top = bottom = lhs.top;

		return sf::IntRect({ left, top }, { right - left, bottom - top });
	}
}

View::View() : m_parent{}
{
}

View::~View() = default;

View& View::AddChild(std::unique_ptr<View> child)
{
	child->m_parent = this;
	m_children.push_back(std::move(child));
	return *m_children.back();
}

void View::BaseDraw(sf::RenderWindow& window, const sf::IntRect& parentClip) const
{
	const sf::IntRect clip = m_rect & parentClip;

	sf::View view(GetLogRect());

	const float width = float(window.getSize().x);
	const float height = float(window.getSize().y);
	view.setViewport(sf::FloatRect(clip.left / width, clip.top / height, clip.width / width, clip.height / height));

	window.setView(view);

	Draw(window);

	for (auto& child : m_children)
		child->BaseDraw(window, clip);
}

void View::Arrange(const sf::IntRect& rect)
{
	m_rect = rect;
}

sf::FloatRect View::GetLogRect() const
{
	float scale = GetDevToLogScale();
	return sf::FloatRect(0, 0, (float)m_rect.width * scale, (float)m_rect.height * scale);
}

View* View::HitTest(const sf::Vector2i& worldPoint) 
{
	if (!m_rect.contains(worldPoint))
		return nullptr;

	for (auto& child : m_children)
		if (View* subView = child->HitTest(worldPoint))
			return subView;

	return this;
}

sf::Vector2i View::WorldToLocal(const sf::Vector2i& point) const
{
	return { point.x - m_rect.left, point.y - m_rect.top };
}

sf::Vector2f View::DevToLog(const sf::Vector2i& point) const
{
	sf::FloatRect logRect = GetLogRect();

	float normX = float(point.x) / float(m_rect.width);
	float normY = float(point.y) / float(m_rect.height);

	return { logRect.left + normX * logRect.width, logRect.top + normY * logRect.height };
}
