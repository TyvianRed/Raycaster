/* raycast.h */
#ifndef RAYCAST_H
#define RAYCAST_H

#include <stdio.h>
/* #include <stdint.h> */
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

#define SCREEN_HEIGHT 240
#define SCREEN_WIDTH 256

#define EPSILON 1E-9

#define IS_RENDERING(flags) (!(flags & 1 || flags & 2))

#define DEGREE_TO_RADIAN(degree) (degree * M_PI / 180.)

void getHorizontalVector(const double rayDirS, const double rayDirT, double* const horzVecSPtr, double* const horzVecTPtr);

/* color to bit pattern */
Uint32 ctob(const Uint32 r, const Uint32 g, const Uint32 b, const Uint32 a);

/* bit pattern to color */
void btoc(const Uint32 color, Uint8* const r,  Uint8* const g, Uint8* const b, Uint8* const a);

void swapBuffersPPM(Uint32* bufPtr, const size_t bufHeight, const size_t bufWidth);
void swapBuffersWindow(SDL_Renderer* renderer, SDL_Texture* texture, Uint32* bufPtr, const size_t bufHeight, const size_t bufWidth);

void drawMap(Uint32* const frameBufPtr, Uint8* const posBufPtr, const size_t bufHeight, const size_t bufWidth, Uint8* const mapPtr, const size_t mapHeight, const size_t mapWidth);

/* void drawPlayer(Uint32* const bufPtr, const size_t bufHeight, const size_t bufWidth, const size_t mapHeight, const size_t mapWidth, const double playerPosS, const double playerPosT); */
void drawPlayer(Uint32* const bufPtr, const size_t bufHeight, const size_t bufWidth, const double playerPosS, const double playerPosT);

/* int isRendering(const Uint8 flags); */

void castRay(Uint32* const frameBufPtr, Uint8* const posBufPtr, const size_t frameBufWidth, const double playerPosS, const double playerPosT, const double rayDirS, const double rayDirT);

void fillBackground(Uint32* const frameBufPtr, const size_t frameBufHeight, const size_t frameBufWidth);

void lookAt(double* const dirSPtr, double* const dirTPtr, double* const camLineS, double* const camLineT, const double radian);

void performRaycasting(Uint32* const frameBufPtr, Uint8* const posBufPtr, const size_t frameBufWidth, const double playerPosS, const double playerPosT, const double rayDirS, const double rayDirT, const double camLineS, const double camLineT);

void handleEvent(
	Uint8* const isExitClickedPtr, Uint8* const isESCPressedPtr,
	double* const playerPosSPtr, double* const playerPosTPtr, double* const playerYawPtr,
	double* const rayDirSPtr, double* const rayDirTPtr,
	double* const camLineSPtr, double* const camLineTPtr
);

#endif
/* RAYCAST_H */
