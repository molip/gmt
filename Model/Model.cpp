#include "Model.h"
#include "ImageObject.h"

#include "libKernel/Log.h"

using namespace GMT;
using namespace GMT::Model;

namespace Serial = Kernel::Serial;

GMT::Model::Model::~Model() = default;

bool GMT::Model::Model::LoadFile(const std::wstring& path)
{
	Kernel::Log() << "Loading model: " << path << std::endl;
	Model model;
	
	bool ok = Kernel::Serial::LoadClass(path, model);
	Kernel::Log(1) << "Result: " << ok << std::endl;

	if (ok)
	{
		m_objects = std::move(model.m_objects);
		return true;
	}

	return false;
}

bool GMT::Model::Model::SaveFile(const std::wstring& path) const
{
	Kernel::Log() << "Saving model: " << path << std::endl;
	bool ok = Kernel::Serial::SaveClass(path, *this);
	Kernel::Log(1) << "Result: " << ok << std::endl;
	return ok;
}

void GMT::Model::Model::Load(const Kernel::Serial::LoadNode& node)
{
	Kernel::Serial::LoadContext ctx(node);
	node.LoadCntr("objects", m_objects, Serial::ObjectLoader());
	ctx.ResolveRefs();
}

void GMT::Model::Model::Save(Kernel::Serial::SaveNode& node) const
{
	Kernel::Serial::SaveContext ctx(node);
	node.SaveCntr("objects", m_objects, Serial::ObjectSaver());
}

void GMT::Model::Model::Draw(RenderContext& rc) const
{
	for (auto& object : m_objects)
		object->Draw(rc);
}

void GMT::Model::Model::PushObject(ObjectPtr object)
{
	m_objects.push_back(std::move(object));
}

ObjectPtr GMT::Model::Model::PopObject()
{
	ObjectPtr object = std::move(m_objects.back());
	m_objects.pop_back();
	return object;
}
