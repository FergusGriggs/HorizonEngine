#include "material.h"

#include "resource_manager.h"

#include "../graphics_handler.h"

#include "../../utils/json_helpers.h"

namespace hrzn::gfx
{
    MaterialTexture::MaterialTexture() :
        m_bindSlot(-1),
        m_texture(nullptr)
    {
    }

    MaterialTexture::MaterialTexture(MaterialTextureType textureType, Texture* texture) :
        m_bindSlot((int)textureType),
        m_texture(texture)
    {
    }

    MaterialTexture::MaterialTexture(int bindSlot, Texture* texture) :
        m_bindSlot(bindSlot),
        m_texture(texture)
    {
    }

    Material::Material() :
        m_isGBufferCompatible(false),

        m_vertexShader(nullptr),
        m_pixelShader(nullptr),
        m_gBufferWritePixelShader(nullptr)
    {
    }

    Material::~Material()
    {
    }

    bool Material::initialiseFromName(std::string name)
    {
        m_name = name;

        std::string materialPath = "res/materials/" + name + ".json";
        rapidjson::Document document;
        if (utils::JSONHelpers::parse(materialPath, document) == scene::SceneLoader::LoadResult::eFailed)
        {
            return false;
        }

        if (!document.HasMember("Material"))
        {
            return false;
        }

        rapidjson::Value& materialObject = document["Material"];

        // Should default textures?
        bool defaultMaterialTextures = true;
        if (materialObject.HasMember("default_material_textures"))
        {
            defaultMaterialTextures = materialObject["default_material_textures"].GetBool();
        }

        // Load/link specified textures
        if (materialObject.HasMember("textures"))
        {
            rapidjson::Value& textures = materialObject["textures"];

            for (auto textureItr = textures.Begin(); textureItr != textures.End(); ++textureItr)
            {
                MaterialTexture materialTexture;

                if (textureItr->IsArray() && textureItr->GetArray().Size() == 2)
                {
                    // Load texture slot
                    rapidjson::Value& textureSlot = textureItr->GetArray()[0];
                    if (textureSlot.IsString())
                    {
                        const auto& textureTypeItr = sc_materialTextureTypeEnumMap.find(textureSlot.GetString());
                        if (textureTypeItr != sc_materialTextureTypeEnumMap.end())
                        {
                            materialTexture.m_bindSlot = static_cast<int>(textureTypeItr->second);
                        }
                        else
                        {
                            utils::ErrorLogger::log("Parsing material file '" + name + "'\n'texture slot' string was not a valid texture name");
                            return false;
                        }

                    }
                    else if (textureSlot.IsInt())
                    {
                        materialTexture.m_bindSlot = textureSlot.GetInt();
                    }
                    else
                    {
                        utils::ErrorLogger::log("Parsing material file '" + name + "'\n'texture slot' is not a slot str ID or slot number");
                        return false;
                    }

                    // Load texture data
                    rapidjson::Value& textureData = textureItr->GetArray()[1];
                    if (textureData.IsString())
                    {
                        std::string textureDataString = textureData.GetString();
                        // Engine texture
                        if (textureDataString[0] == '$')
                        {
                            std::string genTextureName = textureDataString.substr(1);
                            /*if (genTextureName == "noise3d")
                            {
                                GraphicsHandler::it().get3DNoiseTexture();
                            }*/
                        }
                        else
                        {
                            materialTexture.m_texture = ResourceManager::it().getTexturePtr(textureDataString);
                        }
                    }
                    else if (textureData.IsArray() && textureData.GetArray().Size() == 3)
                    {
                        Colour colour = utils::JSONHelpers::getColourFromArray(textureData.GetArray());
                        materialTexture.m_texture = ResourceManager::it().getColourTexturePtr(colour);
                    }
                    else if (textureData.IsFloat() || textureData.IsInt())
                    {
                        BYTE byteColour = utils::JSONHelpers::getByteColourFromValue(textureData);
                        Colour colour = Colour(byteColour, byteColour, byteColour);
                        materialTexture.m_texture = ResourceManager::it().getColourTexturePtr(colour);
                    }
                    else
                    {
                        utils::ErrorLogger::log("Parsing material file '" + name + "'\n'texture data' is not a valid type");
                        return false;
                    }

                    if (materialTexture.m_texture == nullptr)
                    {
                        utils::ErrorLogger::log("Parsing material file '" + name + "'\nTexture loaded was nullptr");
                        return false;
                    }
                    else if (materialTexture.m_bindSlot < 0)
                    {
                        utils::ErrorLogger::log("Parsing material file '" + name + "'\nTexture loaded has a negative bind slot");
                        return false;
                    }

                    m_textures.push_back(materialTexture);
                }
            }
        }

        // Fill default textures
        if (defaultMaterialTextures)
        {
            for (int textureSlot = 0; textureSlot < static_cast<int>(MaterialTextureType::eNumTypes); ++textureSlot)
            {
                bool needsDefault = true;
                for (int textureIndex = 0; textureIndex < m_textures.size(); ++textureIndex)
                {
                    if (m_textures[textureIndex].m_bindSlot == textureSlot)
                    {
                        needsDefault = false;
                        break;
                    }
                }

                if (needsDefault)
                {
                    m_textures.push_back(MaterialTexture(textureSlot, getDefaultTexture(static_cast<MaterialTextureType>(textureSlot))));
                }
            }
        }

        // Load shaders
        // Vertex Shader
        if (materialObject.HasMember("vertex_shader"))
        {
            rapidjson::Value& vertexShader = materialObject["vertex_shader"];
            if (vertexShader.IsString())
            {
                m_vertexShader = ResourceManager::it().getVSPtr(vertexShader.GetString());
            }
            else
            {
                utils::ErrorLogger::log("Parsing material file '" + name + "'\n'vertex_shader' was not a string id");
            }
        }
        else
        {
            utils::ErrorLogger::log("Parsing material file '" + name + "'\n'vertex_shader' was not set");
            return false;
        }

        // Pixel Shader
        if (materialObject.HasMember("pixel_shader"))
        {
            rapidjson::Value& pixelShader = materialObject["pixel_shader"];
            if (pixelShader.IsString())
            {
                m_pixelShader = ResourceManager::it().getPSPtr(pixelShader.GetString());
            }
            else
            {
                utils::ErrorLogger::log("Parsing material file '" + name + "'\n'pixel_shader' was not a string id");
            }
        }
        else
        {
            utils::ErrorLogger::log("Parsing material file '" + name + "'\n'pixel_shader' was not set");
            return false;
        }

        // Gbuffer write pixel Shader
        if (materialObject.HasMember("gbuf_w_pixel_shader"))
        {
            rapidjson::Value& gBufferWritePixelShader = materialObject["gbuf_w_pixel_shader"];
            if (gBufferWritePixelShader.IsString())
            {
                std::string gBufferWritePixelShaderName = gBufferWritePixelShader.GetString();
                if (gBufferWritePixelShaderName[0] == '-')
                {
                    m_gBufferWritePixelShader = nullptr;
                    m_isGBufferCompatible = false;
                }
                else
                {
                    m_gBufferWritePixelShader = ResourceManager::it().getGBufferWritePSPtr(gBufferWritePixelShaderName);
                    m_isGBufferCompatible = true;
                }
            }
            else
            {
                utils::ErrorLogger::log("Parsing material file '" + name + "'\n'gbuf_w_pixel_shader' was not a string id");
            }
        }
        else
        {
            utils::ErrorLogger::log("Parsing material file '" + name + "'\n'gbuf_w_pixel_shader' was not set");
            return false;
        }

        return true;
    }

    bool Material::initialiseFromTextures(const std::vector<MaterialTexture>& textures)
    {
        m_name = "null";

        m_isGBufferCompatible = true;

        m_textures = textures;

        for (int textureSlot = 0; textureSlot < static_cast<int>(MaterialTextureType::eNumTypes); ++textureSlot)
        {
            bool needsDefault = true;
            for (int textureIndex = 0; textureIndex < m_textures.size(); ++textureIndex)
            {
                if (m_textures[textureIndex].m_bindSlot == textureSlot)
                {
                    needsDefault = false;
                    break;
                }
            }

            if (needsDefault)
            {
                m_textures.push_back(MaterialTexture(textureSlot, getDefaultTexture(static_cast<MaterialTextureType>(textureSlot))));
            }
        }

        m_vertexShader = ResourceManager::it().getDefaultVSPtr();
        m_pixelShader = ResourceManager::it().getDefaultPSPtr();
        m_gBufferWritePixelShader = ResourceManager::it().getDefaultGBufferWritePSPtr();

        return false;
    }

    void Material::bind(bool useGBuffer, bool bindPSData)
    {
        ID3D11DeviceContext* deviceContext = GraphicsHandler::it().getDeviceContext();

        deviceContext->VSSetShader(m_vertexShader->getShader(), nullptr, 0);

        if (bindPSData)
        {
            if (useGBuffer)
            {
                deviceContext->PSSetShader(m_gBufferWritePixelShader->getShader(), nullptr, 0);
            }
            else
            {
                deviceContext->PSSetShader(m_pixelShader->getShader(), nullptr, 0);
            }

            for (int textureIndex = 0; textureIndex < m_textures.size(); ++textureIndex)
            {
                MaterialTexture& materialTexture = m_textures[textureIndex];
                deviceContext->PSSetShaderResources(materialTexture.m_bindSlot, 1, materialTexture.m_texture->getShaderResourceView().GetAddressOf());
            }
        }
    }

    const std::string& Material::getName() const
    {
        return m_name;
    }

    const std::vector<MaterialTexture>& Material::getTextures() const
    {
        return m_textures;
    }

    bool Material::isGBufferCompatible() const
    {
        return m_isGBufferCompatible;
    }

    Texture* Material::getDefaultTexture(MaterialTextureType textureType)
    {
        switch (textureType)
        {
        case MaterialTextureType::eAlbedo:
            return ResourceManager::it().getColourTexturePtr(colours::sc_defaultAlbedo);
        case MaterialTextureType::eRoughness:
            return ResourceManager::it().getColourTexturePtr(colours::sc_defaultRoughness);
        case MaterialTextureType::eNormal:
            return ResourceManager::it().getColourTexturePtr(colours::sc_defaultNormal);
        case MaterialTextureType::eMetallic:
            return ResourceManager::it().getColourTexturePtr(colours::sc_defaultMetallic);
        case MaterialTextureType::eEmission:
            return ResourceManager::it().getColourTexturePtr(colours::sc_defaultEmission);
        case MaterialTextureType::eDepth:
            return ResourceManager::it().getColourTexturePtr(colours::sc_defaultDepth);
        case MaterialTextureType::eNumTypes:
            return nullptr;
        }
        return nullptr;
    }
}