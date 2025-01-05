#include "raycast.h"


Uint8 isExitClicked, isESCPressed;
static Uint8 position_buffer[SCREEN_BUF_SIZE];

static Uint32 framebuffer[SCREEN_BUF_SIZE];

static double g_player_pos_s = 20., g_player_pos_t = 25., g_player_yaw;

static double g_dir_s = 1., g_dir_t;

static double g_cam_plane_s, g_cam_plane_t = 1.;


SDL_Window* g_window;
SDL_Renderer* g_renderer;
SDL_Texture* g_texture;


void createWindow(void) {

	// https://wiki.libsdl.org/SDL3/SDL_CreateWindow
	SDL_Init(SDL_INIT_VIDEO);

	if ((g_window = SDL_CreateWindow(
		"Raycaster",
		SCREEN_WIDTH, SCREEN_HEIGHT,
		0
	)) == NULL) {
		
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "createWindow(): failed to create window\n%s\n", SDL_GetError());
		exit(EXIT_FAILURE);
		
	}

	if ((g_renderer = SDL_CreateRenderer(g_window, NULL)) == NULL) {
		
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "createWindow(): failed to create renderer\n%s\n", SDL_GetError());
		exit(EXIT_FAILURE);
		
	}
	
	if (!SDL_SetRenderDrawColor(g_renderer, 127u, 127u, 127u, 255u)) {
		
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "createWindow(): failed to set render draw color\n%s\n", SDL_GetError());
		exit(EXIT_FAILURE);
		
	}
	
	if ((g_texture = SDL_CreateTexture(
		g_renderer,
		SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
		SCREEN_WIDTH, SCREEN_WIDTH
	)) == NULL) {
		
		fprintf(stderr, "createWindow(): failed to create texture\n%s\n", SDL_GetError());
		exit(EXIT_FAILURE);
		
	}
	
}

void quitRaycaster(void) {
	
	SDL_DestroyTexture(g_texture);
	SDL_DestroyWindow(g_window);
	SDL_Quit();
	
}

/* color to bit pattern */
Uint32 ctob(const Uint32 r, const Uint32 g, const Uint32 b, const Uint32 a) {
	
	/* https://github.com/QuantitativeBytes/qbRayTrace/blob/main/Ep1Code/qbRayTrace/qbImage.cpp */
	
#if SDL_BYTEORDER == SDL_BIG_ENDIAN

	return a << 24 | b << 16 | g << 8 | r;
	
#else
	
	return r << 24 | g << 16 | b << 8 | a;

#endif

}

/* bit pattern to color */
void btoc(const Uint32 color, Uint8* const r,  Uint8* const g, Uint8* const b, Uint8* const a) {
	
	assert(r != NULL && g != NULL && b != NULL && a != NULL);
	
#if SDL_BYTEORDER == SDL_BIG_ENDIAN

    *a = color >> 24;
    *b = color >> 16 & 255;
    *g = color >> 8 & 255;
    *r = color & 255;
	
#else
	
	*r = color >> 24;
    *g = color >> 16 & 255;
    *b = color >> 8 & 255;
    *a = color & 255;
	
#endif

}


void handleEvent(void) {
	
	SDL_Event event;

	while (SDL_PollEvent(&event)) {
		
		isExitClicked = (SDL_EVENT_QUIT == event.type) ? 1u : 0u;

		/* https://wiki.libsdl.org/SDL3/BestKeyboardPractices */
		isESCPressed = (SDL_EVENT_KEY_DOWN == event.type && event.key.key == SDLK_ESCAPE) ? 2u : 0u;
		
		if (SDL_EVENT_KEY_DOWN == event.type) {
		
			if (event.key.key == SDLK_W) {
				
				g_player_pos_s += g_dir_s;
				g_player_pos_t += g_dir_t;
				
			} else if (event.key.key == SDLK_S) {
			
				g_player_pos_s -= g_dir_s;
				g_player_pos_t -= g_dir_t;
			
			} else if (event.key.key == SDLK_A) {
				
				double horz_vec_s, horz_vec_t;
				getHorizontalVector(&horz_vec_s, &horz_vec_t);
			
				g_player_pos_s += horz_vec_s;
				g_player_pos_t += horz_vec_t;
			
			} else if (event.key.key == SDLK_D) {
				
				double horz_vec_s, horz_vec_t;
				getHorizontalVector(&horz_vec_s, &horz_vec_t);
			
				g_player_pos_s -= horz_vec_s;
				g_player_pos_t -= horz_vec_t;
			
			} else if (event.key.key == SDLK_LEFT) {
			
				g_player_yaw -= 1.;
				if (g_player_yaw < 0.) g_player_yaw += 360.;
				lookAt(DEGREE_TO_RADIAN(g_player_yaw));
				
			} else if (event.key.key == SDLK_RIGHT) {
				 
				g_player_yaw += 1.;
				if (g_player_yaw > 360.) g_player_yaw -= 360.;
				lookAt(DEGREE_TO_RADIAN(g_player_yaw));
				
			}
		
		}
		
	}	
	
}

void fillBackground(void) {
	
	size_t i = 0, j;
	for (; i < SCREEN_HEIGHT; i++)
		for (j = 0; j < SCREEN_WIDTH; j++)
			framebuffer[i * SCREEN_WIDTH + j] = ctob((Uint32)i, (Uint32)j, 0u, 255u);
	
}

void drawMap(const Uint8* const p_map, const size_t map_height, const size_t map_width) {
	
	const Uint32 color_wall = ctob(0u, 0u, 127u, 255u);
	
	const double inv_map_height = 1.f / map_height;
	const double inv_map_width = 1.f / map_width;
	
	size_t i = 0, j;
	
	for (; i < map_height; i++) {
			
		for (j = 0; j < map_width; j++) {
		
			if (p_map[i * map_width + j] == 0) continue;
				
			const size_t vert_end = (i + 1) * inv_map_height * SCREEN_HEIGHT;
			const size_t horz_end = (j + 1) * inv_map_width * SCREEN_WIDTH;
			
			size_t dHM = i * inv_map_height * SCREEN_HEIGHT, dWM;
			
			for (; dHM < vert_end; dHM++) {
					
				for (dWM = j * inv_map_width * SCREEN_WIDTH; dWM < horz_end; dWM++) {
					
					const size_t buffer_pixel_index = dHM * SCREEN_WIDTH + dWM;
					
					framebuffer[buffer_pixel_index] = color_wall;
					position_buffer[buffer_pixel_index] = 1u;
					
				}
				
			}
			
		}
		
	}
	
}

void drawPlayer(void) {
	
	const size_t s = (size_t)floor(g_player_pos_s);
	const size_t t = (size_t)floor(g_player_pos_t);
	const size_t player_location =  t * SCREEN_WIDTH + s;
		
	framebuffer[player_location] = ctob(0u, 0u, 0u, 255u);
	
}


void lookAt(const double radian) {
	
	/*
	const double temp_dir_s = *p_dir_s * cos(radian) - *p_dir_t * sin(radian);
	const double temp_dir_t = *p_dir_t * sin(radian) + *p_dir_s * cos(radian);
	*p_dir_s = temp_dir_s;
	*p_dir_s = temp_dir_t;
	*/
		
	g_dir_s = cos(radian);
	g_dir_t = sin(radian);

	g_cam_plane_s = -sin(radian);
	g_cam_plane_t = cos(radian);

}

void getHorizontalVector(double* const p_horz_vec_s, double* const p_horz_vec_t) {
	
	*p_horz_vec_s = g_dir_t;
	*p_horz_vec_t = -g_dir_s;
	
}


void castRay(const double ray_dir_s, const double ray_dir_t) {

	/* https://lodev.org/cgtutor/raycasting.html */
	/* https://www.youtube.com/watch?v=NbSee-XM7WA */
		
	int intersected_side; /* 0 for horizontal grid line, 1 for vertical grid line */
	
	int step_s = 0, step_t = 0;
	
	size_t player_pos_s_on_framebuffer = (size_t)floor(g_player_pos_s);
	size_t player_pos_t_on_framebuffer = (size_t)floor(g_player_pos_t);
	
	size_t buffer_pixel_index =
		player_pos_t_on_framebuffer * SCREEN_WIDTH + player_pos_s_on_framebuffer;
	
	int isWallHit = position_buffer[buffer_pixel_index] == 1u;
	
	const Uint32 color_ray_white = ctob(255u, 255u, 255u, 255u);
	
	const double delta_s = (ray_dir_s == 0.) ? DBL_MAX : fabs(1. / ray_dir_s);
	const double delta_t = (ray_dir_t == 0.) ? DBL_MAX : fabs(1. / ray_dir_t);
	
	double dist_s = 0., dist_t = 0.;
	
	if (ray_dir_s > 0.) {
		
		dist_s = (1.0 - g_player_pos_s + (double)player_pos_s_on_framebuffer) * delta_s; 
		step_s = 1;
		
	} else {
		
		dist_s = (g_player_pos_s - (double)player_pos_s_on_framebuffer) * delta_s;
		step_s = -1;
		
	}
	
	if (ray_dir_t > 0.) {
		
		dist_t = (1.0 - g_player_pos_t + (double)player_pos_t_on_framebuffer) * delta_t; 
		step_t = 1;
		
	} else {
		
		dist_t = (g_player_pos_t - (double)player_pos_t_on_framebuffer) * delta_t; 
		step_t = -1;
		
	}


	while(isWallHit == FALSE) {
		
		if (dist_s > dist_t) {
			
			dist_t += delta_t;
			player_pos_t_on_framebuffer += step_t;
			intersected_side = 1;
			
		} else {
			
			dist_s += delta_s;
			player_pos_s_on_framebuffer += step_s;
			intersected_side = 0;
			
		}
		
		buffer_pixel_index = player_pos_t_on_framebuffer * SCREEN_WIDTH + player_pos_s_on_framebuffer;
		framebuffer[buffer_pixel_index] = color_ray_white;
		isWallHit = position_buffer[buffer_pixel_index] == 1u;
	
	}
	
}

void performRaycasting(void) {
			
	size_t ray = 0u;
	
	castRay(g_dir_s, g_dir_t);
	
	for (; ray < 128u; ray++) {	
		
		double left_ray_dir_s = g_dir_s - g_cam_plane_s * ray / 256.;
		double left_ray_dir_t = g_dir_t - g_cam_plane_t * ray / 256.;
		
		double right_ray_dir_s = g_dir_s + g_cam_plane_s * ray / 256.;
		double right_ray_dir_t = g_dir_t + g_cam_plane_t * ray / 256.;
		
		castRay(left_ray_dir_s, left_ray_dir_t);
		castRay(right_ray_dir_s, right_ray_dir_t);
						
	}
	
}


void swapBuffersWindow(void) {
	
	/* https://github.com/QuantitativeBytes/qbRayTrace/blob/main/Ep1Code/qbRayTrace/qbImage.hpp */
	/* https://github.com/QuantitativeBytes/qbRayTrace/blob/main/Ep1Code/qbRayTrace/qbImage.cpp */
	
	/* https://www.youtube.com/watch?v=rB8N5cFCHLQ */
	
	SDL_UpdateTexture(g_texture, NULL, (void*)framebuffer, sizeof(*framebuffer) * SCREEN_WIDTH);
	
	/* https://wiki.libsdl.org/SDL3/SDL_RenderTexture */
	SDL_FRect srcrect, dstrect;
	srcrect.x = dstrect.x = srcrect.y = dstrect.y = 0.f;
	srcrect.w = dstrect.w = (float)SCREEN_WIDTH;
	srcrect.h = dstrect.h = (float)SCREEN_HEIGHT;
	
	SDL_RenderTexture(g_renderer, g_texture, &srcrect, &dstrect);
	SDL_RenderPresent(g_renderer);

}
