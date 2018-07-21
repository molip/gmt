#include "App.h"
#include "Window.h"
#include "Model/Command/Base.h"
#include "Model/CommandStack.h"
#include "Model/Model.h"
#include "Model/ImageObject.h"
	  
#include "libKernel/FileDialog.h"
#include "libKernel/Filesystem.h"
#include "libKernel/Log.h"
#include "libKernel/MessageBox.h"
#include "libKernel/Util.h"

using namespace GMT;

SINGLETON(App)

namespace
{
	auto RootPath = L"\\GMT";
	auto LogPath = L"\\Log";
	auto AutosavePath = L"\\Autosave";
}

GMT::App::App()
{
	m_rootPath = Kernel::FileSystem::GetUserDocumentsDir();
	KERNEL_VERIFY(!m_rootPath.empty());
	m_rootPath += RootPath;
	KERNEL_VERIFY(Kernel::FileSystem::CreateDir(m_rootPath));

	KERNEL_VERIFY(Kernel::FileSystem::CreateDir(m_rootPath + ::LogPath));
	KERNEL_VERIFY(Kernel::Log().Init(m_rootPath + ::LogPath + L"\\" + Kernel::StringToWString(Kernel::GetTimeStamp()) + L".log"));

	KERNEL_VERIFY(Kernel::FileSystem::CreateDir(m_rootPath + ::AutosavePath));

	m_model = std::make_unique<Model::Model>();
	m_commandStack = std::make_unique<Model::CommandStack>(*m_model);
}

GMT::App::~App() = default;

void GMT::App::SetMainWindow(Window* window) 
{
	Get()->m_mainWindow = window; 
	Get()->m_messageBox = std::make_unique<Kernel::MessageBox>(window->getSystemHandle(), "GMT");
}

void GMT::App::DoAutoSave()
{
	auto path = Get()->m_rootPath + ::AutosavePath + L"\\" + Kernel::StringToWString(Kernel::GetTimeStamp()) + L".gmt";
	bool ok = Get()->m_model->SaveFile(path);
	KERNEL_ASSERT(ok);
}

void GMT::App::AddCommand(Model::CommandPtr command, bool alreadyDone)
{
	Get()->m_commandStack->AddCommand(std::move(command), alreadyDone);
	Get()->m_isModelDirty = true;
	DoAutoSave();
}

void GMT::App::Undo()
{
	Get()->m_commandStack->Undo();
	Get()->m_isModelDirty = true;
	DoAutoSave();
}

void GMT::App::Redo()
{
	Get()->m_commandStack->Redo();
	Get()->m_isModelDirty = true;
	DoAutoSave();
}

bool GMT::App::CanUndo()
{
	return Get()->m_commandStack->CanUndo();
}

bool GMT::App::CanRedo()
{
	return Get()->m_commandStack->CanRedo();
}

std::wstring GMT::App::DoFileDialog(bool save)
{	
	using Kernel::FileDialog;
	FileDialog dlg(save ? FileDialog::Type::Save : FileDialog::Type::Open, GetMainWindowHandle());
	dlg.AddFileTypeFilter("GMT files", "*.gmt");
	dlg.AddAllTypesFilter();
	dlg.SetDefaultExtension("gmt");

	return dlg.Show() ? dlg.GetPath() : std::wstring();
}

bool GMT::App::PreDiscardModel()
{
	if (!m_isModelDirty)
		return true;

	std::string msg = (m_modelPath.empty() ? std::string("<untitled>") : Kernel::WStringToString(m_modelPath)) + '\n';

	msg += "Save changes?";

	using Kernel::MessageBox;
	switch (m_messageBox->Show(msg.c_str(), MessageBox::Type::YesNoCancel))
	{
	case MessageBox::Button::Yes:
		return _Save();
	case MessageBox::Button::No:
		return true;
	}

	return false;
}

void* GMT::App::GetMainWindowHandle() const
{
	return m_mainWindow->getSystemHandle();
}

bool GMT::App::_New()
{
	if (!PreDiscardModel())
		return false;

	m_model->Clear();
	m_commandStack->Clear();
	m_modelPath.clear();
	m_isModelDirty = false;
	return true;
}

bool GMT::App::_Save()
{
	if (m_modelPath.empty())
		return _SaveAs();

	if (!m_model->SaveFile(m_modelPath))
	{
		m_messageBox->Show("Error!", Kernel::MessageBox::Type::OK);
		return false;
	}

	m_isModelDirty = false;
	return true;
}

bool GMT::App::_SaveAs()
{
	auto path = DoFileDialog(true);
	if (path.empty())
		return false;

	if (!m_model->SaveFile(path))
	{
		m_messageBox->Show("Error!", Kernel::MessageBox::Type::OK);
		return false;
	}

	m_modelPath = path;
	m_isModelDirty = false;
	return true;
}

bool GMT::App::_Load()
{
	if (!PreDiscardModel())
		return false;

	auto path = DoFileDialog(false);
	if (path.empty())
		return false;

	if (!m_model->LoadFile(path))
	{
		m_messageBox->Show("Error!", Kernel::MessageBox::Type::OK);
		return false;
	}

	m_commandStack->Clear();
	m_modelPath = path;
	m_isModelDirty = false;
	return true;
}
