#include "CommandStack.h"
#include "Selection.h"

#include "Command/Base.h"

#include "libKernel/Debug.h"

using namespace GMT::Model;

CommandStack::CommandStack(Model& model) : m_model(model) {}

CommandStack::~CommandStack() = default;

void CommandStack::Clear()
{
	m_index = 0;
	m_commands.clear();
}

SelectionPtr CommandStack::DoCommand(Command::Base& command, const Selection& selection)
{
	Command::Base::CommandContext ctx(m_model, selection);
	command.Do(ctx);
	return ctx.HarvestSelection();
}

SelectionPtr CommandStack::AddCommand(CommandPtr command, const Selection& selection, bool alreadyDone)
{
	Command::Base::CommandContext ctx(m_model, selection);

	if (!alreadyDone)
		command->Do(ctx);

	m_commands.erase(m_commands.begin() + m_index, m_commands.end());
	m_commands.push_back(std::move(command));
	m_index = m_commands.size();

	return ctx.HarvestSelection();
}

SelectionPtr CommandStack::Undo(const Selection& selection)
{
	KERNEL_ASSERT(CanUndo());
	Command::Base::CommandContext ctx(m_model, selection);
	m_commands[--m_index]->Undo(ctx);
	return ctx.HarvestSelection();
}

SelectionPtr CommandStack::Redo(const Selection& selection)
{
	KERNEL_ASSERT(CanRedo());
	Command::Base::CommandContext ctx(m_model, selection);
	m_commands[m_index++]->Redo(ctx);
	return ctx.HarvestSelection();
}

bool CommandStack::CanUndo() const
{
	return m_index > 0;
}

bool CommandStack::CanRedo() const
{
	return m_index < m_commands.size();
}
