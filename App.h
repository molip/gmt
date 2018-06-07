#pragma once

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

		static Model::Model& GetModel() { return *Get()->m_model; }

	private:
		std::unique_ptr<Model::Model> m_model;
	};
}
