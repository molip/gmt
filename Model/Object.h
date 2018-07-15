#pragma once

#include "../RenderContext.h"

#include "libKernel/Dynamic.h"

namespace GMT::Model 
{
	class Object : public Kernel::Dynamic
	{
	public:
		Object();
		virtual ~Object();

		virtual void Draw(RenderContext& rc) const = 0;
		virtual void Update() const {}
	};
}
