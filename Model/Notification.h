#pragma once

#include "Jig/EdgeMesh.h"

namespace GMT::Model::Notification
{
	class Base
	{
	public:
		virtual ~Base();
	};

	class SelectionChanged : public Base
	{
	};
}
