#pragma once

#include <SFML/System/Vector2.hpp>

#include <filesystem>
#include <map>
#include <memory>

#include "libKernel/Serial.h"

namespace sf
{
	class RenderStates;
	class Texture;
}

namespace GMT
{
	class RenderContext;
}

namespace GMT::Model
{
	class ImageResource
	{
	public:
		enum class Type { Floor, Wall, Object, _Count };

		ImageResource(const std::string& name, const std::vector<std::wstring>& paths);

		void Draw(RenderContext& rc, const sf::RenderStates& renderStates) const;

		bool IsValid() const { return !m_states.empty(); }
		bool IsSurface() const { return true; }
		bool CanFlip() const { return !IsSurface(); }
		sf::Vector2u GetSize() const;
		const sf::Texture& GetTexture(const std::string& stateID = {}) const;

	protected:
		using TextureVec = std::vector<std::unique_ptr<sf::Texture>>;
		struct State { std::string id, displayName; TextureVec textures; };
		using StateVec = std::vector<State>;

		const State* FindState(const std::string& stateID) const { return const_cast<ImageResource*>(this)->FindState(stateID); }
		State* FindState(const std::string& stateID);

		const std::string& m_name;
		StateVec m_states;
		sf::Vector2u m_size;
	};

	class ImageResources
	{
		using Type = ImageResource::Type;
	public:
		static ImageResources& Instance();
		static void Init(const std::wstring& rootPath);
		static const ImageResource* Get(Type type, const std::string& id);

	private:
		void LoadType(Type type, const std::string& typeName);

		using Map = std::map<std::string, std::unique_ptr<ImageResource>>;
		Map m_maps[int(Type::_Count)];
		std::filesystem::path m_rootPath;
	};

	class ImageResourceRef
	{
	public:
		explicit ImageResourceRef(ImageResource::Type type) : m_type(type) {}
		
		void Draw(RenderContext& rc, const sf::RenderStates& renderStates) const { if (m_resource) m_resource->Draw(rc, renderStates); } // TODO: draw placeholder.
		sf::Vector2u GetSize() const { return m_resource ? m_resource->GetSize() : sf::Vector2u(); }
		const sf::Texture* GetTexture() const { return m_resource ? &m_resource->GetTexture() : nullptr; }

		void SetID(const std::string& id) { m_id = id; Update(); }

		void Load(const Kernel::Serial::LoadNode& node)
		{
			node.LoadType("id", m_id);
			Update();
		}

		void Save(Kernel::Serial::SaveNode& node) const
		{
			node.SaveType("id", m_id);
		}

	private:
		void Update()
		{
			m_resource = ImageResources::Get(m_type, m_id);
		}
		
		const ImageResource::Type m_type;
		std::string m_id;
		const ImageResource* m_resource{};
	};

	class ObjectImageResourceRef : public ImageResourceRef
	{ 
	public:
		ObjectImageResourceRef() : ImageResourceRef(ImageResource::Type::Object) {}
	};
}
