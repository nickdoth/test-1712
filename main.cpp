#include <iostream>
#include <fstream>
#include <utility>
#include <ui.h>
#include <portaudio.h>

#include "dummies.h"

typedef struct THPlayback {
    uint16_t* offset_start;
    uint16_t* offest_end;
    uint16_t* offset_loop_point;
    uint16_t* data_pointer;
} THPlayback;


/**
 * *Touhou Project Playback Mechanism
 * BGM = 宇宙巫女現る, 0x10,    0x124B00, 0x124B10,    0x17AF050
 *                   start    loop_len  loop_start   main_len
 * total_len = main_len + loop_len
 * When total_len was played, the BGM cursor go back to loop_start
 */

int initAudio();
int initUi();

int main(void) {
    PaError err = Pa_Initialize();
    if (err != paNoError) {
        return 1;
    }

    size_t bgm_size = _BGM_LEN;
    char* data = new char[bgm_size];
    
    std::ifstream th_bgm(FILE_NAME, std::ios::binary);
    th_bgm.seekg(_BGM_START, std::ios::beg);
    th_bgm.read((char*) data, bgm_size);
    th_bgm.close();

    THPlayback* playback = new THPlayback;
    playback->offset_start = (uint16_t*) (data);
    playback->offset_loop_point = (uint16_t*) (data + _BGM_LOOP_POINT);
    playback->data_pointer = (uint16_t*) (data);
    playback->offest_end = (uint16_t*) (data + bgm_size);

    PaStream* mainStream;

    Pa_OpenDefaultStream(
        &mainStream,
        0,
        2,
        paInt16,
        44100,
        paFramesPerBufferUnspecified,
        [] (const void *inputBuffer,
                void *outputBuffer,
                unsigned long framesPerBuffer,
                const PaStreamCallbackTimeInfo* timeInfo,
                PaStreamCallbackFlags statusFlags,
                void *userData
            ) {
                int16_t* out = (int16_t*) outputBuffer;
                THPlayback* pb = (THPlayback*) userData;
                for (unsigned long i = 0L; i < framesPerBuffer; ++i) {
                    *out++ = *(pb->data_pointer++);
                    *out++ = *(pb->data_pointer++);
                    if (pb->data_pointer >= pb->offest_end) {
                        pb->data_pointer = pb->offset_loop_point;
                    }
                }
                
                return 0;
            },
        playback
    );

    Pa_StartStream(mainStream);

    getchar();
    
    return 0;
}

int initAudio() {
    return 0;
}

int initUi() {
    uiInitOptions options = uiInitOptions();
	const char *err;

	err = uiInit(&options);
	if (err != NULL) {
		std::cout << "error initializing libui: " << err << std::endl;
		delete err;
        err = nullptr;
		return 1;
	}


    uiWindow *mainWindow = uiNewWindow("Hello LibUI!", 840, 640, false);
    
    uiWindowOnClosing(mainWindow, [](uiWindow* w, void* _) {
        uiQuit();
        return 0;
    }, nullptr);

    uiControlShow(uiControl(mainWindow));
    uiMain();
    return 0;
}