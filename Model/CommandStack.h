#pragma once

#include <vector>
#include <memory>

namespace GMT::Model 
{
	namespace Command
	{
		class Base;
	}

	namespace Notification
	{
		class Base;
	}

	class Model;
	class Selection;
	using CommandPtr = std::unique_ptr<Command::Base>;
	using SelectionPtr = std::unique_ptr<Selection>;

	class CommandStack
	{
	public:
		CommandStack(Model& model);
		~CommandStack();
		
		void Clear();

		SelectionPtr DoCommand(Command::Base& command, const Selection& selection);
		SelectionPtr AddCommand(CommandPtr command, const Selection& selection, bool alreadyDone = false);
		SelectionPtr Undo(const Selection& selection);
		SelectionPtr Redo(const Selection& selection);
	
		bool CanUndo() const;
		bool CanRedo() const;

	private:
		Model& m_model;
		std::vector<CommandPtr> m_commands;
		size_t m_index = 0; // Index of next command.
	};
}
