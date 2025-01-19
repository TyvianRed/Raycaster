#ifndef TEXTURE_H
#define TEXTURE_H

#include "raycast.h"

// #include "SDL3/SDL_surface.h"
#include "SDL3/SDL_error.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_surface.h"

enum { NUM_TEXTURE = 6 };

extern Uint32* g_texture_color_buf;
extern size_t g_texture_width, g_texture_height;
extern size_t g_individual_texture_width;

// https://github.com/ssloy/tinyraycaster/blob/master/textures.h
void loadTexture(const char* file_path);
void unloadTexture(void);

#endif /* TEXTURE_H */
