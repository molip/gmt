#pragma once

#include <vector>
#include <memory>

namespace GMT::Model 
{
	namespace Command
	{
		class Base;
	}
	
	class Model;
	using CommandPtr = std::unique_ptr<Command::Base>;

	class CommandStack
	{
	public:
		explicit CommandStack(Model& model);
		~CommandStack();

		void AddCommand(CommandPtr command, bool alreadyDone = false);
		void Undo();
		void Redo();
	
		bool CanUndo() const;
		bool CanRedo() const;

	private:
		Model& m_model;
		std::vector<CommandPtr> m_commands;
		size_t m_index = 0; // Index of next command.
	};
}
