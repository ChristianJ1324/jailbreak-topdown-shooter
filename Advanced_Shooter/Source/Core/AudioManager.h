/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand
(c) [2026] Media Design School
File Name : AudioManager.h
Description : Sound effect and music playback
Author : Christian Jamieson
Mail : christian.jamieson@mds.ac.nz
**************************************************************************/
#pragma once

#include <SFML/Audio.hpp>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class AudioManager
{
public:
    bool LoadSound(const std::string& strId, const std::string& strFilePath);
    bool LoadMusic(const std::string& strId, const std::string& strFilePath);

    void PlaySound(const std::string& strId, float fVolumeScale = 1.f);
    void PlayMusic(const std::string& strId);
    void StopMusic();
    void Update();

    void SetMasterVolume(float fMasterVolume);

private:
    float GetMusicVolume() const;

    // sounds and music cached by id
    std::unordered_map<std::string, sf::SoundBuffer> m_mapSoundBuffers;
    std::unordered_map<std::string, std::unique_ptr<sf::Music>> m_mapMusic;
    std::vector<sf::Sound> m_activeSounds;
    std::string m_strCurrentMusicId;
    float m_fMasterVolume = 100.f;
};
