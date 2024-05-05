#include "sd_settings.h"
#include "ui_sd_settings.h"

SD_settings::SD_settings(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SD_settings)
{
    ui->setupUi(this);
    ui->from_path->setReadOnly(true);
    ui->to_path->setReadOnly(true);
}

SD_settings::~SD_settings()
{
    delete ui;
}

void SD_settings::on_buttonBox_accepted()
{
    int *settings = new int[5];
    settings[0] = 0;
    settings[1] = 0;
    settings[2] = 0;
    settings[3] = 0;
    settings[4] = 0;

    /*
        settings[0] - Copy_type
        settings[1] - Move_type
        settings[2] - folder_settings
        settings[3] - Raw_to_JPEG
        settings[4] - MTS_to_MP4

    */
    if(ui->Copy_type->isChecked()){
        settings[0] = 1;
    }
    if(ui->Move_type->isChecked()){
        settings[1] = 1;
    }
    if (ui->folder_settings->isChecked()){
        settings[2] = 1;
    }
    if (ui->Raw_to_JPEG->isChecked()){
        settings[3] = 1;
    }
    if(ui->MTS_to_MP4->isChecked()){
        settings[4] = 1;
    }
    emit sendSDActionsData(settings);
}

void SD_settings::receiveToPath(QString path){
    ui->to_path->setText(path);
}

void SD_settings::receiveFromPath(QString path){
    ui->from_path->setText(path);
}
