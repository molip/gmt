#include "App.h"
#include "Model/Command/Base.h"
#include "Model/CommandStack.h"
#include "Model/Model.h"
#include "Model/ImageObject.h"
	  
#include "libKernel/Filesystem.h"
#include "libKernel/Log.h"
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

void GMT::App::DoAutoSave()
{
	auto path = Get()->m_rootPath + ::AutosavePath + L"\\" + Kernel::StringToWString(Kernel::GetTimeStamp()) + L".gmt";
	bool ok = Get()->m_model->SaveFile(path);
	KERNEL_ASSERT(ok);
}

void GMT::App::AddCommand(Model::CommandPtr command, bool alreadyDone)
{
	Get()->m_commandStack->AddCommand(std::move(command), alreadyDone);
	DoAutoSave();
}

void GMT::App::Undo()
{
	Get()->m_commandStack->Undo();
	DoAutoSave();
}

void GMT::App::Redo()
{
	Get()->m_commandStack->Redo();
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
