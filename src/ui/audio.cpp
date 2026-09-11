#include "audio.h"
#include <cmath>
#include <cstdlib>
#include <vector>

Sound AudioManager::GenerateTone(float freqHz, float durationSec, float decay) {
    const int sampleRate = 44100;
    int frameCount = static_cast<int>(sampleRate * durationSec);

    std::vector<short> samples(frameCount);
    for (int i = 0; i < frameCount; i++) {
        float t = static_cast<float>(i) / sampleRate;
        float envelope = std::exp(-decay * t);
        float sample = std::sin(2.0f * PI * freqHz * t) * envelope;
        samples[i] = static_cast<short>(sample * 30000.0f);
    }

    Wave wave{};
    wave.frameCount = static_cast<unsigned int>(frameCount);
    wave.sampleRate = sampleRate;
    wave.sampleSize = 16;
    wave.channels = 1;
    wave.data = samples.data();

    // aman walau `samples` keluar scope, soalnya datanya udah dicopy
    Sound sound = LoadSoundFromWave(wave);
    return sound;
}

void AudioManager::Init() {
    InitAudioDevice();

    for (int d = 0; d < 10; d++) {
        float freq = 420.0f + d * 35.0f;
        digitSounds[d] = GenerateTone(freq, 0.09f, 28.0f);
    }

    operatorSound = GenerateTone(300.0f, 0.07f, 24.0f);
    equalsSound   = GenerateTone(660.0f, 0.16f, 12.0f);
    clearSound    = GenerateTone(220.0f, 0.10f, 20.0f);
    errorSound    = GenerateTone(140.0f, 0.22f, 8.0f);

    ready = true;
}

void AudioManager::Shutdown() {
    if (!ready) return;
    for (auto& s : digitSounds) UnloadSound(s);
    UnloadSound(operatorSound);
    UnloadSound(equalsSound);
    UnloadSound(clearSound);
    UnloadSound(errorSound);
    CloseAudioDevice();
    ready = false;
}

void AudioManager::PlayDigit(int digitIndex) {
    if (!ready || digitIndex < 0 || digitIndex > 9) return;
    PlaySound(digitSounds[digitIndex]);
}

void AudioManager::PlayOperator() {
    if (ready) PlaySound(operatorSound);
}

void AudioManager::PlayEquals() {
    if (ready) PlaySound(equalsSound);
}

void AudioManager::PlayClear() {
    if (ready) PlaySound(clearSound);
}

void AudioManager::PlayError() {
    if (ready) PlaySound(errorSound);
}
