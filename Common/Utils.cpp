#include "Utils.h"

#include <string>
#include <vector>

namespace Utils {
    LPCWSTR GetMessageFromHr(HRESULT hr)
    {
        LPVOID errorMsg;
        DWORD flags = FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS;
        DWORD langId = MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT);

        if (FormatMessage(flags, NULL, hr, langId, (LPWSTR)&errorMsg, 0, NULL) != 0)
        {
            return (LPCWSTR)errorMsg;
        }
        else
        {
            return L"Unknown error";
        }
    }

    LPCWSTR GetMessageFromBlob(const char* c)
    {
        size_t length = strlen(c) + 1;
        wchar_t* wText = new wchar_t[length];
        size_t convertedChars = 0;
        mbstowcs_s(&convertedChars, wText, length, c, _TRUNCATE);
        return wText;
    }

    matrix GetModelMatrix(float2 position, float2 _scale, float rotation)
    {
        matrix translation = DirectX::XMMatrixTranslation(position.x, position.y, 0.0f);
        matrix rotationZ = DirectX::XMMatrixRotationZ(rotation);
        matrix scale = DirectX::XMMatrixScaling(_scale.x, _scale.y, 1.0f);
        return scale * rotationZ * translation;
    }

    void UnloadResource(ID3D11Resource** resource)
    {
        if(resource && *resource)
        {
            (*resource)->Release();
        }
        *resource = nullptr;
    }

    std::vector<std::string> Split(const std::string& str, const char character)
    {
        std::vector<std::string> result;
        int ref = 0;
        for (int i = 0; i < str.length(); ++i)
        {
            if(str[i] == character)
            {
                result.push_back(str.substr(ref, i - ref));
                ref = i + 1;
            }
            if(i == str.length() - 1)
            {
                result.push_back(str.substr(ref, str.length() - ref));
            }
        }

        return result;
    }
}