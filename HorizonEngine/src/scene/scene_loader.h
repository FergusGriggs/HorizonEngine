#pragma once

#include <string.h>
#include <iostream>

#include "rapidjson/reader.h"

namespace hrzn::scene
{
    struct JSONHandler
    {
        bool Null()
        {
            std::cout << "Null()" << "\n";
            return true;
        }

        bool Bool(bool b)
        {
            std::cout << "Bool(" << std::boolalpha << b << ")" << "\n";
            return true;
        }

        bool Int(int i)
        {
            std::cout << "Int(" << i << ")" << "\n";
            return true;
        }

        bool Uint(unsigned u)
        { 
            std::cout << "Uint(" << u << ")" << "\n";
            return true;
        }

        bool Int64(int64_t i)
        {
            std::cout << "Int64(" << i << ")" << "\n";
            return true;
        }

        bool Uint64(uint64_t u)
        {
            std::cout << "Uint64(" << u << ")" << "\n";
            return true;
        }

        bool Double(double d)
        {
            std::cout << "Double(" << d << ")" << "\n";
            return true;
        }

        bool RawNumber(const char* str, rapidjson::SizeType length, bool copy)
        {
            std::cout << "Number(" << str << ", " << length << ", " << std::boolalpha << copy << ")" << "\n";
            return true;
        }

        bool String(const char* str, rapidjson::SizeType length, bool copy)
        {
            std::cout << "String(" << str << ", " << length << ", " << std::boolalpha << copy << ")" << "\n";
            return true;
        }

        bool StartObject()
        {
            std::cout << "StartObject()" << "\n";
            return true;
        }

        bool Key(const char* str, rapidjson::SizeType length, bool copy)
        {
            std::cout << "Key(" << str << ", " << length << ", " << std::boolalpha << copy << ")" << "\n";
            return true;
        }

        bool EndObject(rapidjson::SizeType memberCount)
        {
            std::cout << "EndObject(" << memberCount << ")" << "\n";
            return true;
        }

        bool StartArray()
        {
            std::cout << "StartArray()" << "\n";
            return true;
        }

        bool EndArray(rapidjson::SizeType elementCount)
        {
            std::cout << "EndArray(" << elementCount << ")" << "\n";
            return true;
        }
    };

    class SceneLoader
    {
    public:
        SceneLoader();
        ~SceneLoader();

        void loadScene(std::string sceneName);

    private:
        rapidjson::Reader m_reader;
        JSONHandler       m_handler;
    };
}
