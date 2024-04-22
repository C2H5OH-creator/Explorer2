#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "sd_settings.h"

#include <QApplication>
#include <QWidget>
#include <QHBoxLayout>
#include <QListView>
#include <QTreeView>
#include <QFileSystemModel>
#include <QMainWindow>
#include <QKeyEvent>




QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionOpen_folder_triggered();

    void on_listView_left_doubleClicked(const QModelIndex &index);

    void on_input_clicked();

    void on_output_clicked();

    void on_listView_right_doubleClicked(const QModelIndex &index);

    void on_back_right_clicked();

    void on_back_left_clicked();

    //void on_listView_left_clicked(const QModelIndex &index);

    void on_move_to_right_clicked();

    void on_move_to_left_clicked();

    void moveDirectoryRecursively(const QString &sourceDir, const QString &destDir);

    void copyDirectoryRecursively(const QString &sourceDir, const QString &destDir);

    void setPathView(int type, QString path);

    void on_copy_to_right_clicked();

    void on_copy_to_left_clicked();

    void on_delete_left_clicked();

    void on_delete_right_clicked();

    void permanentlyDeleteFiles(const QStringList& filePaths);

    void on_permanent_del_left_clicked();

    void on_permanent_del_right_clicked();

    void on_sd_actions_left_clicked();

    void on_sd_actions_right_clicked();

    void on_switch_paths_clicked();

    void slotShortcutCtrl_Up();

    void slotShortcutCtrl_Down();

    void slotShortcutCtrl_Eq();

    void viewStop(int &view);

    void wheelEvent(QWheelEvent *event);

public slots:
    void receiveLeftData(int *settins);
    void receiveRightData(int *settins);

private:
    Ui::MainWindow *ui;
    QFileSystemModel* model;
    QString visited;
    QShortcut *keyCtrl_Up;    // Ctrl + +
    QShortcut *keyCtrl_Down;  // Ctrl + -
    QShortcut *keyCtrl_Eq;    // Ctrl + =

};
#endif // MAINWINDOW_H
