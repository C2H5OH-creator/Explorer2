#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QApplication>
#include <QWidget>
#include <QHBoxLayout>
#include <QListView>
#include <QTreeView>
#include <QFileSystemModel>
#include <QDialog>
#include <QFileDialog>
#include <QDir>
#include <QDesktopServices>
#include <QMimeDatabase>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    model = new QFileSystemModel(this);
    model->setFilter(QDir::Files);
    model->setRootPath("");
    ui->listView_left->setModel(model);
    ui->listView_right->setModel(model);
    //connect(ui->listView_right, SIGNAL(doubleVliced(QModelIndex)),
            //this, SLOT(on_listView_left_doubleClicked(QModelIndex)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionOpen_folder_triggered() {

    // Open a dialog to select a directory
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::Directory);
    QString directoryPath = dialog.getExistingDirectory();

    // Check if a directory was selected
    if (!directoryPath.isEmpty()) {
        //Устанавливаем в модель нужный нам путь
        model->setRootPath(directoryPath);
        // Set the root index to display the directory contents
        QModelIndex rootIndex = model->index(directoryPath);
        ui->listView_left->setRootIndex(rootIndex);
    }
}


void MainWindow::on_listView_left_doubleClicked(const QModelIndex &index)
{
    //QListView* listView = (QListView*)sender();
    QFileInfo fileInfo = model->fileInfo(index);

    // Check if the file exists
    if (QFileInfo(fileInfo.path()).exists()) {
        // Open the file using the default application
        QDesktopServices::openUrl(QUrl::fromLocalFile(fileInfo.absoluteFilePath()));
    } else {
        // Handle the case where the file doesn't exist
        qDebug() << "File not found: " << fileInfo.path();
    }
}

void MainWindow::on_listView_right_doubleClicked(const QModelIndex &index)
{
    QFileInfo fileInfo = model->fileInfo(index);

    // Check if the file exists
    if (QFileInfo(fileInfo.path()).exists()) {
        // Open the file using the default application
        QDesktopServices::openUrl(QUrl::fromLocalFile(fileInfo.absoluteFilePath()));
    } else {
        // Handle the case where the file doesn't exist
        qDebug() << "File not found: " << fileInfo.path();
    }
}

void MainWindow::on_input_clicked()
{
    // Open a dialog to select a directory
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::Directory);
    QString directoryPath = dialog.getExistingDirectory();

    // Check if a directory was selected
    if (!directoryPath.isEmpty()) {
        //Устанавливаем в модель нужный нам путь
        model->setRootPath(directoryPath);
        // Set the root index to display the directory contents
        QModelIndex rootIndex = model->index(directoryPath);
        ui->listView_left->setRootIndex(rootIndex);
    }
}


void MainWindow::on_output_clicked()
{
    // Open a dialog to select a directory
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::Directory);
    QString directoryPath = dialog.getExistingDirectory();

    // Check if a directory was selected
    if (!directoryPath.isEmpty()) {
        //Устанавливаем в модель нужный нам путь
        model->setRootPath(directoryPath);
        // Set the root index to display the directory contents
        QModelIndex rootIndex = model->index(directoryPath);
        ui->listView_right->setRootIndex(rootIndex);
    }
}
