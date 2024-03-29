#pragma once

#pragma warning(disable: 4820) //warning C4820 : 'xxxxx' : 'n' bytes padding added after data member 'yyyyy'
#pragma warning(disable: 5039) //warning C5039 : 'xxxxxx' : pointer or reference to potentially throwing function passed to 'extern "C"' function under - EHc.Undefined behavior may occur if this function throws an exception.
#pragma warning(disable: 5045) //warning C5045: Compiler will insert Spectre mitigation for memory load if /Qspectre switch specified

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

//D3DX12 helpers
#pragma warning(push)
#pragma warning(disable: 4061) //warning C4061: enumerator 'xxxxx' in switch of enum 'yyyy' is not explicitly handled by a case label
#pragma warning(disable: 4365) //warning C4365: '=': conversion from 'xxxxx' to 'yyyy', signed/unsigned mismatch
#pragma warning(disable: 5219) // warning C5219: implicit conversion from 'xxx' to 'yyyy', possible loss of data
#define D3DX12_NO_STATE_OBJECT_HELPERS
#include <d3dx12/d3dx12.h>
#pragma warning(pop)

//for PIX support
#pragma warning(push)
#pragma warning(disable: 4365) //warning C4365 : 'return' : conversion from 'xxxxx' to 'yyyyy', signed / unsigned mismatch
#include <WinPixEventRuntime/pix3.h>
#pragma warning(pop)

//DirectXMath
#pragma warning(push)
#pragma warning(disable: 4668) //warning C4668: 'xxxxx' is not defined as a preprocessor macro, replacing with '0' for '#if/#elif'
#include <DirectXMath.h>
#pragma warning(pop)