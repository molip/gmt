#pragma once

#include "../RenderContext.h"

namespace GMT::Model
{
	class Object
	{
	public:
		Object();
		virtual ~Object();

		virtual void Draw(RenderContext& rc) const = 0;
	};
}
