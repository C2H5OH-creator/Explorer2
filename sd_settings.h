#ifndef SD_SETTINGS_H
#define SD_SETTINGS_H

#include <QDialog>

namespace Ui {
class SD_settings;
}

class SD_settings : public QDialog
{
    Q_OBJECT

signals:
    void sendSDActionsData(int *settings);

public:
    explicit SD_settings(QWidget *parent = nullptr);
    ~SD_settings();

private slots:
    void on_buttonBox_accepted();

public slots:
    void receiveToPath(QString path);
    void receiveFromPath(QString path);

private:
    Ui::SD_settings *ui;
};

#endif // SD_SETTINGS_H
