#pragma once
#include "raylib.h"

class AudioManager {
public:
    void Init();
    void Shutdown();

    void PlayDigit(int digitIndex);
    void PlayOperator();
    void PlayEquals();
    void PlayClear();
    void PlayError();

private:
    bool ready = false;
    Sound digitSounds[10];
    Sound operatorSound;
    Sound equalsSound;
    Sound clearSound;
    Sound errorSound;

    Sound GenerateTone(float freqHz, float durationSec, float decay);
};
