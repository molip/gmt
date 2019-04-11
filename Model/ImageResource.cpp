#include "ImageResource.h"

#include "../RenderContext.h"

#include "libKernel/Debug.h"
#include "libKernel/Util.h"

#include <SFML/Graphics.hpp>

#include <set>

using namespace GMT;
using namespace GMT::Model;

namespace fs = std::filesystem;

ImageResource::ImageResource(const std::string& name, const std::vector<std::wstring>& paths) : m_name(name)
{
	// Example filename: "00 StateName 00.png"
	// First number is the state index, used to order the states in the UI. Can be omitted, but order will be undefined. All images for a given state should have the same index. 
	// Second number is the frame index. Can be omitted for single-frame states.

	std::map<int, std::string> indexedStates; // stateIndex : id.
	std::set<std::string> unindexedStates;
	std::map<std::string, std::map<int, std::wstring>> states; // id : imageIndex : path.
	std::map<std::string, std::string> displayNames; // id : display name.

	for (auto& item : paths)
	{
		fs::path path(item);
		auto ext = Kernel::ToLower(path.extension());
		if (ext == L".png" || ext == L".jpg")
		{
			const auto stem = Kernel::WStringToString(path.stem());
			auto it = stem.begin();
			std::string stateIndexString, name, imageIndexString;

			auto isSeparator = [](const char& c) { return c == ' '; };
			auto isDigit = [](const char& c) { return c >= '0' && c <= '9'; };

			for (; it != stem.end() && isDigit(*it); ++it)
				stateIndexString += *it;

			for (; it != stem.end() && isSeparator(*it); ++it);

			for (; it != stem.end() && !isSeparator(*it); ++it)
				name += *it;

			for (; it != stem.end() && isSeparator(*it); ++it);

			for (; it != stem.end() && isDigit(*it); ++it)
				imageIndexString += *it;

			const auto id = Kernel::ToLower(name);

			auto&[stateIt, isNew] = states.try_emplace(id);
			
			// Only map the first path encountered for each state - assume the others match.
			if (isNew)
			{
				if (stateIndexString.empty())
					unindexedStates.insert(id);
				else
					indexedStates[std::stoi(stateIndexString)] = id;
				
				displayNames[id] = name;
			}

			const int imageIndex = imageIndexString.empty() ? 0 : std::stoi(imageIndexString);
			stateIt->second[imageIndex] = path;
		}
	}

	auto load = [&](const std::string& id)
	{
		TextureVec textures;
		for (auto&[index, path] : states[id])
		{
			auto texture = std::make_unique<sf::Texture>();
			if (texture->loadFromFile(Kernel::WStringToString(path)))
			{
				m_size = texture->getSize();
				textures.emplace_back(std::move(texture));
			}
		}

		if (!textures.empty())
			m_states.push_back({ id, displayNames[id], std::move(textures) });
	};

	for (auto&[index, id] : indexedStates)
		load(id);

	for (auto& id : unindexedStates)
		load(id);
}

void ImageResource::Draw(RenderContext& rc, const sf::RenderStates& renderStates) const
{
	rc.GetWindow().draw(sf::Sprite(GetTexture()), renderStates);
}

sf::Vector2u ImageResource::GetSize() const
{
	return GetTexture().getSize();
}

const sf::Texture& ImageResource::GetTexture(const std::string& stateID) const
{
	const State* state {};
	
	if (!stateID.empty())
		state = FindState(stateID);
	
	if (!state)
		state = &m_states.front();

	return *state->textures[0].get();
}

ImageResource::State* ImageResource::FindState(const std::string& stateID)
{
	auto it = std::find_if(m_states.begin(), m_states.end(), [&](auto& state) { return state.id == stateID; });
	return it == m_states.end() ? nullptr : &*it;
}



ImageResources& ImageResources::Instance()
{
	static ImageResources instance;
	return instance;
}

void ImageResources::Init(const std::wstring& rootPath)
{
	Instance().m_rootPath = rootPath;

	Instance().LoadType(Type::Floor, "Floors");
	Instance().LoadType(Type::Wall, "Walls");
	Instance().LoadType(Type::Object, "Objects");
}

void ImageResources::LoadType(Type type, const std::string& typeName)
{
	std::error_code ec;
	auto dit = fs::directory_iterator(m_rootPath / typeName, ec);
	if (ec)
		return;

	for (auto& item : dit)
	{
		const auto name = Kernel::WStringToString(item.path().stem());
		std::vector<std::wstring> paths;

		if (!item.is_directory())
			paths.push_back(item.path());
		else 
			for (auto& item2 : fs::directory_iterator(item.path()))
				paths.push_back(item2.path());

		auto image = std::make_unique<ImageResource>(name, paths);
		if (image->IsValid())
			m_maps[int(type)][Kernel::ToLower(name)] = std::move(image);
	}
}

const ImageResource* ImageResources::Get(Type type, const std::string& id)
{
	const auto& map = Instance().m_maps[int(type)];
	auto it = map.find(id);
	return it == map.end() ? nullptr : it->second.get();
}
