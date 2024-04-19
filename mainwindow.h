#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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

    void on_listView_left_clicked(const QModelIndex &index);

    void on_move_to_right_clicked();

    void on_move_to_left_clicked();

    void moveDirectoryRecursively(const QString &sourceDir, const QString &destDir);

    void copyDirectoryRecursively(const QString &sourceDir, const QString &destDir);

    //void on_left_path_textChanged(const QString &arg1);
    void setPathView(int type, QString path);

    void on_copy_to_right_clicked();

    void on_copy_to_left_clicked();

    void on_delete_left_clicked();

    void on_delete_right_clicked();

    void permanentlyDeleteFiles(const QStringList& filePaths);
    void on_permanent_del_left_clicked();

    void on_permanent_del_right_clicked();

private:
    Ui::MainWindow *ui;
    QFileSystemModel* model;
    QString visited;

};

//Ищет отсутствующие или неактуальные элементы в резервной директории
//void contentDifference(QDir &sDir, QDir &dDir, QFileInfoList &diffList);
//Наполняет список всех вложеных директорий и файлов
//void recursiveContentList(QDir &dir, QFileInfoList &contentList);

#endif // MAINWINDOW_H
