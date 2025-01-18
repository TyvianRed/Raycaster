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

enum {
    SCREEN_HEIGHT = 240,
    SCREEN_WIDTH = 256,
    SCREEN_BUF_SIZE = 61440,
    DUAL_SCREEN_WIDTH = 512,
    DUAL_SCREEN_BUF_SIZE = 122880
};

typedef enum {
    SIDE_HORIZONTAL = 0,
    SIDE_VERTICAL
} side_t;

#define EPSILON (1E-9)

#define SDL_LOG_ERROR_STR(msg) SDL_LogError(SDL_LOG_CATEGORY_ERROR, msg, SDL_GetError())
#define SDL_LOG_ERROR(msg) SDL_LogError(SDL_LOG_CATEGORY_ERROR, msg)

typedef union {
    Uint8 should_exit;
    struct {
        Uint8 is_exit_clicked : 1;
        Uint8 is_esc_pressed : 1;
    } flags;
} exitflag_t;

typedef union {
    Uint32 bits;
    struct {
    // https://github.com/QuantitativeBytes/qbRayTrace/blob/main/Ep1Code/qbRayTrace/qbImage.cpp
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
        Uint8 r;
        Uint8 g;
        Uint8 b;
        Uint8 a;
#else
        Uint8 a;
        Uint8 b;
        Uint8 g;
        Uint8 r;
#endif
    } rgba;
} color_t;

static const color_t COLOR_WALL_BLUE = {
    .rgba.r = 0u,
    .rgba.g = 0u,
    .rgba.b = 127u,
    .rgba.a = 255u
};

static const color_t COLOR_DIMMED_WALL_BLUE = {
    .rgba.r = 0u,
    .rgba.g = 0u,
    .rgba.b = 63u,
    .rgba.a = 255u
};

static const color_t COLOR_PLAYER_BLACK = {
    .rgba.r = 0u,
    .rgba.g = 0u,
    .rgba.b = 0u,
    .rgba.a = 0u
};

static const color_t COLOR_RAY_WHITE = {
    .rgba.r = 255u,
    .rgba.g = 255u,
    .rgba.b = 255u,
    .rgba.a = 255u
};

extern exitflag_t g_exit_flag;

extern double g_delta_time;

extern SDL_Window* g_window;
extern SDL_Renderer* g_renderer;
extern SDL_Texture* g_texture;

void initializeRaycaster(void);
void createWindow(void);
void quitRaycaster(void);

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
