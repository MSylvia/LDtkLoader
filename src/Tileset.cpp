// Created by Modar Nasser on 11/11/2020.

#include "LDtkLoader/Tileset.hpp"
#include "LDtkLoader/World.hpp"

using namespace ldtk;

Tileset::Tileset(const nlohmann::json& j, World* w) :
name(j["identifier"].get<std::string>()),
uid(j["uid"].get<int>()),
path(j["relPath"].get<std::string>()),
texture_size({j["pxWid"].get<int>(), j["pxHei"].get<int>()}),
tile_size(j["tileGridSize"].get<int>()),
spacing(j["spacing"].get<int>()),
padding(j["padding"].get<int>()),
m_tags_enum(j["tagsSourceEnumUid"].is_null() ? nullptr : &w->getEnum(j["tagsSourceEnumUid"].get<int>()))
{
    // parse tiles custom data
    m_custom_data[-1] = "";
    m_custom_data.reserve(j["customData"].size());
    for (auto& data : j["customData"]) {
        m_custom_data[data["tileId"].get<int>()] = data["data"].get<std::string>();
    }

    // parse tiles enum tags
    if (m_tags_enum != nullptr) {
        for (const auto& tag : j["enumTags"]) {
            auto enumval_name = tag["enumValueId"].get<std::string>();
            auto& tiles = tag["tileIds"];
            m_tag_tiles_map[enumval_name].reserve(tiles.size());
            for (auto& tile_id : tiles) {
                m_tag_tiles_map[enumval_name].push_back(tile_id.get<int>());
            }
        }
    }

}

auto Tileset::getTileTexturePos(int tile_id) const -> IntPoint {
    auto grid_width = texture_size.x / tile_size;
    return {
        padding + (tile_id % grid_width) * (tile_size + spacing),
        padding + (tile_id / grid_width) * (tile_size + spacing)
    };
}

auto Tileset::getTileData(int tile_id) const -> const std::string& {
    if (m_custom_data.count(tile_id) > 0)
        return m_custom_data.at(tile_id);
    return m_custom_data.at(-1);
}

auto Tileset::hasTags() -> bool {
    return m_tags_enum != nullptr;
}

auto Tileset::getTagsEnum() -> const Enum& {
    return *m_tags_enum;
}

auto Tileset::getTilesWithTag(const EnumValue& enumvalue) const -> const std::vector<int>& {
    if (enumvalue.type.uid != m_tags_enum->uid)
        ldtk_error("Enum value \""+enumvalue.name+"\" is not a value of Enum \""+m_tags_enum->name+"\".");

    return m_tag_tiles_map.at(enumvalue.name);
}
