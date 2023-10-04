
#include "handmade.h"


void gameOutputSound(GameSoundBuffer *soundBuffer, int sampleCountOut) {
    static float tSine = 0;
    i16 toneVolume = 1500;
    int toneHz = 256;
    int wavePeriod = soundBuffer->samplesPerSecond / toneHz;

    i16 *sampleOut = soundBuffer->samples;
    float sineVal;
    for(int sampleIndex = 0; sampleIndex < sampleCountOut; ++sampleIndex) {
        tSine += 2.0f * PI32 * 1.0f / ((float) wavePeriod);
        sineVal = sinf(tSine);
        i16 sampleValue = (i16) (sineVal * toneVolume);
        *sampleOut++ = sampleValue;
        *sampleOut++ = sampleValue;
    }

}


void drawGradient(PixelBuffer *pixBuff) {
    int offX = 0;
    int offY = 0;
    u32 * pixels = (u32*) pixBuff->pixels;
    for (int j=0; j < pixBuff->height; j++) {
        for (int i=0; i < pixBuff->width; i++) {
            *pixels++ = ((u8) (i+offX)) << 16 | ((u8) (j+offY)) << 8;
        }
    }
}


void gameUpdateAndRender(PixelBuffer *pixBuff, GameSoundBuffer *soundBuffer, int sampleCountOut) {
    // TODO: Allow sample offset for more robust options
    //gameOutputSound(soundBuffer, sampleCountOut);
    drawGradient(pixBuff);
}



