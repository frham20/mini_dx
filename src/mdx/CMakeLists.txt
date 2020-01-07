target_sources(mini_dx 
	PRIVATE
		"app.cpp"
		"exception.cpp"
		"gfx.cpp"
		"window.cpp"
	)

target_precompile_headers(mini_dx 
	PRIVATE
		"platform.h"
		"core.h"
	)