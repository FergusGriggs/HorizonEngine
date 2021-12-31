#pragma once

#include <unordered_map>
#include <vector>

#include "texture.h"
#include "../shaders/shader.h"

namespace hrzn::gfx
{
    struct MaterialTexture
    {
        MaterialTexture();
        MaterialTexture(int bindSlot, Texture* texture);

        int      m_bindSlot;
        Texture* m_texture;
    };

    class Material
    {
    public:
        enum class TextureType
        {
            eAlbedo,
            eRoughness,
            eNormal,
            eMetallic,
            eEmission,
            eDepth,
            eNumTypes,
        };

    public:
        Material();
        ~Material();

        bool initialise(std::string name);

        void bind(bool useGBuffer, bool bindPSData);

        bool isGBufferCompatible() const;

        static Texture* getDefaultTexture(TextureType textureType);

    private:
        std::vector<MaterialTexture> m_textures;

        bool m_isGBufferCompatible;

        VertexShader* m_vertexShader;
        PixelShader*  m_pixelShader;
        PixelShader*  m_gBufferWritePixelShader;

        std::string   m_name;
    };

    static const std::unordered_map<std::string, Material::TextureType> sc_materialTextureTypeEnumMap = {
        { "albedo",     Material::TextureType::eAlbedo },
        { "roughness",  Material::TextureType::eRoughness },
        { "normal_map", Material::TextureType::eNormal },
        { "metallic",   Material::TextureType::eMetallic },
        { "emission",   Material::TextureType::eEmission },
        { "depth",      Material::TextureType::eDepth }
    };
}
