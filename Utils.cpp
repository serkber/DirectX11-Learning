#include "Utils.h"

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
        mbstowcs(wText, c, length);
        return wText;
    }

    matrix GetModelMatrix(float2 position, float2 _scale, float rotation)
    {
        matrix translation = DirectX::XMMatrixTranslation(position.x, position.y, 0.0f);
        matrix rotationZ = DirectX::XMMatrixRotationZ(rotation);
        matrix scale = DirectX::XMMatrixScaling(_scale.x, _scale.y, 1.0f);
        return scale * rotationZ * translation;
    }
}