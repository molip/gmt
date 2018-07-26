#include "AddObject.h"
#include "../Model.h"
#include "../Selection.h"
#include "../VectorObject.h"

using namespace GMT::Model;
using namespace GMT::Model::Command;

AddObject::AddObject(ObjectPtr object) : m_object(std::move(object))
{
}

void AddObject::Do(CommandContext& ctx)
{
	ctx.GetModel().PushObject(std::move(m_object));
}

void AddObject::Undo(CommandContext& ctx)
{
	m_object = ctx.GetModel().PopObject();

	if (auto* vector = dynamic_cast<VectorObject*>(m_object.get()))
	{
		for (auto& vert : vector->GetMesh().GetVerts())
			ctx.Deselect(Selection(*vert));
	}
}
