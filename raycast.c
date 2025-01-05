#include "raycast.h"

/* color to bit pattern */
Uint32 ctob(const Uint32 r, const Uint32 g, const Uint32 b, const Uint32 a) {

	/* https://github.com/QuantitativeBytes/qbRayTrace/blob/main/Ep1Code/qbRayTrace/qbImage.cpp */

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	return a << 24 | b << 16 | g << 8 | r;
#else
	/* little endian */
	return r << 24 | g << 16 | b << 8 | a;
#endif
	
}


/* bit pattern to color */
void btoc(const Uint32 color, Uint8* const r,  Uint8* const g, Uint8* const b, Uint8* const a) {
	
	assert(r != NULL && g != NULL && b != NULL && a != NULL);
	
    // red green blue alpha
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    *a = color >> 24;
    *b = (color >> 16) & 255;
    *g = (color >> 8) & 255;
    *r = color & 255;
#else
	/* little endian */
	*r = color >> 24;
    *g = (color >> 16) & 255;
    *b = (color >> 8) & 255;
    *a = color & 255;
#endif
}


void swapBuffersPPM(Uint32* bufPtr, const size_t bufHeight, const size_t bufWidth) {
	
	assert(bufPtr != NULL);
	
	FILE* imageFile = NULL;
    fopen_s(&imageFile, "image.ppm", "w");
	
	fprintf(imageFile, "P3\n%zu %zu\n255\n", bufWidth, bufHeight);

	size_t i, j;
	for (i = 0; i < bufHeight; i++) {
		
		for (j = 0; j < bufWidth; j++) {
			
			Uint8 red, green, blue, alpha;
			btoc(bufPtr[i * bufWidth + j], &red, &green, &blue, &alpha);
			fprintf(imageFile, "%d %d %d\n", red, green, blue);
			
		}
	}
	
	fclose(imageFile);

}

void swapBuffersWindow(SDL_Renderer* renderer, SDL_Texture* texture, Uint32* bufPtr, const size_t bufHeight, const size_t bufWidth) {
	
	assert(renderer != NULL && texture != NULL && bufPtr != NULL);
	
	/* https://github.com/QuantitativeBytes/qbRayTrace/blob/main/Ep1Code/qbRayTrace/qbImage.hpp */
	/* https://github.com/QuantitativeBytes/qbRayTrace/blob/main/Ep1Code/qbRayTrace/qbImage.cpp */
	
	/* https://www.youtube.com/watch?v=rB8N5cFCHLQ */
	
	
	/* https://wiki.libsdl.org/SDL3/SDL_UpdateTexture */
	SDL_UpdateTexture(texture, NULL, (void*)bufPtr, bufWidth * sizeof(Uint32));
	
	/* https://wiki.libsdl.org/SDL3/SDL_RenderTexture */
	SDL_FRect srcrect, dstrect;
	srcrect.x = dstrect.x = 0.f;
	srcrect.y = dstrect.y = 0.f;
	srcrect.w = dstrect.w = (float)bufWidth;
	srcrect.h = dstrect.h = (float)bufHeight;
	
	SDL_RenderTexture(renderer, texture, &srcrect, &dstrect);

}


void drawMap(Uint32* const frameBufPtr, Uint8* const posBufPtr, const size_t bufHeight, const size_t bufWidth, Uint8* const mapPtr, const size_t mapHeight, const size_t mapWidth) {

	assert(frameBufPtr != NULL && posBufPtr != NULL);
	assert(bufHeight >= mapHeight & bufWidth >= mapWidth);
	assert(bufHeight % mapHeight == 0 && bufWidth % mapWidth == 0);
	
	double invMapHeight = 1.f / mapHeight;
	double invMapWidth = 1.f / mapWidth;
	
	Uint32 wallColor = ctob(0u, 0u, 127u, 255u);

	/* size_t dHB = bufHeight * invMapHeight; */
	/* size_t dWB = bufWidth * invMapWidth; */
	
	size_t i, j;
	for (i = 0; i < mapHeight; i++) {
			
		for (j = 0; j < mapWidth; j++) {
		
			if (mapPtr[i * mapWidth + j] == 0) continue;
				
			size_t verticalEnd = (i + 1) * invMapHeight * bufHeight;
			size_t horizontalEnd = (j + 1) * invMapWidth * bufWidth;
			
			size_t dHM, dWM;
			
			for (dHM = i * invMapHeight * bufHeight; dHM < verticalEnd; dHM++) {
					
				for (dWM = j * invMapWidth * bufWidth; dWM < horizontalEnd; dWM++) {
					
					frameBufPtr[dHM * bufWidth + dWM] = wallColor;
					
					if (posBufPtr == NULL) {
						fprintf(stderr, "drawMap(): warning: posBufPtr is NULL\n");
						continue;
					}
					
					posBufPtr[dHM * bufWidth + dWM] = 1u;
					/* fprintf(stderr, "drawMap(): posBufPtr[dHM * bufWidth + dWM] = %d\n", posBufPtr[dHM * bufWidth + dWM]); */
					
				}
				
			}
			
		}
		
	}
	
}

/*
int isRendering(const Uint8 flags) {
	
	return !(flags & 1 || flags & 2);
	
}
*/


void drawPlayer(Uint32* const bufPtr, const size_t bufHeight, const size_t bufWidth, const double playerPosS, const double playerPosT) {
	
	assert(bufPtr != NULL);
	assert((double)bufHeight > playerPosT && (double)bufWidth > playerPosS);
	
	size_t s = (size_t)floorf(playerPosS);
	size_t t = (size_t)floorf(playerPosT);
	size_t playerLocation =  t * SCREEN_WIDTH + s;
	
	/* fprintf(stderr, "drawPlayer(): s = %zu, t = %zu\n", s, t); */
	
	bufPtr[playerLocation] = ctob(0u, 0u, 0u, 255u);
	
}

void castRay(Uint32* const frameBufPtr, Uint8* const posBufPtr, const size_t frameBufWidth, const double playerPosS, const double playerPosT, const double rayDirS, const double rayDirT) {

	assert(frameBufPtr != NULL && posBufPtr != NULL);

	/* https://lodev.org/cgtutor/raycasting.html */
	/* https://www.youtube.com/watch?v=NbSee-XM7WA */
	
	/* const Uint32 wallBlue = ctob(0u, 0u, 127u, 255u); */
	const Uint32 rayWhite = ctob(255u, 255u, 255u, 255u);
	
	int intersectedSide = -1; /* 0 for horizontal grid line, 1 for vertical grid line */
	
	int frameBufS = (int)floorf(playerPosS), frameBufT = (int)floorf(playerPosT);
	
	int stepDirS = 0, stepDirT = 0;
	
	const double deltaDistS = (rayDirS) ? fabs(1./rayDirS) : DBL_MAX;
	const double deltaDistT = (rayDirT) ? fabs(1./rayDirT) : DBL_MAX;
	
	double sideDistS = 0., sideDistT = 0.;
	
	if (rayDirS > 0.f) {
		
		sideDistS = (1.0 - playerPosS + frameBufS) * deltaDistS; 
		stepDirS = 1;
		
	} else {
		
		sideDistS = (playerPosS - frameBufS) * deltaDistS;
		stepDirS = -1;
		
	}
	
	if (rayDirT > 0.f) {
		
		sideDistT = (1.0 - playerPosT + frameBufT) * deltaDistT; 
		stepDirT = 1;
		
	} else {
		
		sideDistT = (playerPosT - frameBufT) * deltaDistT; 
		stepDirT = -1;
		
	}
	
	/* fprintf(stderr, "main(): (posBuffer[framebufferT * SCREEN_WIDTH + framebufferS] != 1u) == %d\n", posBuffer[framebufferT * SCREEN_WIDTH + framebufferS] != 1u); */
	
	while(posBufPtr[frameBufT * frameBufWidth + frameBufS] != 1u) { /* 1u in posBufPtr is a wall */
		
		/* static int whileCount = 0; */
		
		/* fprintf(stderr, "main(): while loop running... %d\n", whileCount++); */
		
		if (sideDistS > sideDistT) {
			
			sideDistT += deltaDistT;
			frameBufT += stepDirT;
			intersectedSide = 1;
			
		} else {
			
			sideDistS += deltaDistS;
			frameBufS += stepDirS;
			intersectedSide = 0;
			
		}
		
		frameBufPtr[frameBufT * frameBufWidth + frameBufS] = rayWhite;
		
	}
	
}

void fillBackground(Uint32* const frameBufPtr, const size_t frameBufHeight, const size_t frameBufWidth) {
	
	assert(frameBufPtr != NULL);
	
	size_t i, j;
	for (i = 0; i < frameBufHeight; i++) {
	
		for (j = 0; j < frameBufWidth; j++) frameBufPtr[i * frameBufWidth + j] = ctob((Uint32)i, (Uint32)j, 0u, 255u);
		
	}
	
}

void getHorizontalVector(const double rayDirS, const double rayDirT, double* const horzVecSPtr, double* const horzVecTPtr) {
	
	assert(horzVecSPtr != NULL && horzVecTPtr != NULL);
	
	*horzVecSPtr = rayDirT;
	*horzVecTPtr = -rayDirS;
	
}

void lookAt(double* const dirSPtr, double* const dirTPtr, double* const camLineS, double* const camLineT, const double radian) {
	
	/*
	const double tempDirS = *dirSPtr * cos(radian) - *dirTPtr * sin(radian);
	const double tempDirT = *dirSPtr * sin(radian) + *dirTPtr * cos(radian);
	*dirSPtr = tempDirS;
	*dirTPtr = tempDirT;
	*/
		
	*dirSPtr = cos(radian);
	*dirTPtr = sin(radian);

	*camLineS = -sin(radian);
	*camLineT = cos(radian);

}

void performRaycasting(Uint32* const frameBufPtr, Uint8* const posBufPtr, const size_t frameBufWidth, const double playerPosS, const double playerPosT, const double rayDirS, const double rayDirT, const double camLineS, const double camLineT) {
			
	size_t ray;
	
	castRay(frameBufPtr, posBufPtr, frameBufWidth, playerPosS, playerPosT, rayDirS, rayDirT);
	 
	for (ray = 0u; ray < 128u; ray++) {	
		
		double leftRayDirS = rayDirS - camLineS * ray / 256;
		double leftRayDirT = rayDirT - camLineT * ray / 256;
		
		double rightRayDirS = rayDirS + camLineS * ray / 256;
		double rightRayDirT = rayDirT + camLineT * ray / 256;
		
		castRay(frameBufPtr, posBufPtr, frameBufWidth, playerPosS, playerPosT, leftRayDirS, leftRayDirT);
		castRay(frameBufPtr, posBufPtr, frameBufWidth, playerPosS, playerPosT, rightRayDirS, rightRayDirT);
						
	}
	
}

void handleEvent(
	Uint8* const isExitClickedPtr, Uint8* const isESCPressedPtr,
	double* const playerPosSPtr, double* const playerPosTPtr, double* const playerYawPtr,
	double* const rayDirSPtr, double* const rayDirTPtr,
	double* const camLineSPtr, double* const camLineTPtr
) {
	
	SDL_Event event;

	while (SDL_PollEvent(&event)) {
		
		*isExitClickedPtr = (SDL_EVENT_QUIT == event.type) ? 1u : 0u;

		/* https://wiki.libsdl.org/SDL3/BestKeyboardPractices */
		*isESCPressedPtr = (SDL_EVENT_KEY_DOWN == event.type && event.key.key == SDLK_ESCAPE) ? 2u : 0u;
		
		if (SDL_EVENT_KEY_DOWN == event.type) {
		
			if (event.key.key == SDLK_W) {
				
				*playerPosSPtr += *rayDirSPtr;
				*playerPosTPtr += *rayDirTPtr;
				
			} else if (event.key.key == SDLK_S) {
			
				*playerPosSPtr -= *rayDirSPtr;
				*playerPosTPtr -= *rayDirTPtr;
			
			} else if (event.key.key == SDLK_A) {
				
				double horzVecS = 0., horzVecT = 0.;
				getHorizontalVector(*rayDirSPtr, *rayDirTPtr, &horzVecS, &horzVecT);
			
				*playerPosSPtr += horzVecS;
				*playerPosTPtr += horzVecT;
			
			} else if (event.key.key == SDLK_D) {
				
				double horzVecS = 0., horzVecT = 0.;
				getHorizontalVector(*rayDirSPtr, *rayDirTPtr, &horzVecS, &horzVecT);
			
				*playerPosSPtr -= horzVecS;
				*playerPosTPtr -= horzVecT;
			
			} else if (event.key.key == SDLK_LEFT) {
			
				*playerYawPtr -= 1.;
				if (*playerYawPtr < 0.) *playerYawPtr += 360.;
				lookAt(rayDirSPtr, rayDirTPtr, camLineSPtr, camLineTPtr, DEGREE_TO_RADIAN(*playerYawPtr));
				
			} else if (event.key.key == SDLK_RIGHT) {
				 
				*playerYawPtr += 1.;
				if (*playerYawPtr > 0.) *playerYawPtr -= 360.;
				lookAt(rayDirSPtr, rayDirTPtr, camLineSPtr, camLineTPtr, DEGREE_TO_RADIAN(*playerYawPtr));
				
			}
		
		}
		
	}	
	
}
