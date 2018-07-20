#pragma once

#include "libraries/libKernel/Singleton.h"

#include <ctime>
#include <iomanip>
#include <memory>

namespace GMT::Model
{
	namespace Command
	{
		class Base;
	}

	using CommandPtr = std::unique_ptr<Command::Base>;
	class CommandStack;
	class Model;
}

namespace GMT
{
	class App : public Kernel::Singleton<App>
	{
	public:
		App();
		~App();

		static const Model::Model& GetModel() { return *Get()->m_model; }
		static void DoAutoSave();

		static void AddCommand(Model::CommandPtr command, bool alreadyDone = false);
		static void Undo();
		static void Redo();

		static bool CanUndo();
		static bool CanRedo();

	private:
		std::unique_ptr<Model::Model> m_model;
		std::unique_ptr<Model::CommandStack> m_commandStack;
		std::wstring m_rootPath;
	};
}
