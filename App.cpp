#include "App.h"
#include "Model/Model.h"
#include "Model/ImageObject.h"

using namespace GMT;

SINGLETON(App)

GMT::App::App()
{
	m_model = std::make_unique<Model::Model>();

	if (!m_model->LoadFile())
		m_model->AddObject(std::make_unique<Model::ImageObject>());
}

GMT::App::~App() = default;
