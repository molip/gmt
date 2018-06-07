#pragma once

#include <memory>
#include <vector>

namespace GMT
{
	class RenderContext;
}

namespace GMT::Model
{
	class Object;

	class Model
	{
	public:
		using ObjectPtr = std::unique_ptr<Object>;
		using Objects = std::vector<ObjectPtr>;
		
		Model();
		~Model();

		void Draw(RenderContext& rc) const;

		const Objects& GetObjects() const { return m_objects; }
		Object& AddObject(ObjectPtr object);

	private:
		std::vector<std::unique_ptr<Object>> m_objects;
	};
}
