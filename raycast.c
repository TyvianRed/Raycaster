#include "raycast.h"

exitflag_t g_exit_flag;

static Uint8 s_position_buffer[SCREEN_BUF_SIZE];

static Uint32 s_framebuffer[DUAL_SCREEN_BUF_SIZE];

double g_player_pos_s, g_player_pos_t;
static double s_dir_s, s_dir_t;
static double s_cam_plane_s, s_cam_plane_t;

static const double S_ROTATION_SPEED = M_PI / 180.;

double g_delta_time;

SDL_Window* g_window;
SDL_Renderer* g_renderer;
SDL_Texture* g_texture;

void initializeRaycaster(void) {
    g_player_pos_s = 20.;
    g_player_pos_t = 25.;
    s_dir_s = 1.;
    s_cam_plane_t = 0.3;
    s_dir_t = s_cam_plane_s = 0.;
    memset(&g_exit_flag, 0, sizeof g_exit_flag);
    createWindow();
}

void createWindow(void) {
	// https://wiki.libsdl.org/SDL3/SDL_CreateWindow
	SDL_Init(SDL_INIT_VIDEO);

    g_window = SDL_CreateWindow("Raycaster", DUAL_SCREEN_WIDTH, SCREEN_HEIGHT, 0);
	if (g_window == NULL) {
        SDL_LOG_ERROR_STR("createWindow(): failed to create window.\n%s\n");
        goto createWindow_failed_to_create_window;
	}

    g_renderer = SDL_CreateRenderer(g_window, NULL);
	if (g_renderer == NULL) {
		SDL_LOG_ERROR_STR("createWindow(): failed to create renderer.\n%s\n");
        goto createWindow_failed_to_create_renderer;
	}
	
	if (!SDL_SetRenderDrawColor(g_renderer, 127u, 127u, 127u, 255u)) {
		SDL_LOG_ERROR_STR("createWindow(): failed to set render draw color.\n%s\n");
		goto createWindow_failed_to_create_renderer;
	}
	
    g_texture = SDL_CreateTexture(
		g_renderer,
		SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
		DUAL_SCREEN_WIDTH, SCREEN_WIDTH
	);
	if (g_texture == NULL) {
        SDL_LOG_ERROR_STR("createWindow(): failed to create texture.\n%s\n");
		goto createWindow_failed_to_create_texture;
	}
    
    return;
    assert(false); // the code below shouldn't run unless initializing SDL context failed
    
createWindow_failed_to_create_texture:
    SDL_DestroyTexture(g_texture);
createWindow_failed_to_create_renderer:
    SDL_DestroyRenderer(g_renderer);
createWindow_failed_to_create_window:
	SDL_DestroyWindow(g_window);
    SDL_Quit();
    exit(EXIT_FAILURE);
}

void quitRaycaster(void) {
	SDL_DestroyTexture(g_texture);
    SDL_DestroyRenderer(g_renderer);
	SDL_DestroyWindow(g_window);
	SDL_Quit();
}

void handleEvent(void) {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
        
		if (SDL_EVENT_QUIT == event.type) {
            g_exit_flag.flags.is_exit_clicked = 1;
            break;
        }
		// https://wiki.libsdl.org/SDL3/BestKeyboardPractices
		
		if (SDL_EVENT_KEY_DOWN == event.type) {  
            if (event.key.key == SDLK_ESCAPE) {
                g_exit_flag.flags.is_esc_pressed = 1;
                break;
            }
            
            if (event.key.key == SDLK_LEFT) {
                lookAt(-1.);
                continue;
			}
            
            if (event.key.key == SDLK_RIGHT) {  
                lookAt(1.);
                continue;
            }   
        
            const double previoug_player_pos_s = g_player_pos_s;
            const double previoug_player_pos_t = g_player_pos_t;
        
            if (event.key.key == SDLK_W) {
                g_player_pos_s += s_dir_s * g_delta_time;
                g_player_pos_t += s_dir_t * g_delta_time;
            } else if (event.key.key == SDLK_S) {
                g_player_pos_s -= s_dir_s * g_delta_time;
				g_player_pos_t -= s_dir_t * g_delta_time;
            } else if (event.key.key == SDLK_A) {
                double horz_vec_s, horz_vec_t;
				getHorizontalVector(&horz_vec_s, &horz_vec_t);
                
				g_player_pos_s += horz_vec_s * g_delta_time;
				g_player_pos_t += horz_vec_t * g_delta_time;
            } else if (event.key.key == SDLK_D) {
                double horz_vec_s, horz_vec_t; 
				getHorizontalVector(&horz_vec_s, &horz_vec_t);
                
				g_player_pos_s -= horz_vec_s * g_delta_time;
				g_player_pos_t -= horz_vec_t * g_delta_time;
            }
			
            detectCollision(previoug_player_pos_s, previoug_player_pos_t); 
		}
	}
}

void fillBackground(void) {
	for (size_t row = 0u; row < SCREEN_HEIGHT; row++) {
		for (size_t col = 0u; col < SCREEN_WIDTH; col++) {
            const color_t color_pixel = {
                .rgba.r = row,
                .rgba.g = col,
                .rgba.b = 0u,
                .rgba.a = 255u
            };
            const size_t screen_row = row * DUAL_SCREEN_WIDTH;
			s_framebuffer[screen_row + col] = color_pixel.bits;
            s_framebuffer[screen_row + col + SCREEN_WIDTH] = color_pixel.bits;
        }
    }
}

void drawMap(const Uint8* const p_map, const size_t map_height, const size_t map_width) {
	const double inv_map_height = 1.f / map_height;
	const double inv_map_width = 1.f / map_width;
	
	for (size_t row = 0u; row < map_height; row++) {
		for (size_t col = 0u; col < map_width; col++) {
			if (p_map[row * map_width + col] == 0) continue;
            
			const size_t vert_end = (row + 1) * inv_map_height * SCREEN_HEIGHT;
			const size_t horz_end = (col + 1) * inv_map_width * SCREEN_WIDTH;
			
			for (size_t dHM = row * inv_map_height * SCREEN_HEIGHT; dHM < vert_end; dHM++) {
				for (size_t dWM = col * inv_map_width * SCREEN_WIDTH; dWM < horz_end; dWM++) {
					s_framebuffer[dHM * DUAL_SCREEN_WIDTH + dWM] = COLOR_WALL_BLUE.bits;
					s_position_buffer[dHM * SCREEN_WIDTH + dWM] = 1u;
				}
			}
		}
	}
}

void drawPlayer(void) {
	const size_t player_buf_s = (size_t)floor(g_player_pos_s);
	const size_t player_buf_t = (size_t)floor(g_player_pos_t);
	const size_t player_location =  player_buf_t * DUAL_SCREEN_WIDTH + player_buf_s;
    
	s_framebuffer[player_location] = COLOR_PLAYER_BLACK.bits;
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
	side_t intersected_side;
	
	int step_s = 0, step_t = 0;
	
	size_t player_buf_s = (size_t)floor(g_player_pos_s);
	size_t player_buf_t = (size_t)floor(g_player_pos_t);
    
	bool is_wall_intersected = s_position_buffer[player_buf_t * SCREEN_WIDTH + player_buf_s] == 1u;
    
	const double delta_s = (ray_dir_s == 0.) ? DBL_MAX : fabs(1. / ray_dir_s);
	const double delta_t = (ray_dir_t == 0.) ? DBL_MAX : fabs(1. / ray_dir_t);
	
	double dist_s = 0., dist_t = 0.;
    
	if (ray_dir_s > 0.) {
		dist_s = (1.0 - g_player_pos_s + (double)player_buf_s) * delta_s; 
		step_s = 1;
	} else {
		dist_s = (g_player_pos_s - (double)player_buf_s) * delta_s;
		step_s = -1;
	}
	
	if (ray_dir_t > 0.) {
		dist_t = (1.0 - g_player_pos_t + (double)player_buf_t) * delta_t; 
		step_t = 1;
	} else {
		dist_t = (g_player_pos_t - (double)player_buf_t) * delta_t; 
		step_t = -1;
	}

	while(!is_wall_intersected) {
		if (dist_s > dist_t) {
			dist_t += delta_t;
			player_buf_t += step_t;
			intersected_side = SIDE_VERTICAL;
		} else {
			dist_s += delta_s;
			player_buf_s += step_s;
			intersected_side = SIDE_HORIZONTAL;
		}
		
		s_framebuffer[player_buf_t * DUAL_SCREEN_WIDTH + player_buf_s] = COLOR_RAY_WHITE.bits;
		is_wall_intersected = s_position_buffer[player_buf_t * SCREEN_WIDTH + player_buf_s] == 1u;
	}
    
    const double perpendicular_distance =
        intersected_side ? dist_t - delta_t : dist_s - delta_s;
    const size_t intersected_column_height =
        perpendicular_distance > EPSILON ? SCREEN_HEIGHT * 42 / perpendicular_distance : SCREEN_HEIGHT;
    
    const signed int missed_column_height = (SCREEN_HEIGHT - intersected_column_height) * 0.5;
    
    size_t framebuffer_row = 0;
    size_t framebuffer_bound = SCREEN_HEIGHT;
    
    if (missed_column_height >= 0) {
        framebuffer_row = (size_t)missed_column_height;
        framebuffer_bound = SCREEN_HEIGHT - (size_t)missed_column_height;
    }

    for (; framebuffer_row < framebuffer_bound; framebuffer_row++) {
        if (intersected_side == SIDE_HORIZONTAL) {
            s_framebuffer[framebuffer_row * DUAL_SCREEN_WIDTH + SCREEN_WIDTH + column] =
                COLOR_DIMMED_WALL_BLUE.bits;
            continue;
        }
        s_framebuffer[framebuffer_row * DUAL_SCREEN_WIDTH + SCREEN_WIDTH + column] = 
            COLOR_WALL_BLUE.bits;
        // s_framebuffer[framebuffer_row * DUAL_SCREEN_WIDTH + SCREEN_WIDTH + column] =
        //    texture[];
    }   
    
}

void performRaycasting(void) {
    const double left_ray_dir_s = s_dir_s - s_cam_plane_s;
    const double left_ray_dir_t = s_dir_t - s_cam_plane_t;
	
	for (size_t ray = 0u; ray < SCREEN_WIDTH; ray++) {	
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
	SDL_UpdateTexture(
        g_texture, NULL, (void*)s_framebuffer, sizeof(*s_framebuffer) * DUAL_SCREEN_WIDTH
    );
	
	// https://wiki.libsdl.org/SDL3/SDL_RenderTexture
	SDL_FRect srcrect, dstrect;
	srcrect.x = dstrect.x = srcrect.y = dstrect.y = 0.f;
	srcrect.w = dstrect.w = (float)DUAL_SCREEN_WIDTH;
	srcrect.h = dstrect.h = (float)SCREEN_HEIGHT;
	
	SDL_RenderTexture(g_renderer, g_texture, &srcrect, &dstrect);
	SDL_RenderPresent(g_renderer);
}

void detectCollision(const double previoug_player_pos_s, const double previoug_player_pos_t) {
    size_t player_buf_s = (size_t)floor(g_player_pos_s);
    size_t player_buf_t = (size_t)floor(previoug_player_pos_t);

    if (s_position_buffer[player_buf_t * SCREEN_WIDTH + player_buf_s] == 1u)
        g_player_pos_s = previoug_player_pos_s;
    
    player_buf_t = (size_t)floor(g_player_pos_t);
    
    if (s_position_buffer[player_buf_t * SCREEN_WIDTH + player_buf_s] == 1u)
        g_player_pos_t = previoug_player_pos_t;
}
