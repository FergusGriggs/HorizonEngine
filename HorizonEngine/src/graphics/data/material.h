#pragma once

#include <unordered_map>
#include <vector>

#include "texture.h"
#include "../shaders/pixel_shader.h"
#include "../shaders/vertex_shader.h"

namespace hrzn::gfx
{
    enum class MaterialTextureType
    {
        eAlbedo,
        eRoughness,
        eNormal,
        eMetallic,
        eEmission,
        eDepth,

        eNumTypes,
    };

    struct MaterialTexture
    {
        MaterialTexture();
        MaterialTexture(MaterialTextureType textureType, Texture* texture);
        MaterialTexture(int bindSlot, Texture* texture);

        int      m_bindSlot;
        Texture* m_texture;
    };

    class Material
    {
    public:
        Material();
        ~Material();

        bool initialiseFromName(std::string name);
        bool initialiseFromTextures(const std::vector<MaterialTexture>& textures);

        void bind(bool useGBuffer, bool bindPSData);

        const std::string&                  getName() const;
        const std::vector<MaterialTexture>& getTextures() const;

        bool isGBufferCompatible() const;

        static Texture* getDefaultTexture(MaterialTextureType textureType);

    private:
        std::vector<MaterialTexture> m_textures;

        bool m_isGBufferCompatible;

        VertexShader* m_vertexShader;
        PixelShader*  m_pixelShader;
        PixelShader*  m_gBufferWritePixelShader;

        std::string   m_name;
    };

    static const std::unordered_map<std::string, MaterialTextureType> sc_materialTextureTypeEnumMap = {
        { "albedo",     MaterialTextureType::eAlbedo },
        { "roughness",  MaterialTextureType::eRoughness },
        { "normal_map", MaterialTextureType::eNormal },
        { "metallic",   MaterialTextureType::eMetallic },
        { "emission",   MaterialTextureType::eEmission },
        { "depth",      MaterialTextureType::eDepth }
    };
}
