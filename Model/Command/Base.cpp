#include "Base.h"
#include "../Selection.h"

using namespace GMT::Model;
using namespace GMT::Model::Command;

Base::~Base() = default;

Base::CommandContext::CommandContext(Model& model, const Selection& selection) : m_model(model), m_selection(selection) {}
Base::CommandContext::~CommandContext() = default;

SelectionPtr Base::CommandContext::HarvestSelection()
{
	return std::move(m_newSelection);
}

void Base::CommandContext::Deselect(const Selection& selection)
{
	if (!m_newSelection)
	{
		if (!m_selection.Contains(selection))
			return;

		m_newSelection = std::make_unique<Selection>(m_selection);
	}

	m_newSelection->Remove(selection);
}
