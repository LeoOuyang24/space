#include <filesystem>

#include "../headers/audio.h"

std::unordered_map<std::string,Sound> SoundLibrary::sounds;
Music SoundLibrary::bgm; //background music
bool SoundLibrary::paused = paused;


void SoundLibrary::addSound(std::string_view path)
{
    std::string read = "sounds/";
    read += path.data();
    sounds[path.data()] = LoadSound(read.c_str());
    if (!IsSoundValid(sounds[path.data()]))
    {
        std::cout << "ERROR SoundLibrary::addSound: Failed to load sound: " << path << "\n";
    }
}

Sound* SoundLibrary::getSound(std::string_view path)
{
    if (sounds.find(path.data()) != sounds.end())
    {
        return &sounds[path.data()];
    }
    return nullptr;
}

void SoundLibrary::loadSounds(std::string_view folderPath)
{
   for (const auto & entry : std::filesystem::directory_iterator(folderPath))
   {
       if (!std::filesystem::is_directory(entry.path()))
       {
            std::string path = entry.path().filename().string();
            addSound(path);
       }
   }
}

void SoundLibrary::loadBGM(std::string_view path)
{
    bgm = LoadMusicStream(path.data());
    if (IsMusicValid(bgm))
    {
        SetMusicVolume(bgm,1);
        PlayMusicStream(bgm);
    }
    else
    {
        std::cerr << "ERROR SoundLibrary::loadSong: error loading song: " << path << "\n";
    }
}

void SoundLibrary::toggleBGM(bool val)
{
    if (!val)
    {
        PauseMusicStream(bgm);
    }
    else
    {
        ResumeMusicStream(bgm);
    }
    paused = !val;
}

void SoundLibrary::toggleBGM()
{
    toggleBGM(paused);
}

void SoundLibrary::update()
{
    UpdateMusicStream(bgm);
}
