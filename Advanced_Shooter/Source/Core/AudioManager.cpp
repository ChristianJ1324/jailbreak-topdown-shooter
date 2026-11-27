/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand
(c) [2026] Media Design School
File Name : AudioManager.cpp
Description : Sound effect and music playback
Author : Christian Jamieson
Mail : christian.jamieson@mds.ac.nz
**************************************************************************/
#include "Core/AudioManager.h"
#include <algorithm>
#include <filesystem>
#include <utility>

bool AudioManager::LoadSound(const std::string& strId, const std::string& strFilePath)
{
    if (!std::filesystem::exists(strFilePath))
    {
        return false;
    }

    sf::SoundBuffer soundBuffer;

    if (!soundBuffer.loadFromFile(strFilePath))
    {
        return false;
    }

    m_mapSoundBuffers[strId] = std::move(soundBuffer);
    return true;
}

bool AudioManager::LoadMusic(const std::string& strId, const std::string& strFilePath)
{
    if (!std::filesystem::exists(strFilePath))
    {
        return false;
    }

    std::unique_ptr<sf::Music> pMusic = std::make_unique<sf::Music>();

    if (!pMusic->openFromFile(strFilePath))
    {
        return false;
    }

    pMusic->setLooping(true);
    pMusic->setVolume(GetMusicVolume());
    m_mapMusic[strId] = std::move(pMusic);
    return true;
}

void AudioManager::PlaySound(const std::string& strId, float fVolumeScale)
{
    const auto itSoundBuffer = m_mapSoundBuffers.find(strId);

    if (itSoundBuffer == m_mapSoundBuffers.end())
    {
        return;
    }

    // keep sounds alive while they play
    Update();
    sf::Sound& sound = m_activeSounds.emplace_back(itSoundBuffer->second);
    sound.setVolume(std::clamp(m_fMasterVolume * fVolumeScale, 0.f, 100.f));
    sound.play();
}

void AudioManager::PlayMusic(const std::string& strId)
{
    if (m_strCurrentMusicId == strId)
    {
        // dont restart the same track
        const auto itCurrentMusic = m_mapMusic.find(strId);

        if (itCurrentMusic != m_mapMusic.end() &&
            itCurrentMusic->second->getStatus() != sf::SoundSource::Status::Playing)
        {
            itCurrentMusic->second->play();
        }

        return;
    }

    StopMusic();

    const auto itMusic = m_mapMusic.find(strId);

    if (itMusic == m_mapMusic.end())
    {
        m_strCurrentMusicId.clear();
        return;
    }

    itMusic->second->setLooping(true);
    itMusic->second->setVolume(GetMusicVolume());
    itMusic->second->play();
    m_strCurrentMusicId = strId;
}

void AudioManager::StopMusic()
{
    if (m_strCurrentMusicId.empty())
    {
        return;
    }

    const auto itMusic = m_mapMusic.find(m_strCurrentMusicId);

    if (itMusic != m_mapMusic.end())
    {
        itMusic->second->stop();
    }

    m_strCurrentMusicId.clear();
}

void AudioManager::Update()
{
    // clear finished sounds
    m_activeSounds.erase(
        std::remove_if(
            m_activeSounds.begin(),
            m_activeSounds.end(),
            [](const sf::Sound& sound)
            {
                return sound.getStatus() == sf::SoundSource::Status::Stopped;
            }),
        m_activeSounds.end());
}

void AudioManager::SetMasterVolume(float fMasterVolume)
{
    m_fMasterVolume = std::clamp(fMasterVolume, 0.f, 100.f);

    // apply new volume right away
    for (sf::Sound& sound : m_activeSounds)
    {
        sound.setVolume(m_fMasterVolume);
    }

    for (auto& pairMusic : m_mapMusic)
    {
        if (pairMusic.second)
        {
            pairMusic.second->setVolume(GetMusicVolume());
        }
    }
}

float AudioManager::GetMusicVolume() const
{
    return std::clamp(m_fMasterVolume * 0.45f, 0.f, 100.f);
}
