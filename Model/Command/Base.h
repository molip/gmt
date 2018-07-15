#pragma once

#include <memory>

namespace GMT::Model
{
	class Model;
}

namespace GMT::Model::Command
{
	class Base
	{
	public:
		virtual ~Base();

		virtual void Do(Model& model) = 0;
		virtual void Undo(Model& model) = 0;
		virtual void Redo(Model& model) { Do(model); }
	};

	using BasePtr = std::unique_ptr<Base>;
}
