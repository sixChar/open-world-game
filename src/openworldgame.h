#ifndef HANDMADE_H

#define BYTES_P_PIXEL 4
#define PI32 3.14159265358979f


typedef uint8_t u8;
typedef uint32_t u32;
typedef uint32_t u64;

typedef int16_t i16;

typedef struct PixelBuffer {
    u8 *pixels;
    int width;
    int height;
    int pitch;
} PixelBuffer;


typedef struct GameSoundBuffer {
    int sampleCount;
    int samplesPerSecond;
    i16 *samples;
} GameSoundBuffer;



/*
 * Services the platform layer provides to the game
 */


/*
 * Services that the game provides platform layer
 */
void gameUpdateAndRender(PixelBuffer *pixBuff, GameSoundBuffer *soundBuffer, int samplesToWrite);




#define HANDMADE_H 1
#endif
