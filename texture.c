#include "texture.h"

Uint32* s_texture;
static size_t texture_width, texture_height;

void loadTexture(const char* file_path) {
    // https://github.com/ssloy/tinyraycaster/blob/master/textures.cpp
    SDL_Surface* const p_surface_image = SDL_LoadBMP(file_path);
    if (p_surface_image == NULL) {
        SDL_LOG_ERROR_STR("loadTexture(): failed to load image to p_surface_image\n%s\n");
        SDL_DestroySurface(p_surface_image);
        exit(EXIT_FAILURE);
    }
    
    SDL_Surface* const p_surface_converted_image = SDL_ConvertSurface(
        p_surface_image, SDL_PIXELFORMAT_RGBA8888
    );
    SDL_DestroySurface(p_surface_image);
    if (p_surface_converted_image == NULL) {
        SDL_LOG_ERROR_STR("loadTexture(): failed to load image to p_surface_converted_image\n%s\n");
        SDL_DestroySurface(p_surface_converted_image);
        exit(EXIT_FAILURE);
    }
    
    texture_width = p_surface_converted_image->w;
    if ((int)(texture_width * 4u) != p_surface_converted_image->pitch) {
        SDL_LOG_ERROR("loadTexture(): loaded image has invalid pitch\n");
        SDL_DestroySurface(p_surface_converted_image);
        exit(EXIT_FAILURE);
    }
    
    texture_height = p_surface_converted_image->h;
    if (texture_width & texture_height) {
        SDL_LOG_ERROR("loadTexture(): individual image isn't square\n");
        SDL_DestroySurface(p_surface_converted_image);
        exit(EXIT_FAILURE);
    }
    
    Uint8* const image = p_surface_converted_image->pixels;
    s_texture = malloc(texture_height * texture_width * sizeof(Uint32));
    for (size_t image_height = 0u; image_height < texture_height; image_height++) {
        for (size_t image_width = 0u; image_width < texture_width; image_width++) {
            const size_t texture_base_index = image_height * texture_width + image_width;
            const size_t color_base_index = texture_base_index * 4u;
            const Uint8 red = image[color_base_index];
            const Uint8 green = image[color_base_index + 1];
            const Uint8 blue = image[color_base_index + 2];
            const Uint8 alpha = image[color_base_index + 3];
            s_texture[texture_base_index] = CTOB(red, green, blue, alpha);
        }
    }
    
    SDL_DestroySurface(p_surface_converted_image);
    fprintf(stderr, "loadTexture(): texture loaded successfully\n");
}

void unloadTexture(void) {
    free(s_texture);
    fprintf(stderr, "loadTexture(): s_texture freed successfully\n");
}

Uint32 filterTexture(const double intersected_s, const double intersected_t) {
    return COLOR_PLAYER_BLACK;
}
