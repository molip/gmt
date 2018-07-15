#include "Command/Base.h"
#include "CommandStack.h"

#include "libKernel/Debug.h"

using namespace GMT::Model;

CommandStack::CommandStack(Model& model) : m_model(model) {}

CommandStack::~CommandStack() = default;

void CommandStack::AddCommand(CommandPtr command, bool alreadyDone)
{
	if (!alreadyDone)
		command->Do(m_model);

	m_commands.erase(m_commands.begin() + m_index, m_commands.end());
	m_commands.push_back(std::move(command));
	m_index = m_commands.size();
}

void CommandStack::Undo()
{
	KERNEL_ASSERT(CanUndo());
	m_commands[--m_index]->Undo(m_model);
}

void CommandStack::Redo()
{
	KERNEL_ASSERT(CanRedo());
	m_commands[m_index++]->Redo(m_model);
}

bool CommandStack::CanUndo() const
{
	return m_index > 0;
}

bool CommandStack::CanRedo() const
{
	return m_index < m_commands.size();
}
