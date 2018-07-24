#pragma once

#include "libraries/libKernel/Singleton.h"

#include <ctime>
#include <iomanip>
#include <memory>

namespace Kernel
{
	class MessageBox;
}

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
	class Window;

	class App : public Kernel::Singleton<App>
	{
	public:
		App();
		~App();

		static void SetMainWindow(Window* window);

		static const Model::Model& GetModel() { return *Get()->m_model; }
		static void DoAutoSave();

		static void DoCommand(Model::Command::Base& command);
		static void AddCommand(Model::CommandPtr command, bool alreadyDone = false);
		static void Undo();
		static void Redo();

		static bool CanUndo();
		static bool CanRedo();

		static bool New() { return Get()->_New(); }
		static bool Save() { return Get()->_Save(); }
		static bool SaveAs() { return Get()->_SaveAs(); }
		static bool Load() { return Get()->_Load(); }
		static bool PreClose() { return Get()->PreDiscardModel(); }

	private:
		std::wstring DoFileDialog(bool save);
		bool PreDiscardModel(); // Return true to discard.
		void* GetMainWindowHandle() const;
			
		bool _New();
		bool _Load();
		bool _Save();
		bool _SaveAs();

		std::unique_ptr<Model::Model> m_model;
		std::unique_ptr<Model::CommandStack> m_commandStack;
		std::wstring m_rootPath;
		std::wstring m_modelPath;
		bool m_isModelDirty{};
		Window* m_mainWindow{};
		std::unique_ptr<Kernel::MessageBox> m_messageBox;
	};
}
