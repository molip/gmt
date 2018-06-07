#include "App.h"
#include "Model/Model.h"

using namespace GMT;

SINGLETON(App)

GMT::App::App()
{
	m_model = std::make_unique<Model::Model>();
}

GMT::App::~App() = default;
