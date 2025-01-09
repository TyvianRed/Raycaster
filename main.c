#include "raycast.h"

int main (void) {
    
	const Uint8 map[256] = {
		1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u,
		1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u,
		1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u,
		1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u,
		1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u, 0u, 0u, 0u, 1u,
		1u, 0u, 0u, 0u, 0u, 0u, 1u, 0u, 0u, 0u, 0u, 1u, 0u, 0u, 0u, 1u,
		1u, 0u, 0u, 0u, 1u, 1u, 1u, 0u, 0u, 0u, 0u, 1u, 1u, 1u, 0u, 1u,
		1u, 0u, 0u, 0u, 1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u,
		1u, 0u, 0u, 0u, 1u, 1u, 1u, 1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u,
		1u, 0u, 0u, 0u, 0u, 0u, 0u, 1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u,
		1u, 0u, 0u, 0u, 0u, 0u, 0u, 1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u,
		1u, 0u, 0u, 0u, 0u, 0u, 0u, 1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u,
		1u, 0u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 0u, 1u,
		1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u,
		1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u,
		1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u,
	};
	
	const size_t map_height = 16u, map_width = 16u;
		
	createWindow();
	
    Uint64 current_time, previous_time = SDL_GetTicks();
    
	while (IS_RENDERING(isExitClicked | isESCPressed)) {
        
        current_time = SDL_GetTicks();
        g_delta_time = (double)(current_time - previous_time);
        previous_time = current_time;
        
		SDL_RenderClear(g_renderer);
        
		handleEvent();
		
		fillBackground();
		drawMap(map, map_height, map_width);
		drawPlayer();
		
		performRaycasting();
		
		swapBuffersWindow();
        
	}

	quitRaycaster();

	return 0;
}
