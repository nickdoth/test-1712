#include <memory>
#include <iostream>
#include <fstream>
#include <utility>
#include <ui.h>
#include <portaudio.h>

#include "dummies.h"

using std::unique_ptr;
using std::shared_ptr;

typedef struct THPlayback {
    uint16_t* offset_start;
    uint16_t* offest_end;
    uint16_t* offset_loop_point;
    uint16_t* data_pointer;
} THPlayback;

#define try_or(expr) if (!(expr))


/**
 * *Touhou Project Playback Mechanism
 * BGM = 宇宙巫女現る, 0x10,    0x124B00, 0x124B10,    0x17AF050
 *                   start    loop_len  loop_start   main_len
 * total_len = main_len + loop_len
 * When total_len was played, the BGM cursor go back to loop_start
 */

int initAudio(shared_ptr<THPlayback>);
int initUi();

int main(void) {
    const size_t bgm_size = _BGM_LEN;
    auto data = unique_ptr<char>(new char[bgm_size]);
    // auto data = new char[bgm_size];

    std::ifstream th_bgm(FILE_NAME, std::ios::binary);
    th_bgm.seekg(_BGM_START, std::ios::beg);
    th_bgm.read(data.get(), bgm_size);
    th_bgm.close();

    auto playback = shared_ptr<THPlayback>(new THPlayback);
    playback->offset_start = (uint16_t*) (data.get());
    playback->offset_loop_point = (uint16_t*) (data.get() + _BGM_LOOP_POINT);
    playback->data_pointer = (uint16_t*) (data.get());
    playback->offest_end = (uint16_t*) (data.get() + bgm_size);

    initAudio(playback);

    getchar();

    return 0;
}

int initAudio(shared_ptr<THPlayback> playback) {
    try_or(Pa_Initialize() == paNoError) {
        return 1;
    }

    PaStream* mainStream;

    try_or(Pa_OpenDefaultStream(
        &mainStream,
        0,
        2,
        paInt16,
        44100,
        paFramesPerBufferUnspecified,
        [] (const void* inputBuffer,
                void* outputBuffer,
                unsigned long framesPerBuffer,
                const PaStreamCallbackTimeInfo* timeInfo,
                PaStreamCallbackFlags statusFlags,
                void* userData
            ) {
                int16_t* out = (int16_t*) outputBuffer;
                THPlayback* pb = (THPlayback*) userData;
                for (unsigned long i = 0L; i < framesPerBuffer; ++i) {
                    *out++ = *(pb->data_pointer++);
                    *out++ = *(pb->data_pointer++);
                    if (pb->data_pointer >= pb->offest_end) {
                        pb->data_pointer = pb->offset_loop_point;
                        std::cout << "Loop Point" << std::endl;
                    }
                }
                
                return 0;
            },
        playback.get()
    ) == paNoError) {
        std::cerr << "Cannot create stream" << std::endl;
        Pa_CloseStream(mainStream);
        return 1;
    }

    try_or(Pa_StartStream(mainStream) == paNoError) {
        std::cerr << "Cannot start streaming" << std::endl;
        Pa_CloseStream(mainStream);
        return 1;
    }

    // Pa_CloseStream(mainStream);
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