#pragma once

#include "Base.h"

#include "Jig/EdgeMeshCommand.h"

namespace GMT::Model { class VectorObject; }

namespace GMT::Model::Command
{
	class EdgeMesh : public Base
	{
	public:
		using Ptr = std::unique_ptr<Jig::EdgeMeshCommand::Base>;
		EdgeMesh(const VectorObject& object);
		EdgeMesh(Ptr command, const VectorObject& object);
		EdgeMesh(Jig::EdgeMeshCommand::Compound::Vec&& commands, const VectorObject& object);

		virtual void Do(CommandContext& ctx) override;
		virtual void Undo(CommandContext& ctx) override;
		virtual void Redo(CommandContext& ctx) override;

		void UndoNoUpdate();

	protected:
		Jig::EdgeMesh& GetMesh() const;
			
		Ptr m_command;
		const VectorObject& m_object;
	};
}
