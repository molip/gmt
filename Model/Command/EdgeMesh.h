#pragma once

#include "Base.h"

#include "Jig/EdgeMeshCommand.h"

namespace GMT::Model { class Object; }

namespace GMT::Model::Command
{
	class EdgeMesh : public Base
	{
	public:
		using Ptr = std::unique_ptr<Jig::EdgeMeshCommand::Base>;
		EdgeMesh(const Object& object);
		EdgeMesh(Ptr command, const Object& object);
		EdgeMesh(Jig::EdgeMeshCommand::Compound::Vec&& commands, const Object& object);

		virtual void Do(Model& model) override;
		virtual void Undo(Model& model) override;
		virtual void Redo(Model& model) override;

		void UndoNoUpdate();

	protected:
		Ptr m_command;
		const Object& m_object;
	};
}
