
# WinPixEventRuntime library
add_library(WinPixEventRuntime SHARED IMPORTED GLOBAL)

set_target_properties(WinPixEventRuntime
	PROPERTIES
		IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/lib/WinPixEventRuntime_1.0.190604001/bin/x64/WinPixEventRuntime.dll"
		IMPORTED_IMPLIB   "${CMAKE_SOURCE_DIR}/lib/WinPixEventRuntime_1.0.190604001/bin/x64/WinPixEventRuntime.lib"
	)

target_include_directories(WinPixEventRuntime INTERFACE "WinPixEventRuntime_1.0.190604001/include/WinPixEventRuntime")