
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>


#include <SDL2/SDL.h>

#include "handmade.c"

#define BYTES_P_PIXEL 4

#define PI32 3.14159265358979f


void update_window_SDL(SDL_Renderer *renderer, SDL_Texture *texture, PixelBuffer *pixBuff) {
    SDL_RenderClear(renderer);
    SDL_UpdateTexture(texture, 0, pixBuff->pixels, pixBuff->pitch);
    SDL_RenderCopy(renderer, texture, 0, 0);
    SDL_RenderPresent(renderer);
}

void audio_callback_SDL(void *userData, u8 *audioData, int length) {
    memset(audioData, 0, length);
}

void handle_window_resize_SDL(PixelBuffer *pixBuff, SDL_Texture **texture, SDL_Renderer *renderer, int width, int height) {
    pixBuff->pitch = width * 4;
    if (texture) {
        SDL_DestroyTexture(*texture);
        *texture=0;
    }
    if (pixBuff->pixels) {
        free(pixBuff->pixels);
        pixBuff->pixels=0;
    }

    *texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_STREAMING,
        width,
        height
    );
    pixBuff->width = width;
    pixBuff->pitch = width * BYTES_P_PIXEL;
    pixBuff->height = height;
    pixBuff->pixels = malloc(height * pixBuff->pitch);
}

int main(int argc, char *argv[]) {
    
    int samplesPerSecond = 48000;
    int bytesPerSample = sizeof(i16) * 2;
    int latencySampleCount = samplesPerSecond / 16;
    int targetQueueBytes = latencySampleCount * bytesPerSample;
    int bytesToWrite = 2 * targetQueueBytes;

    GameSoundBuffer soundBuffer;
    soundBuffer.samplesPerSecond = samplesPerSecond;
    soundBuffer.samples = malloc(2 * targetQueueBytes);


    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) != 0) {
        printf("Error initializing!\n");
        exit(1);    
    }

    SDL_Window *window;
    window = SDL_CreateWindow(
        "Handmade Hero",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        640,
        480,
        SDL_WINDOW_RESIZABLE
    );

    // Sound setup
    SDL_AudioSpec audioSettings = {0};

    audioSettings.freq = samplesPerSecond;
    audioSettings.format = AUDIO_S16SYS;
    audioSettings.channels = 2;
    audioSettings.samples = 4096;
    audioSettings.callback = 0;

    //SDL_OpenAudio(&audioSettings, 0);

    if (audioSettings.format != AUDIO_S16SYS) {
        printf("Error getting audio buffer!\n");
    }

    //SDL_QueueAudio(1, (void *) soundBuffer.samples, bytesToWrite);

    //SDL_PauseAudio(0);


    SDL_GameController *controller = 0;
    int numSticks = SDL_NumJoysticks();
    for (int i=0; i < numSticks; i++) {
        if (SDL_IsGameController(i)) {
            controller = SDL_GameControllerOpen(i);
            break;
        }
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
    int width, height;
    SDL_GetWindowSize(window, &width, &height);
    int pitch = width * BYTES_P_PIXEL;
    SDL_Texture *texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_STREAMING,
        width,
        height
    );
    u8 *pixels = malloc(height * pitch);
    PixelBuffer pixBuff = {
        .pixels = pixels,
        .width = width,
        .height = height,
        .pitch = pitch,
    };

    // Timing
    u64 perfCountFreq = SDL_GetPerformanceFrequency();
    u64 lastCounter = SDL_GetPerformanceCounter();

    u64 endCounter;
    u64 counterElapsed;
    double msPerFrame;
    double fps;

    //u64 lastCycleCount = _rdtsc();

    //u64 endCycleCount = _rdtsc();
    u64 cyclesElapsed;
    double mcpf;

    SDL_Event event;
    int quit = 0;
    while (!quit) {
        while (SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_QUIT:
                    printf("SDL_QUIT\n");
                    quit = 1;
                    break;
                case SDL_WINDOWEVENT:
                    switch(event.window.event) {
                        case SDL_WINDOWEVENT_RESIZED:
                            width = event.window.data1;
                            height = event.window.data2;
                            handle_window_resize_SDL(&pixBuff, &texture, renderer, width, height);
                            break;
                    }
                    break;
                case SDL_JOYAXISMOTION:
                    if ((event.jaxis.value < -3200) || (event.jaxis.value > 3200)) {
                        switch(event.jaxis.axis) {
                            case 0:
                                if (event.jaxis.value < 0) {
                                    printf("Left Stick Left\n");
                                }
                                else if (event.jaxis.value > 0) {
                                    printf("Left Stick Right\n");
                                }
                                break;
                            case 1:
                                if (event.jaxis.value < 0) {
                                    printf("Left Stick Up\n");
                                }
                                else if (event.jaxis.value > 0) {
                                    printf("Left Stick Down\n");
                                }
                                break;
                            case 2:
                                printf("Left Trigger\n");
                                break;
                            case 3:
                                if (event.jaxis.value < 0) {
                                    printf("Right Stick Left\n");
                                }
                                else if (event.jaxis.value > 0) {
                                    printf("Right Stick Right\n");
                                }
                                break;
                            case 4:
                                if (event.jaxis.value < 0) {
                                    printf("Right Stick Up\n");
                                }
                                else if (event.jaxis.value > 0) {
                                    printf("Right Stick Down\n");
                                }
                                break;
                            case 5:
                                printf("Right Trigger\n");
                                break;
                        }
                    }
                    break;
                case SDL_JOYBUTTONDOWN:
                    printf("Button: %d\n", event.jbutton.button);
                    break;
                case SDL_KEYDOWN:
                case SDL_KEYUP:
                    printf("Char: %d\n", event.key.keysym.sym);

            } 

        }
        bytesToWrite = 2 * targetQueueBytes - SDL_GetQueuedAudioSize(1);

        // timing, input, pixBuff, soundBuffer
        gameUpdateAndRender(&pixBuff, &soundBuffer, bytesToWrite);
        update_window_SDL(renderer, texture, &pixBuff);
        //SDL_QueueAudio(1, (void *) soundBuffer.samples, 0);

        endCounter = SDL_GetPerformanceCounter();
        counterElapsed = endCounter - lastCounter;
        msPerFrame = (((1000.0f * (double) counterElapsed) / (double) perfCountFreq));
        fps = (double) perfCountFreq / (double) counterElapsed;

        //endCycleCount = _rdtsc();
        //cyclesElapsed = endCycleCount - lastCycleCount;
        //mcpf = ((double) cyclesElapsed / (1000.0f * 1000.0f));
        //lastCycleCount = endCycleCount;

        printf("%.02f ms/f, %.02ffps, %.02f (M)cycles\n", msPerFrame, fps, mcpf);

        lastCounter = endCounter;
    }

    if (controller) {
        SDL_GameControllerClose(controller);
    }

    //SDL_CloseAudio();
    SDL_Quit();
    return 0;
}




