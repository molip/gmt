#pragma once

#include "Model/CommandStack.h"

#include "libraries/libKernel/Singleton.h"

#include <memory>

namespace GMT::Model
{
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
		static Model::CommandStack& GetCommandStack() { return *Get()->m_commandStack; }

	private:
		std::unique_ptr<Model::Model> m_model;
		std::unique_ptr<Model::CommandStack> m_commandStack;
	};
}
