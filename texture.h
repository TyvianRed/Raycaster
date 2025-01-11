#ifndef TEXTURE_H
#define TEXTURE_H

#include "raycast.h"

// #include "SDL3/SDL_surface.h"
#include "SDL3/SDL_error.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_surface.h"

// https://github.com/ssloy/tinyraycaster/blob/master/textures.h

void loadTexture(const char* file_path);
void unloadTexture(void);

Uint32 filterTexture(const double intersected_s, const double intersected_t);

#endif /* TEXTURE_H */
