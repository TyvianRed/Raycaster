#include <time.h>

#include "raycast.h"

int main (void) {
	
	Uint8 posBuffer[SCREEN_HEIGHT * SCREEN_WIDTH] = { 0u, };
	Uint32 framebuffer[SCREEN_HEIGHT * SCREEN_WIDTH] = { 0u, };
	
	Uint8 isExitClicked = 0u, isESCPressed = 0u;
	
	const size_t mapHeight = 16, mapWidth = 16;
	Uint8 map[256] = {
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
		1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
		1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
		1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1,
		1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1,
		1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 0, 1,
		1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
		1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1,
		1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1,
		1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1,
		1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1,
		1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1,
		1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
		1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	};
	
	/* https://github.com/ssloy/tinyraycaster/wiki/Part-1:-crude-3D-renderings */
	double playerPosS = 20., playerPosT = 25.;
	double playerYaw = 0.;
	double rayDirS = 1., rayDirT = 0.;
	double camLineS = 0., camLineT = 1.;

	SDL_Window* window = NULL;
	SDL_Renderer* renderer = NULL;
	SDL_Texture* texture = NULL;

	/* https://wiki.libsdl.org/SDL3/SDL_CreateWindow */
	SDL_Init(SDL_INIT_VIDEO);

	window = SDL_CreateWindow("Raycaster", SCREEN_WIDTH, SCREEN_HEIGHT, 0);
	if (NULL == window) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "main(): failed to create window\n%s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	renderer = SDL_CreateRenderer(window, NULL);
	SDL_SetRenderDrawColor(renderer, 127u, 127u, 127u, 255u);
	
	/* https://wiki.libsdl.org/SDL3/SDL_DestroyTexture */
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, SCREEN_WIDTH, SCREEN_WIDTH);
	if (texture == NULL) {
		fprintf(stderr, "main(): failed to create texture\n%s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}
		
	while (IS_RENDERING(isExitClicked | isESCPressed) == 1) {
		
		SDL_RenderClear(renderer);
		
		handleEvent(
			&isExitClicked, &isESCPressed,
			&playerPosS, &playerPosT, &playerYaw,
			&rayDirS, &rayDirT,
			&camLineS, &camLineT
		);
		
		fillBackground(framebuffer, SCREEN_HEIGHT, SCREEN_WIDTH);
		
		drawMap(framebuffer, posBuffer, SCREEN_HEIGHT, SCREEN_WIDTH, map, mapHeight, mapWidth);
		drawPlayer(framebuffer, SCREEN_HEIGHT, SCREEN_WIDTH, playerPosS, playerPosT);
		
		performRaycasting(
			framebuffer, posBuffer, SCREEN_WIDTH,
			playerPosS, playerPosT, rayDirS, rayDirT,
			camLineS, camLineT
		);
		
		swapBuffersWindow(renderer, texture, framebuffer, SCREEN_HEIGHT, SCREEN_WIDTH);
		SDL_RenderPresent(renderer);
		
	}

	SDL_DestroyTexture(texture);

	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
