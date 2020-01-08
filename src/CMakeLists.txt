target_sources(mini_dx 
	PRIVATE
		"mdx/app.cpp"
		"mdx/exception.cpp"
		"mdx/gfx.cpp"
		"mdx/window.cpp"
	)

target_precompile_headers(mini_dx 
	PRIVATE
		"mdx/platform.h"
		"mdx/core.h"
	)