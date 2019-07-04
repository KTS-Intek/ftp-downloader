#include "widget.h"
#include "ui_widget.h"
#include "downloader.h"
#include <QScrollBar>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    loadSettings();

    if(ui->cbxAutoStart->isChecked())
        ui->pushButton->animateClick();
}

Widget::~Widget()
{
    delete ui;
}

QString Widget::getSettingsFileName() const
{
    return QString("%1.conf").arg(qAppName());
}

void Widget::loadSettings()
{
    QSettings settings(getSettingsFileName(), QSettings::IniFormat);
    settings.beginGroup("downloader");

    ui->cbxAutoStart->setChecked(settings.value("cbxAutoStart", false).toBool());
    ui->cbxCreateLaunchers->setChecked(settings.value("cbxCreateLaunchers", true).toBool());
    ui->leHost->setText(settings.value("leHost").toString());
    ui->leLdir->setText(settings.value("leLdir", "apps").toString());
    ui->leRdir->setText(settings.value("leRdir").toString());
    ui->leUser->setText(settings.value("leUser").toString());
    ui->lePass->setText(settings.value("lePass").toString());
    ui->pteLauncherText->setPlainText(settings.value("pteLauncherText", ui->pteLauncherText->toPlainText()).toString());


}

void Widget::saveSettings()
{
    QSettings settings(getSettingsFileName(), QSettings::IniFormat);
    settings.beginGroup("downloader");
    settings.setValue("cbxAutoStart", ui->cbxAutoStart->isChecked());
    settings.setValue("cbxCreateLaunchers", ui->cbxCreateLaunchers->isChecked());
    settings.setValue("leHost", ui->leHost->text());
    settings.setValue("leLdir", ui->leLdir->text());
    settings.setValue("lePass", ui->lePass->text());
    settings.setValue("leRdir", ui->leRdir->text());
    settings.setValue("leUser", ui->leUser->text());
    settings.setValue("pteLauncherText", ui->pteLauncherText->toPlainText());
}

void Widget::onOperationFinished()
{
    ui->pushButton->setEnabled(true);

}

void Widget::append2log(QString message)
{
    qDebug() << message;
    ui->pteLog->appendPlainText(message);
}

void Widget::putLog(QByteArray message)
{
    ui->pteLog->insertPlainText(message);

    QScrollBar *bar = ui->pteLog->verticalScrollBar();
    bar->setValue(bar->maximum());
}

void Widget::on_pushButton_clicked()
{
    ui->pushButton->setEnabled(false);

    DownloadSett sett;
    sett.host = ui->leHost->text().simplified().trimmed();
    sett.launchershbody = ui->cbxCreateLaunchers->isChecked() ? ui->pteLauncherText->toPlainText() : QString();
    sett.ldir = ui->leLdir->text().simplified().trimmed();
    sett.rdir = ui->leRdir->text().simplified().trimmed();
    sett.pass = ui->lePass->text().simplified().trimmed();
    sett.user = ui->leUser->text().simplified().trimmed();

    Downloader *d = new Downloader(sett);
    QThread *t = new QThread(this);
    d->moveToThread(t);

    connect(d, SIGNAL(destroyed(QObject*)), t, SLOT(quit()));
    connect(t, SIGNAL(finished()), t, SLOT(deleteLater()));
    connect(t, SIGNAL(started()), d, SLOT(onThreadStarted()));

    connect(d, &Downloader::appendMessage, this, &Widget::append2log);
    connect(d, &Downloader::onFinished, this, &Widget::onOperationFinished);
    connect(d, &Downloader::putLog, this, &Widget::putLog);
    t->start();

}

void Widget::on_pbSaveSett_clicked()
{
    saveSettings();
}

void Widget::on_pbReloadSett_clicked()
{
    loadSettings();
}
