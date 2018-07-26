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

		virtual void Do(CommandContext& ctx) override;
		virtual void Undo(CommandContext& ctx) override;
		virtual void Redo(CommandContext& ctx) override;

		void UndoNoUpdate();

	protected:
		Ptr m_command;
		const Object& m_object;
	};
}
