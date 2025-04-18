#pragma once
#include <string>

namespace Asset
{
    class IAssetLoader
    {
    public:
        virtual ~IAssetLoader() = default;
        virtual bool Load(const std::wstring& filepath) = 0;
    };
}