#ifndef AUDIO_H_INCLUDED
#define AUDIO_H_INCLUDED

//audio stuff

#include <iostream>
#include <unordered_map>

#include <raylib.h>

class SoundLibrary
{
    static std::unordered_map<std::string,Sound> sounds;
    static Music bgm; //background music
    static bool paused;
public:
    static void addSound(std::string_view path);
    static Sound* getSound(std::string_view name);
    static void loadSounds(std::string_view folderPath);

    static void loadBGM(std::string_view path);
    static void toggleBGM(bool val);
    static void toggleBGM();

    static void update();

};

#endif // AUDIO_H_INCLUDED
