#pragma once
#include <d3d11.h>
#include <string>
#include <vector>
#include <Windows.h>
#include "Typedefs.h"

namespace Utils {
    LPCWSTR GetMessageFromHr(HRESULT hr);
    LPCWSTR GetMessageFromBlob(const char* c);
    matrix GetModelMatrix(float2 position, float2 _scale, float rotation);
    void UnloadResource(ID3D11Resource** resource);
    std::vector<std::string> Split(const std::string& str, const char character);
}