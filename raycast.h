/* raycast.h */
#ifndef RAYCAST_H
#define RAYCAST_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <float.h>

#define _USE_MATH_DEFINES
#include <math.h>

#include "SDL3/SDL.h"
#include "SDL3/SDL_init.h"
// #include "SDL3/SDL_oldnames.h"
#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_timer.h"

#define SCREEN_HEIGHT (240u)
#define SCREEN_WIDTH (256u)
#define DUAL_SCREEN_WIDTH (512u)
#define SCREEN_BUF_SIZE (122880u)

#define EPSILON 1E-9

#define IS_RENDERING(flags) (!((flags) & 1u || (flags) & 2u))

#define DEGREE_TO_RADIAN(degree) (degree * M_PI / 180.)


extern Uint8 isExitClicked, isESCPressed;

extern double g_delta_time;

extern SDL_Window* g_window;
extern SDL_Renderer* g_renderer;
extern SDL_Texture* g_texture;


void createWindow(void);
void quitRaycaster(void);

// RGB to bit pattern
inline Uint32 ctob(const Uint32 r, const Uint32 g, const Uint32 b, const Uint32 a) {
	
	// https://github.com/QuantitativeBytes/qbRayTrace/blob/main/Ep1Code/qbRayTrace/qbImage.cpp
	
#if SDL_BYTEORDER == SDL_BIG_ENDIAN

	return a << 24 | b << 16 | g << 8 | r;
	
#else
	
	return r << 24 | g << 16 | b << 8 | a;

#endif

}

void btoc(
	const Uint32 color, Uint8* const r,  Uint8* const g, Uint8* const b, Uint8* const a
); // Bit pattern to RGB

void handleEvent(void);
void detectCollision(const double previous_player_pos_s, const double previous_player_pos_t);

void fillBackground(void);
void drawMap(const Uint8* const p_map, const size_t map_height, const size_t map_width);
void drawPlayer(void);

void lookAt(const double rotation_direction);
void getHorizontalVector(double* const p_horz_vec_s, double* const p_horz_vec_t);

void castRay(const double ray_dir_s, const double ray_dir_t, const size_t column);
void performRaycasting(void);

void swapBuffersWindow(void);

#endif
/* RAYCAST_H */
