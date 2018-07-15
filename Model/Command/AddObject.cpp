#include "AddObject.h"
#include "../Model.h"
#include "../Object.h"

using namespace GMT::Model;
using namespace GMT::Model::Command;

AddObject::AddObject(ObjectPtr object) : m_object(std::move(object))
{
}

void AddObject::Do(Model& model)
{
	model.PushObject(std::move(m_object));
}

void AddObject::Undo(Model& model)
{
	m_object = model.PopObject();
}
