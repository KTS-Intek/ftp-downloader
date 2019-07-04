#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <QObject>
#include <QtCore>

struct DownloadSett
{
    QString host;
    QString user;
    QString pass;
    QString ldir;//local
    QString rdir;//remote
    QString launchershbody;
    DownloadSett() {}
};

class Downloader : public QObject
{
    Q_OBJECT
public:
    explicit Downloader(const DownloadSett &sett, QObject *parent = nullptr);

    DownloadSett msett;

    bool runBashProc(const QString &command, QString &err);
signals:
    void onFinished();
    void appendMessage(QString message);
    void putLog(QByteArray message);

public slots:
    void onThreadStarted();

    void downloadAll();

    void createLaunchers4executables();


};

#endif // DOWNLOADER_H
