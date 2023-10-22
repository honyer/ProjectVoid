#pragma once

#include <QObject>
#include <QTimer>

class GlobalTimer : public QObject
{
    Q_OBJECT
public:
    static GlobalTimer &GetInstance()
    {
        static GlobalTimer instance;
        return instance;
    }

    QTimer *GetTimer()
    {
        return &timer_;
    }

    void SwitchTimer()
    {
        if (timer_.isActive())
        {
            emit MainTimerStarted();
        }
        else
        {
            emit MainTimerStopped();
        }
    }
signals:
    void MainTimerStarted();
    void MainTimerStopped();

private:
    GlobalTimer()
    {
        timer_.setInterval(10);
        timer_.start();
    }

    QTimer timer_;

    // GlobalTimer(const GlobalTimer &) = delete;
    // GlobalTimer &operator=(const GlobalTimer &) = delete;
};