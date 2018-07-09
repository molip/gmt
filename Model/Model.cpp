#include "Model.h"
#include "ImageObject.h"

using namespace GMT;
using namespace GMT::Model;

namespace Serial = Kernel::Serial;

namespace
{
	const std::wstring TestPath = L"D:\\Users\\jxxwh\\Documents\\GMT\\Test.xml";
	//const std::wstring TestPath = L"E:\\Users\\Jon\\Documents\\GMT\\Test.xml";
}

GMT::Model::Model::Model()
{
}

GMT::Model::Model::~Model() = default;

bool GMT::Model::Model::LoadFile()
{
	Model model;
	if (Kernel::Serial::LoadClass(TestPath, model))
	{
		m_objects = std::move(model.m_objects);
		return true;
	}

	return false;
}

void GMT::Model::Model::SaveFile() const
{
	Kernel::Serial::SaveClass(TestPath, *this);
}

void GMT::Model::Model::Load(const Kernel::Serial::LoadNode& node)
{
	node.LoadCntr("objects", m_objects, Serial::ObjectLoader());
}

void GMT::Model::Model::Save(Kernel::Serial::SaveNode& node) const
{
	node.SaveCntr("objects", m_objects, Serial::ObjectSaver());
}

void GMT::Model::Model::Draw(RenderContext& rc) const
{
	for (auto& object : m_objects)
		object->Draw(rc);
}

Object& GMT::Model::Model::AddObject(ObjectPtr object)
{
	m_objects.push_back(std::move(object));
	return *m_objects.back();
}
