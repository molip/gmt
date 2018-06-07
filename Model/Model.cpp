#include "Model.h"
#include "ImageObject.h"

using namespace GMT;
using namespace GMT::Model;

GMT::Model::Model::Model()
{
	AddObject(std::make_unique<ImageObject>());
}

GMT::Model::Model::~Model() = default;

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
