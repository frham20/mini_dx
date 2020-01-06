#pragma once

//warning C4820 : 'xxxxx' : 'n' bytes padding added after data member 'yyyyy'
#pragma warning(disable: 4820)
//warning C5039 : 'xxxxxx' : pointer or reference to potentially throwing function passed to 'extern "C"' function under - EHc.Undefined behavior may occur if this function throws an exception.
#pragma warning(disable: 5039)
//warning C5045: Compiler will insert Spectre mitigation for memory load if /Qspectre switch specified
#pragma warning(disable: 5045)

#include <sdkddkver.h>
#undef _WIN32_WINNT
#define _WIN32_WINNT    _WIN32_WINNT_WIN10
#undef WINVER
#define WINVER          _WIN32_WINNT

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <cassert>
#include <cwchar>
#include <cstdarg>

//for ComPtr
#include <wrl/client.h>

#include <d3d12.h>
#include <d3d12sdklayers.h>
#include <dxgi1_6.h>

//for PIX support
#include <pix3.h>

#include "mdx\types.h"
#include "mdx\exception.h"

