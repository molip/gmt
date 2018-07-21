#pragma once

#include "libKernel/Serial.h"

#include <memory>
#include <vector>

namespace GMT
{
	class RenderContext;
}

namespace GMT::Model
{
	class Object;
	using ObjectPtr = std::unique_ptr<Object>;

	class Model
	{
	public:
		using ObjectPtr = std::unique_ptr<Object>;
		using Objects = std::vector<ObjectPtr>;
		
		~Model();

		void Clear() { m_objects.clear(); }

		bool LoadFile(const std::wstring& path);
		bool SaveFile(const std::wstring& path) const;

		void Load(const Kernel::Serial::LoadNode& node);
		void Save(Kernel::Serial::SaveNode& node) const;

		void Draw(RenderContext& rc) const;

		const Objects& GetObjects() const { return m_objects; }
		void PushObject(ObjectPtr object);
		ObjectPtr PopObject();

	private:
		std::vector<std::unique_ptr<Object>> m_objects;
	};
}
