#pragma once

#include <QObject>
#include <QString>
#include <QQueue>

class Log : public QObject
{
    Q_OBJECT
public:
    static Log &GetInstance()
    {
        static Log instance;
        return instance;
    }

    void AddLog(QString str)
    {
        log_queue_.enqueue(str);
        if (log_queue_.size() > length_)
            log_queue_.dequeue();
        emit LogAdded(str);
    }

    QStringList GetLogs()
    {
        QStringList list;
        QQueue<QString> copy = log_queue_;
        while (!copy.empty())
        {
            list.append(copy.front());
            copy.dequeue();
        }
        return list;
    }
    void Clear()
    {
        log_queue_.clear();
    }
signals:
    void LogAdded(QString str);

private:
    int length_;
    QQueue<QString> log_queue_;
    Log()
    {
        length_ = 10;
    }
};
