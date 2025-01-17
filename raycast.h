/* raycast.h */
#ifndef RAYCAST_H
#define RAYCAST_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <float.h>

#define _USE_MATH_DEFINES
#include <math.h>

#include "texture.h"

#include "SDL3/SDL.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_timer.h"

#define SCREEN_HEIGHT (240u)
#define SCREEN_WIDTH (256u)
#define SCREEN_BUF_SIZE (61440u)
#define DUAL_SCREEN_WIDTH (512u)
#define DUAL_SCREEN_BUF_SIZE (122880u)

#define COLOR_WALL_BLUE (0x7FFFu)
#define COLOR_DIMMED_WALL_BLUE (0x3FFFu)
#define COLOR_RAY_WHITE (0xFFFFFFFFu)
#define COLOR_PLAYER_BLACK (0xFFu)

#define EPSILON (1E-9)

#define SDL_LOG_ERROR_STR(msg) SDL_LogError(SDL_LOG_CATEGORY_ERROR, msg, SDL_GetError())
#define SDL_LOG_ERROR(msg) SDL_LogError(SDL_LOG_CATEGORY_ERROR, msg)

// RGB to bit pattern
// https://github.com/QuantitativeBytes/qbRayTrace/blob/main/Ep1Code/qbRayTrace/qbImage.cpp
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
#define CTOB(r, g, b, a) (a << 24 | b << 16 | g << 8 | r)
#else
#define CTOB(r, g, b, a) (r << 24 | g << 16 | b << 8 | a)
#endif

typedef union {
    Uint8 should_exit;
    struct {
        Uint8 is_exit_clicked : 1;
        Uint8 is_esc_pressed : 1;
    } flags;
} exitflag_t;

extern exitflag_t g_exit_flag;

extern double g_delta_time;

extern SDL_Window* g_window;
extern SDL_Renderer* g_renderer;
extern SDL_Texture* g_texture;

void initializeRaycaster(void);
void createWindow(void);
void quitRaycaster(void);

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
