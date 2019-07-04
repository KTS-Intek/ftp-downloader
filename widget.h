#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();

     QString getSettingsFileName() const;//return QString("%1.conf").arg(qAppName());

public slots:
     void loadSettings();

     void saveSettings();


     void onOperationFinished();

     void append2log(QString message);

     void putLog(QByteArray message);

private slots:
    void on_pushButton_clicked();

    void on_pbSaveSett_clicked();

    void on_pbReloadSett_clicked();

private:
    Ui::Widget *ui;
};

#endif // WIDGET_H
