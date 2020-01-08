# D3DX12 header only library
add_library(d3dx12 INTERFACE IMPORTED GLOBAL)
target_include_directories(d3dx12 INTERFACE "d3dx12/include")

# WinPixEventRuntime library
add_library(WinPixEventRuntime SHARED IMPORTED GLOBAL)
target_include_directories(WinPixEventRuntime INTERFACE "WinPixEventRuntime/include")
set_target_properties(WinPixEventRuntime
	PROPERTIES
		IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/lib/WinPixEventRuntime/bin/x64/WinPixEventRuntime.dll"
		IMPORTED_IMPLIB   "${CMAKE_SOURCE_DIR}/lib/WinPixEventRuntime/bin/x64/WinPixEventRuntime.lib"
	)

