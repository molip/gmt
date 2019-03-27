#pragma once

#include "libraries/libKernel/Singleton.h"

#include <memory>
#include <vector>

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

	namespace Notification
	{
		class Base;
	}

	class CommandStack;
	class Model;
	class Selection;
	using CommandPtr = std::unique_ptr<Command::Base>;
	using SelectionPtr = std::unique_ptr<Selection>;
}

namespace GMT
{
	class Window;

	class App : public Kernel::Singleton<App>
	{
	public:
		App(const std::vector<std::wstring>& args);
		~App();

		static void SetMainWindow(Window* window);

		static const Model::Model& GetModel() { return *Get()->m_model; }
		static const Model::Selection& GetSelection() { return *Get()->m_selection; }
		static void SetSelection(const Model::Selection& selection);
		static void ClearSelection();
		static void DoAutoSave();

		static void DoCommand(Model::Command::Base& command) { Get()->_DoCommand(command); }
		static void AddCommand(Model::CommandPtr command, bool alreadyDone = false);
		static void Undo() { Get()->_Undo(); }
		static void Redo() { Get()->_Redo(); }

		static bool CanUndo();
		static bool CanRedo();

		static bool New() { return Get()->_New(); }
		static bool Save() { return Get()->_Save(); }
		static bool SaveAs() { return Get()->_SaveAs(); }
		static bool Load() { return Get()->_Load(); }
		static bool PreClose() { return Get()->PreDiscardModel(); }

	private:
		void ResaveDocuments();
		std::wstring DoFileDialog(bool save);
		bool PreDiscardModel(); // Return true to discard.
		void* GetMainWindowHandle() const;
		void Notify(const Model::Notification::Base& notification);
		void UpdateSelection(Model::SelectionPtr selection);

		bool _New();
		bool _Load();
		bool _Save();
		bool _SaveAs();
		void _DoCommand(Model::Command::Base& command);
		void _AddCommand(Model::CommandPtr command, bool alreadyDone = false);
		void _Undo();
		void _Redo();

		std::unique_ptr<Model::Model> m_model;
		std::unique_ptr<Model::CommandStack> m_commandStack;
		Model::SelectionPtr m_selection;
		std::wstring m_rootPath;
		std::wstring m_modelPath;
		bool m_isModelDirty{};
		Window* m_mainWindow{};
		std::unique_ptr<Kernel::MessageBox> m_messageBox;
	};
}
