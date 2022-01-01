#pragma once

#include <fstream>
#include <sstream>
#include <algorithm>

#include "rapidjson/reader.h"
#include "rapidjson/document.h"

#include "../utils/error_logger.h"
#include "../scene/scene_loader.h"

#include "../graphics/data/colour.h"

namespace hrzn::utils::JSONHelpers
{
    static scene::SceneLoader::LoadResult parse(std::string filePath, rapidjson::Document& outDocument)
    {
        std::ifstream fileObject(filePath.c_str());

        if (!fileObject) return scene::SceneLoader::LoadResult::eFailed;

        std::stringstream buffer;
        buffer << fileObject.rdbuf();
        std::string fileString = buffer.str();

        // Parse the data loaded from file using rapidjson
        outDocument.Parse(fileString.c_str());
        if (outDocument.HasParseError())
        {
            rapidjson::ParseErrorCode error = outDocument.GetParseError();
            int errorOffset = static_cast<int>(outDocument.GetErrorOffset());

            int errorStart = std::max(0, errorOffset - 20);
            int errorEnd = std::min(static_cast<int>(strlen(fileString.c_str())), errorOffset + 20);

            int beforeLen = errorOffset - errorStart;
            int afterLen = errorEnd - errorOffset;

            std::string errorString = fileString.substr(errorStart, beforeLen) + "[HERE]" + fileString.substr(errorOffset, afterLen);
            errorString.erase(std::remove(errorString.begin(), errorString.end(), '\n'), errorString.end());
            errorString.erase(std::remove(errorString.begin(), errorString.end(), '\t'), errorString.end());

            int lineNumber = utils::string_helpers::getLineNumberFromOffset(fileString, errorOffset);

            std::string finalErrorString = "Parsing Scene JSON\nCode: '" + std::to_string(error) + "'\nLine Num: '" + std::to_string(lineNumber) + "'\nError String: '" + errorString + "'\n";

            utils::ErrorLogger::log(finalErrorString);
            return scene::SceneLoader::LoadResult::eFailed;
        }

        return scene::SceneLoader::LoadResult::eOk;
    }

    static DirectX::XMFLOAT3 getFloat3FromArray(rapidjson::Value::Array arrayObject)
    {
        DirectX::XMFLOAT3 returnValue = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
        int index = 0;
        for (auto itr = arrayObject.Begin(); itr != arrayObject.End(); ++itr)
        {
            if (index == 0) returnValue.x = itr->GetFloat();
            else if (index == 1) returnValue.y = itr->GetFloat();
            else if (index == 2) returnValue.z = itr->GetFloat();
            else return returnValue;

            ++index;
        }

        return returnValue;
    }

    static void writeFloat3(const DirectX::XMFLOAT3& value, rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
    {
        writer.SetFormatOptions(rapidjson::PrettyFormatOptions::kFormatSingleLineArray);

        writer.StartArray();

        writer.Double(value.x);
        writer.Double(value.y);
        writer.Double(value.z);

        writer.EndArray();
        
        writer.SetFormatOptions(rapidjson::PrettyFormatOptions::kFormatDefault);
    }

    static DirectX::XMFLOAT4 getFloat4FromArray(rapidjson::Value::Array arrayObject)
    {
        DirectX::XMFLOAT4 returnValue = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
        int index = 0;
        for (auto itr = arrayObject.Begin(); itr != arrayObject.End(); ++itr)
        {
            if (index == 0) returnValue.x = itr->GetFloat();
            else if (index == 1) returnValue.y = itr->GetFloat();
            else if (index == 2) returnValue.z = itr->GetFloat();
            else if (index == 3) returnValue.w = itr->GetFloat();
            else return returnValue;

            ++index;
        }

        return returnValue;
    }

    static void writeFloat4(const DirectX::XMFLOAT4& value, rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
    {
        writer.SetFormatOptions(rapidjson::PrettyFormatOptions::kFormatSingleLineArray);

        writer.StartArray();

        writer.Double(value.x);
        writer.Double(value.y);
        writer.Double(value.z);
        writer.Double(value.w);

        writer.EndArray();

        writer.SetFormatOptions(rapidjson::PrettyFormatOptions::kFormatDefault);
    }

    static BYTE getByteColourFromValue(rapidjson::Value& value)
    {
        BYTE byteColour;
        if (value.IsInt())
        {
            byteColour = static_cast<BYTE>(value.GetInt());
        }
        else
        {
            byteColour = static_cast<BYTE>(value.GetFloat() * 255.0f);
        }
        return byteColour;
    }

    static gfx::Colour getColourFromArray(rapidjson::Value::Array arrayObject)
    {
        BYTE red = 0, green = 0, blue = 0;

        if (arrayObject.Size() >= 3)
        {
            red = getByteColourFromValue(arrayObject[0]);
            green = getByteColourFromValue(arrayObject[1]);
            blue = getByteColourFromValue(arrayObject[2]);
        }

        return gfx::Colour(red, green, blue);
    }
}