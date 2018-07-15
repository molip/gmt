#pragma once

#include "Base.h"

namespace GMT::Model
{
	class Object;
	using ObjectPtr = std::unique_ptr<Object>;
}

namespace GMT::Model::Command
{
	class AddObject : public Base
	{
	public:
		AddObject(ObjectPtr object);

		virtual void Do(Model& model) override;
		virtual void Undo(Model& model) override;

	private:
		ObjectPtr m_object;
	};
}
