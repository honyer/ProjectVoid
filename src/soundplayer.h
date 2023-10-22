#pragma once

#include <QSoundEffect>

class SoundPlayer
{
public:
    static SoundPlayer &GetInstance()
    {
        static SoundPlayer instance;
        return instance;
    }

    QSoundEffect *StartBgmSound()
    {
        return &start_bgm_sound_;
    }

    QSoundEffect *GamingBgmSound()
    {
        return &gaming_bgm_sound_;
    }

    QSoundEffect *ButtonSound()
    {
        return &button_sound_;
    }

    QSoundEffect *GetHurtSound()
    {
        return &hurt_sound_;
    }

    QSoundEffect *GetLevelUpSound()
    {
        return &levelup_sound_;
    }

    QSoundEffect *GetFireSound()
    {
        return &fire_sound_;
    }

    QSoundEffect *GetBulletSound()
    {
        return &bullet_sound_;
    }

    QSoundEffect *GetExplodeSound()
    {
        return &explode_sound_;
    }

private:
    SoundPlayer()
    {
        start_bgm_sound_.setSource(QUrl::fromLocalFile(":/assets/bgm/infinite night sky - DDRKirby(ISQ).wav"));
        start_bgm_sound_.setLoopCount(QSoundEffect::Infinite);
        start_bgm_sound_.setVolume(0.5);

        gaming_bgm_sound_.setSource(QUrl::fromLocalFile(":/assets/bgm/Horizon Star - DDRKirby(ISQ).wav"));
        gaming_bgm_sound_.setLoopCount(QSoundEffect::Infinite);
        gaming_bgm_sound_.setVolume(0.5);

        button_sound_.setSource(QUrl::fromLocalFile(":/assets/sfx/ButtonClick.wav"));
        button_sound_.setVolume(0.8);

        hurt_sound_.setSource(QUrl::fromLocalFile(":/assets/sfx/Blood_Splash_Quick_01.wav"));
        hurt_sound_.setVolume(0.5);

        levelup_sound_.setSource(QUrl::fromLocalFile(":/assets/sfx/Buff_Intelligence.wav"));
        levelup_sound_.setVolume(0.5);

        fire_sound_.setSource(QUrl::fromLocalFile(":/assets/sfx/single_shot.wav"));
        fire_sound_.setVolume(0.5);

        bullet_sound_.setSource(QUrl::fromLocalFile(":/assets/sfx/sfx_sounds_impact1.wav"));
        bullet_sound_.setVolume(0.2);

        explode_sound_.setSource(QUrl::fromLocalFile(":/assets/sfx/explosion-sfx.wav"));
        explode_sound_.setVolume(0.5);
    }
    QSoundEffect start_bgm_sound_;  // 背景音乐
    QSoundEffect gaming_bgm_sound_; // 游戏音效

    QSoundEffect button_sound_;  // 点击按钮音效
    QSoundEffect hurt_sound_;    // 受伤音效
    QSoundEffect levelup_sound_; // 升级音效
    QSoundEffect fire_sound_;    // 射击音效
    QSoundEffect bullet_sound_;  // 子弹音效,击中敌人
    QSoundEffect explode_sound_; // 爆炸音效
};