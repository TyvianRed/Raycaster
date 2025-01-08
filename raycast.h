/* raycast.h */
#ifndef RAYCAST_H
#define RAYCAST_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <float.h>

#define _USE_MATH_DEFINES
#include <math.h>

#include "SDL3/SDL.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_stdinc.h"

#define TRUE 1
#define FALSE 0

#define SCREEN_HEIGHT (240)
#define SCREEN_WIDTH (256)
#define DUAL_SCREEN_WIDTH (512)
/* #define SCREEN_BUF_SIZE (61440) */ /* SCREEN_HEIGHT * SCREEN_WIDTH */
#define SCREEN_BUF_SIZE (122880)

#define EPSILON 1E-9

#define IS_RENDERING(flags) (!(flags & 1 || flags & 2))

#define DEGREE_TO_RADIAN(degree) (degree * M_PI / 180.)


extern Uint8 isExitClicked, isESCPressed;

extern SDL_Window* g_window;
extern SDL_Renderer* g_renderer;
extern SDL_Texture* g_texture;


void createWindow(void);
void quitRaycaster(void);

Uint32 ctob(
	const Uint32 r, const Uint32 g, const Uint32 b, const Uint32 a
); /* RGB to bit pattern */
void btoc(
	const Uint32 color, Uint8* const r,  Uint8* const g, Uint8* const b, Uint8* const a
); /* Bit pattern to RGB */

void handleEvent(void);
void fillBackground(void);
void drawMap(const Uint8* const p_map, const size_t map_height, const size_t map_width);
void drawPlayer(void);

void lookAt(const double radian);
void getHorizontalVector(double* const p_horz_vec_s, double* const p_horz_vec_t);

void castRay(const double ray_dir_s, const double ray_dir_t, const size_t column);
void performRaycasting(void);

void swapBuffersWindow(void);

#endif
/* RAYCAST_H */
