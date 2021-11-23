cmake_minimum_required(VERSION 3.16)
project(mini_dx VERSION 0.1 LANGUAGES CXX)

include(GNUInstallDirs)

add_executable(mini_dx WIN32)

set_target_properties(mini_dx
    PROPERTIES
		ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
		LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
		RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
		CXX_STANDARD 20
	)

target_include_directories(mini_dx PRIVATE "src")
target_compile_definitions(mini_dx PRIVATE UNICODE)
target_compile_options(mini_dx PRIVATE /Wall /WX)


# Add Pix NuGet package
#    set_target_properties(mini_dx PROPERTIES VS_PACKAGE_REFERENCES "WinPixEventRuntime_1.0.190604001")
# Doesn't work for C++ projects :( add dependency manually
# + Need post build step to copy the WinPixEventRuntime DLL as it's an external lib not being built by CMake
target_link_libraries(mini_dx PRIVATE WinPixEventRuntime d3dx12 DirectXMath)
add_custom_command(TARGET mini_dx 
	POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:WinPixEventRuntime> $<TARGET_FILE_DIR:mini_dx>
	)

add_subdirectory(src)
add_subdirectory(lib)
