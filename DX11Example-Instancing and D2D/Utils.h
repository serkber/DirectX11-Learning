#pragma once
#include <Windows.h>
#include "Typedefs.h"

namespace Utils {
	LPCWSTR GetMessageFromHr(HRESULT hr);
	LPCWSTR GetMessageFromBlob(const char* c);
	matrix GetModelMatrix(float2 position, float2 _scale, float rotation);
}