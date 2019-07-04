#include "downloader.h"

Downloader::Downloader(const DownloadSett &sett, QObject *parent) : QObject(parent)
{
    msett = sett;
}

bool Downloader::runBashProc(const QString &command, QString &err)
{
    QProcess proc;
    proc.setProcessChannelMode(QProcess::MergedChannels);

    proc.start("bash");
    if(!proc.waitForStarted(3333)){
        err = tr("Couldn't launch bash, %1").arg(proc.errorString());
        return false;
    }
    //cd $LDIR && wget --continue --mirror --no-host-directories --ftp-user=$USER --ftp-password=$PASS ftp://$HOST/$RDIR

    proc.write(command.toUtf8());

    QTime time;
    time.start();
    for(int i = 0, imax = 0xFFFFFFF; i < imax && time.elapsed() < 7200000; i++){
        if(proc.waitForReadyRead(1111))
            emit putLog(proc.readAll());
        if(proc.state() != QProcess::Running)
            break;
    }
    proc.close();
    return true;
}

void Downloader::onThreadStarted()
{
    downloadAll();
    createLaunchers4executables();
    emit onFinished();
    QTimer::singleShot(111, this, SLOT(deleteLater()));
}

void Downloader::downloadAll()
{
    QString err;
    if(runBashProc(QString("cd %1 && wget --continue --mirror --no-host-directories --ftp-user=%2 --ftp-password=%3 ftp://%4/%5 && exit\n")
                   .arg(msett.ldir)
                   .arg(msett.user)
                   .arg(msett.pass)
                   .arg(msett.host)
                   .arg(msett.rdir), err)){

        emit appendMessage(tr("Downloaded"));

    }
}

void Downloader::createLaunchers4executables()
{
    emit appendMessage(tr("Creating launchers"));
    if(msett.launchershbody.isEmpty()){
        emit appendMessage(tr("Launcher script is invalid"));
        return;
    }
    QDir dir(QString("%1/%2").arg(msett.ldir).arg(msett.rdir));

    const QFileInfoList fil = dir.entryInfoList(QDir::Dirs|QDir::NoDotAndDotDot|QDir::Readable);
    const QString path2launchers = msett.ldir;

    QStringList makeexecutable;
    for(int i = 0, imax = fil.size(); i < imax; i++){

        QDir appdir(QString("%1/%2").arg(dir.absolutePath()).arg(fil.at(i).baseName()));
        qDebug() << fil.at(i).isDir() << fil.at(i).baseName() << appdir.absolutePath();

         const QStringList executables = appdir.entryList(QDir::Files|QDir::NoDotAndDotDot, QDir::Size);
         if(executables.isEmpty())
             continue;
         const QString fullpath2app = appdir.absoluteFilePath(executables.last());
         QFileInfo fi(fullpath2app);

         qDebug() << fi.absolutePath() << fi.isExecutable() << fi.isFile() << fi.size();
         if(!fi.isExecutable() && fi.isFile() && fi.size() > 1000){
             makeexecutable.append(fullpath2app);
         }


         const QString appname = executables.last();
         const QString filename = QString("%1/%2.sh").arg(path2launchers).arg(appname);

         fi.setFile(filename);
         if(fi.exists() && fi.isExecutable() && fi.size() > 50){
             emit appendMessage(tr("'%1' is already ready").arg(fi.baseName()));
             continue;
         }

         QSaveFile file(filename);
         if(file.open(QSaveFile::WriteOnly)){
             file.write(msett.launchershbody.toUtf8());
             if(file.commit()){
                 makeexecutable.append(filename);

                 emit appendMessage(tr("'%1' was created").arg(appname));

             }else{
                 emit appendMessage(tr("Couldn't create a launcher for '%1', %2").arg(appname).arg(file.errorString()));
             }
         }

    }

    for(int i = 0, imax = makeexecutable.size(); i < imax; i++){
        QString err;
        if(!runBashProc(QString("chmod +x %1 && exit\n").arg(makeexecutable.at(i)), err)){
            emit appendMessage(tr("Couldn't set the properties - 'executable'\n%1\n%2").arg(err).arg(makeexecutable.at(i)));
        }
    }


}
