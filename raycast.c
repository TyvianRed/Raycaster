#include "raycast.h"


Uint8 isExitClicked, isESCPressed;
static Uint8 s_position_buffer[SCREEN_BUF_SIZE];
static Uint32 s_framebuffer[SCREEN_BUF_SIZE];

static double s_player_pos_s = 20., s_player_pos_t = 25.;
static double s_dir_s = 1., s_dir_t = 0.;
static const double S_ROTATION_SPEED = 1. * M_PI / 180.;
static double s_cam_plane_s = 0., s_cam_plane_t = 0.5;
double g_delta_time;

SDL_Window* g_window;
SDL_Renderer* g_renderer;
SDL_Texture* g_texture;


void createWindow(void) {

	// https://wiki.libsdl.org/SDL3/SDL_CreateWindow
	SDL_Init(SDL_INIT_VIDEO);

    g_window = SDL_CreateWindow("Raycaster", DUAL_SCREEN_WIDTH, SCREEN_HEIGHT, 0);
	if (g_window == NULL) {
		
		SDL_LogError(
            SDL_LOG_CATEGORY_ERROR,
            "createWindow(): failed to create window\n%s\n",
            SDL_GetError()
        );
		exit(EXIT_FAILURE);
		
	}

    g_renderer = SDL_CreateRenderer(g_window, NULL);
	if (g_renderer == NULL) {
		
		SDL_LogError(
            SDL_LOG_CATEGORY_ERROR,
            "createWindow(): failed to create renderer\n%s\n",
            SDL_GetError()
        );
		exit(EXIT_FAILURE);
		
	}
	
	if (!SDL_SetRenderDrawColor(g_renderer, 127u, 127u, 127u, 255u)) {
		
		SDL_LogError(
            SDL_LOG_CATEGORY_ERROR,
            "createWindow(): failed to set render draw color\n%s\n",
            SDL_GetError()
        );
		exit(EXIT_FAILURE);
		
	}
	
    g_texture = SDL_CreateTexture(
		g_renderer,
		SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
		DUAL_SCREEN_WIDTH, SCREEN_WIDTH
	);
	if (g_texture == NULL) {
		
		fprintf(stderr, "createWindow(): failed to create texture\n%s\n", SDL_GetError());
		exit(EXIT_FAILURE);
		
	}
	
}

void quitRaycaster(void) {
	
	SDL_DestroyTexture(g_texture);
	SDL_DestroyWindow(g_window);
	SDL_Quit();
	
}

// bit pattern to color
void btoc(
    const Uint32 color, Uint8* const r,  Uint8* const g, Uint8* const b, Uint8* const a
) {
	
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

		// https://wiki.libsdl.org/SDL3/BestKeyboardPractices
		isESCPressed =
            (SDL_EVENT_KEY_DOWN == event.type && event.key.key == SDLK_ESCAPE) ? 2u : 0u;
		
		if (SDL_EVENT_KEY_DOWN == event.type) {
        
            const double previous_player_pos_s = s_player_pos_s;
            const double previous_player_pos_t = s_player_pos_t;
        
			if (event.key.key == SDLK_W) {
                
                s_player_pos_s += s_dir_s * g_delta_time;
                s_player_pos_t += s_dir_t * g_delta_time;
                
                detectCollision(previous_player_pos_s, previous_player_pos_t);
                
                /*
                const double previous_player_pos_s = s_player_pos_s;
                const double previous_player_pos_t = s_player_pos_t;
                
                s_player_pos_s += s_dir_s * g_delta_time;
                
                size_t player_pos_s_on_framebuffer = (size_t)floor(s_player_pos_s);
                size_t player_pos_t_on_framebuffer = (size_t)floor(s_player_pos_t);
                
                if (s_position_buffer[
                    player_pos_t_on_framebuffer * DUAL_SCREEN_WIDTH + player_pos_s_on_framebuffer
                ] == 1u) {
                    
                    s_player_pos_s = previous_player_pos_s;
                   
                }
                
                s_player_pos_t += s_dir_t * g_delta_time;
                player_pos_t_on_framebuffer = (size_t)floor(s_player_pos_t);
                
                if (s_position_buffer[
                    player_pos_t_on_framebuffer * DUAL_SCREEN_WIDTH + player_pos_s_on_framebuffer
                ] == 1u) {
                    
                    s_player_pos_t = previous_player_pos_t;
                   
                }
                */
                
                /*
                // Not necessary when the map has wall on its boundaries.
                if (s_player_pos_s < EPSILON || s_player_pos_s >= SCREEN_WIDTH)
                    s_player_pos_s = previous_player_pos_s;
                
                if (s_player_pos_t < EPSILON || s_player_pos_t >= SCREEN_HEIGHT)
                    s_player_pos_t = previous_player_pos_t;
                */
                
			} else if (event.key.key == SDLK_S) {

				s_player_pos_s -= s_dir_s * g_delta_time;
				s_player_pos_t -= s_dir_t * g_delta_time;
                
                detectCollision(previous_player_pos_s, previous_player_pos_t);
			
			} else if (event.key.key == SDLK_A) {
				
                double horz_vec_s, horz_vec_t;
				getHorizontalVector(&horz_vec_s, &horz_vec_t);

				s_player_pos_s += horz_vec_s * g_delta_time;
				s_player_pos_t += horz_vec_t * g_delta_time;
                
                detectCollision(previous_player_pos_s, previous_player_pos_t);
			
			} else if (event.key.key == SDLK_D) {
				
				double horz_vec_s, horz_vec_t;
				getHorizontalVector(&horz_vec_s, &horz_vec_t);

				s_player_pos_s -= horz_vec_s * g_delta_time;
				s_player_pos_t -= horz_vec_t * g_delta_time;
                
                detectCollision(previous_player_pos_s, previous_player_pos_t);
                
			} else if (event.key.key == SDLK_LEFT) {
                lookAt(-1.);
			} else if (event.key.key == SDLK_RIGHT) {
                lookAt(1.);
            }
                        
		}
		
	}	
	
}

void fillBackground(void) {
	
	size_t i = 0, j;
	for (; i < SCREEN_HEIGHT; i++) {
		for (j = 0; j < SCREEN_WIDTH; j++) {
            const Uint32 color_pixel = ctob((Uint32)i, (Uint32)j, 0u, 255u);
			s_framebuffer[i * DUAL_SCREEN_WIDTH + j] = color_pixel;
            s_framebuffer[i * DUAL_SCREEN_WIDTH + (j + SCREEN_WIDTH)] = color_pixel;
        }
    }
    
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
					
                    const size_t buffer_pixel_index = dHM * DUAL_SCREEN_WIDTH + dWM;
					s_framebuffer[buffer_pixel_index] = color_wall;
					s_position_buffer[buffer_pixel_index] = 1u;
					
				}
				
			}
			
		}
		
	}
	
}

void drawPlayer(void) {
	
	const size_t s = (size_t)floor(s_player_pos_s);
	const size_t t = (size_t)floor(s_player_pos_t);
	const size_t player_location =  t * DUAL_SCREEN_WIDTH + s;
		
	s_framebuffer[player_location] = ctob(0u, 0u, 0u, 255u);
	
}


void lookAt(const double rotation_direction) {
	
    // https://lodev.org/cgtutor/raycasting.html
    
    const double rotation = rotation_direction * S_ROTATION_SPEED * g_delta_time;

    const double temp_s_dir_s = s_dir_s;
    const double temp_cam_plane_s = s_cam_plane_s;
    
    s_dir_s = temp_s_dir_s * cos(rotation) - s_dir_t * sin(rotation);
    s_dir_t = temp_s_dir_s * sin(rotation) + s_dir_t * cos(rotation);

    s_cam_plane_s = temp_cam_plane_s * cos(rotation) - s_cam_plane_t * sin(rotation);
    s_cam_plane_t = temp_cam_plane_s * sin(rotation) + s_cam_plane_t * cos(rotation);

}

void getHorizontalVector(double* const p_horz_vec_s, double* const p_horz_vec_t) {
	
	*p_horz_vec_s = s_dir_t;
	*p_horz_vec_t = -s_dir_s;
	
}


void castRay(const double ray_dir_s, const double ray_dir_t, const size_t column) {

	// https://lodev.org/cgtutor/raycasting.html
	// https://www.youtube.com/watch?v=NbSee-XM7WA
		
	bool intersected_side; // 0 for horizontal grid line, 1 for vertical grid line
	
	int step_s = 0, step_t = 0;
	
	size_t player_pos_s_on_framebuffer = (size_t)floor(s_player_pos_s);
	size_t player_pos_t_on_framebuffer = (size_t)floor(s_player_pos_t);
	
	size_t buffer_pixel_index =
		player_pos_t_on_framebuffer * DUAL_SCREEN_WIDTH + player_pos_s_on_framebuffer;
	
	bool is_wall_intersected = (s_position_buffer[buffer_pixel_index] == 1u);
	
	const Uint32 color_ray_white = ctob(255u, 255u, 255u, 255u);
	const Uint32 color_intersected = ctob(0u, 0u, 127u, 255u);
    const Uint32 color_dimmed_intersected = ctob(0u, 0u, 63u, 255u);
    
	const double delta_s = (ray_dir_s == 0.) ? DBL_MAX : fabs(1. / ray_dir_s);
	const double delta_t = (ray_dir_t == 0.) ? DBL_MAX : fabs(1. / ray_dir_t);
	
	double dist_s = 0., dist_t = 0.;
    
	if (ray_dir_s > 0.) {
		
		dist_s = (1.0 - s_player_pos_s + (double)player_pos_s_on_framebuffer) * delta_s; 
		step_s = 1;
		
	} else {
		
		dist_s = (s_player_pos_s - (double)player_pos_s_on_framebuffer) * delta_s;
		step_s = -1;
		
	}
	
	if (ray_dir_t > 0.) {
		
		dist_t = (1.0 - s_player_pos_t + (double)player_pos_t_on_framebuffer) * delta_t; 
		step_t = 1;
		
	} else {
		
		dist_t = (s_player_pos_t - (double)player_pos_t_on_framebuffer) * delta_t; 
		step_t = -1;
		
	}


	while(!is_wall_intersected) {
		
		if (dist_s > dist_t) {
			
			dist_t += delta_t;
			player_pos_t_on_framebuffer += step_t;
			intersected_side = true;
			
		} else {
			
			dist_s += delta_s;
			player_pos_s_on_framebuffer += step_s;
			intersected_side = false;
			
		}
		
		buffer_pixel_index =
            player_pos_t_on_framebuffer * DUAL_SCREEN_WIDTH + player_pos_s_on_framebuffer;
		s_framebuffer[buffer_pixel_index] = color_ray_white;
		is_wall_intersected = (s_position_buffer[buffer_pixel_index] == 1u);
	
	}
    
    // https://lodev.org/cgtutor/raycasting.html
    
    const double perpendicular_distance = (intersected_side) ? dist_t - delta_t : dist_s - delta_s;
    size_t intersected_column_height = SCREEN_HEIGHT;
    if (perpendicular_distance > EPSILON)
        intersected_column_height = (size_t)(SCREEN_HEIGHT * 42 / perpendicular_distance);
    
    const signed int missed_column_height = 
        (signed int)((SCREEN_HEIGHT - intersected_column_height) * 0.5);
    
    size_t framebuffer_row = 0, framebuffer_bound = SCREEN_HEIGHT;
    
    if (missed_column_height >= 0) {
        framebuffer_row = (size_t)missed_column_height;
        framebuffer_bound = SCREEN_HEIGHT - (size_t)missed_column_height;
    }
    
    for (; framebuffer_row < framebuffer_bound; framebuffer_row++) {
        s_framebuffer[
            (framebuffer_row * DUAL_SCREEN_WIDTH) + (SCREEN_WIDTH + column)
        ] = (intersected_side) ? color_dimmed_intersected : color_intersected;
    }
	
}

void performRaycasting(void) {
			
	size_t ray = 0u;
    
    const double left_ray_dir_s = s_dir_s - s_cam_plane_s;
    const double left_ray_dir_t = s_dir_t - s_cam_plane_t;
	
	for (; ray < SCREEN_WIDTH; ray++) {	
        
        const double ray_screen_ratio = (double)ray / 128.;
        castRay(
            left_ray_dir_s + s_cam_plane_s * ray_screen_ratio,
            left_ray_dir_t + s_cam_plane_t * ray_screen_ratio,
            ray
        );
		
	}
	
}


void swapBuffersWindow(void) {
	
	// https://github.com/QuantitativeBytes/qbRayTrace/blob/main/Ep1Code/qbRayTrace/qbImage.hpp
	// https://github.com/QuantitativeBytes/qbRayTrace/blob/main/Ep1Code/qbRayTrace/qbImage.cpp
	
	// https://www.youtube.com/watch?v=rB8N5cFCHLQ
	
	SDL_UpdateTexture(g_texture, NULL, (void*)s_framebuffer, sizeof(*s_framebuffer) * DUAL_SCREEN_WIDTH);
	
	// https://wiki.libsdl.org/SDL3/SDL_RenderTexture
	SDL_FRect srcrect, dstrect;
	srcrect.x = dstrect.x = srcrect.y = dstrect.y = 0.f;
	srcrect.w = dstrect.w = (float)DUAL_SCREEN_WIDTH;
	srcrect.h = dstrect.h = (float)SCREEN_HEIGHT;
	
	SDL_RenderTexture(g_renderer, g_texture, &srcrect, &dstrect);
	SDL_RenderPresent(g_renderer);

}

void detectCollision(const double previous_player_pos_s, const double previous_player_pos_t) {
    
    // s_player_pos_s += s_dir_s * g_delta_time;
    
    size_t player_pos_s_on_framebuffer = (size_t)floor(s_player_pos_s);
    size_t player_pos_t_on_framebuffer = (size_t)floor(previous_player_pos_t);
    
    if (s_position_buffer[
        player_pos_t_on_framebuffer * DUAL_SCREEN_WIDTH + player_pos_s_on_framebuffer
    ] == 1u) {
        
        s_player_pos_s = previous_player_pos_s;
       
    }
    
    player_pos_t_on_framebuffer = (size_t)floor(s_player_pos_t);
    
    if (s_position_buffer[
        player_pos_t_on_framebuffer * DUAL_SCREEN_WIDTH + player_pos_s_on_framebuffer
    ] == 1u) {
        
        s_player_pos_t = previous_player_pos_t;
       
    }
    
}
