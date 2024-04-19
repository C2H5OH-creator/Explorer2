#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QApplication>
#include <QWidget>
#include <QHBoxLayout>
#include <QListView>
#include <QFileSystemModel>
#include <QDialog>
#include <QFileDialog>
#include <QDir>
#include <QDesktopServices>
#include <QMimeDatabase>
#include <QFileInfo>
#include <QMessageBox>
#include <QAbstractItemView>
#include <QModelIndexList>
#include <QFile>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    model = new QFileSystemModel(this);
    model->setRootPath("");

    //Left
    ui->listView_left->setModel(model);
    ui->listView_left->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->left_path->setPlaceholderText("");
    ui->left_path->setReadOnly(true);

    //Right
    ui->listView_right->setModel(model);
    ui->listView_right->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->right_path->setPlaceholderText("");
    ui->right_path->setReadOnly(true);

}

MainWindow::~MainWindow()
{
    delete ui;
}

//Установка пути в сточку
void MainWindow::setPathView(int type, QString path){

    switch(type){
    case(0):
        ui->left_path->setText(path);
        break;
    case(1):
        ui->right_path->setText(path);
        break;
    }
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

//Оккрытие папки/файла слева
void MainWindow::on_listView_left_doubleClicked(const QModelIndex &index)
{
    QFileInfo fileInfo = model->fileInfo(index);

    if (fileInfo.isDir()){
        ui->listView_left->setRootIndex(index);
        setPathView(0, model->filePath(index));
    }
    else if (QFileInfo(fileInfo.path()).exists()) {
        // Open the file using the default application
        QDesktopServices::openUrl(QUrl::fromLocalFile(fileInfo.absoluteFilePath()));
    }
    else {
        // Handle the case where the file doesn't exist
        qDebug() << "File not found: " << fileInfo.path();
    }
}

//Назад слева
void MainWindow::on_back_left_clicked()
{
    // Получаем нынешний root index
    QModelIndex currentRootIndex = ui->listView_left->rootIndex();
    QFileInfo fileInfo = model->fileInfo(currentRootIndex);
    QString parentPath = currentRootIndex.parent().data(QFileSystemModel::FilePathRole).toString();
    //Обновление пути
    setPathView(0, parentPath);

    // Check if the parent path is the root directory
    if (parentPath == "") {
        ui->listView_left->setRootIndex(model->index(""));
        //Обновление пути

    }
    else{
        QDir dir = fileInfo.dir();
        dir.cd(".");
        ui->listView_left->setRootIndex(model->index(dir.absolutePath()));
        ui->left_path->setText(dir.absolutePath());
    }
}

//Оккрытие папки/файла справа
void MainWindow::on_listView_right_doubleClicked(const QModelIndex &index)
{
    QFileInfo fileInfo = model->fileInfo(index);

    if (fileInfo.isDir()){
        ui->listView_right->setRootIndex(index);
        //Обновление пути
        setPathView(1, model->filePath(index));
    }
    else if (QFileInfo(fileInfo.path()).exists()) {
        // Open the file using the default application
        QDesktopServices::openUrl(QUrl::fromLocalFile(fileInfo.absoluteFilePath()));
    }
    else {
        // Handle the case where the file doesn't exist
        qDebug() << "File not found: " << fileInfo.path();
    }
}

//Назад справа
void MainWindow::on_back_right_clicked()
{
    // Получаем нынешний root index
    QModelIndex currentRootIndex = ui->listView_right->rootIndex();
    QFileInfo fileInfo = model->fileInfo(currentRootIndex);
    QString parentPath = currentRootIndex.parent().data(QFileSystemModel::FilePathRole).toString();
    //Обновление пути
    setPathView(1, parentPath);

    // Проверка на "" директорию
    if (parentPath == "") {
        ui->listView_right->setRootIndex(model->index(""));

    }
    else{
        QDir dir = fileInfo.dir();
        dir.cd(".");
        ui->listView_right->setRootIndex(model->index(dir.absolutePath()));
    }
}

//Открытие папки влева через проводник
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

//Открытие папки справа через проводник
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

//Копирование файлов слева-направо
void MainWindow::on_copy_to_left_clicked()
{
    int copyIndex = 1;

    // Получаем модель выбора из listView'ов
    QItemSelectionModel* selectionModelLeft = ui->listView_right->selectionModel();

    // Получаем список выбранных индексов
    QModelIndexList rightSelected = selectionModelLeft->selectedIndexes();

    // Проверяем, есть ли выделенные элементы
    if (rightSelected.isEmpty()) {
        QMessageBox::information(this, "Copy", "No items selected for copying.");
        return;
    }

    // Переменная для отслеживания выбора пользователя "Да, заменить все"
    bool replaceAll = false;

    // Итерация по выбранным индексам
    foreach (const QModelIndex &index, rightSelected) {
        QFileInfo fileInfo = model->fileInfo(index);

        // Проверяем тип файла (файл или папка)
        if (fileInfo.isFile()) {
            QString sourcePath = fileInfo.absoluteFilePath();
            QString destDirPath = model->filePath(ui->listView_left->rootIndex());
            QString destFileName = fileInfo.fileName();

            // Создаем переменную для индекса
            int index = 1;
            QString newDestPath = destDirPath + '/' + destFileName;

            // Проверяем, существует ли файл с таким именем в целевой папке
            while (QFile::exists(newDestPath)) {
                // Если файл существует, добавляем индекс к имени файла
                QString baseName = fileInfo.baseName();
                QString suffix = fileInfo.suffix();

                // Создаем новое имя файла с индексом
                destFileName = baseName + "(" + QString::number(index) + ")";

                // Если файл имеет расширение, добавляем его обратно
                if (!suffix.isEmpty()) {
                    destFileName += "." + suffix;
                }

                // Обновляем конечный путь назначения с новым именем файла
                newDestPath = destDirPath + '/' + destFileName;

                // Увеличиваем индекс для следующей итерации
                index++;
            }

            // Копирование файла и обработка возможных ошибок
            if (!QFile::copy(sourcePath, newDestPath)) {
                QMessageBox::critical(this, "Copy Error", "Failed to copy file: " + sourcePath + " to " + newDestPath);
            }

        } else if (fileInfo.isDir()) {
            // Реализуем перемещение директорий рекурсивно
            QString destPath = model->filePath(ui->listView_left->rootIndex()) + '/' + fileInfo.fileName();
            copyDirectoryRecursively(fileInfo.absoluteFilePath(), destPath);
        }
    }

}

//Копирование файлов справа-налево
void MainWindow::on_copy_to_right_clicked()
{
    int copyIndex = 1;

    // Получаем модель выбора из listView'ов
    QItemSelectionModel* selectionModelLeft = ui->listView_left->selectionModel();

    // Получаем список выбранных индексов
    QModelIndexList leftSelected = selectionModelLeft->selectedIndexes();

    // Проверяем, есть ли выделенные элементы
    if (leftSelected.isEmpty()) {
        QMessageBox::information(this, "Copy", "No items selected for copying.");
        return;
    }

    // Переменная для отслеживания выбора пользователя "Да, заменить все"
    bool replaceAll = false;

    // Итерация по выбранным индексам
    foreach (const QModelIndex &index, leftSelected) {
        QFileInfo fileInfo = model->fileInfo(index);

        // Проверяем тип файла (файл или папка)
        if (fileInfo.isFile()) {
            QString sourcePath = fileInfo.absoluteFilePath();
            QString destDirPath = model->filePath(ui->listView_right->rootIndex());
            QString destFileName = fileInfo.fileName();

            // Создаем переменную для индекса
            int index = 1;
            QString newDestPath = destDirPath + '/' + destFileName;

            // Проверяем, существует ли файл с таким именем в целевой папке
            while (QFile::exists(newDestPath)) {
                // Если файл существует, добавляем индекс к имени файла
                QString baseName = fileInfo.baseName();
                QString suffix = fileInfo.suffix();

                // Создаем новое имя файла с индексом
                destFileName = baseName + "(" + QString::number(index) + ")";

                // Если файл имеет расширение, добавляем его обратно
                if (!suffix.isEmpty()) {
                    destFileName += "." + suffix;
                }

                // Обновляем конечный путь назначения с новым именем файла
                newDestPath = destDirPath + '/' + destFileName;

                // Увеличиваем индекс для следующей итерации
                index++;
            }

            // Копирование файла и обработка возможных ошибок
            if (!QFile::copy(sourcePath, newDestPath)) {
                QMessageBox::critical(this, "Copy Error", "Failed to copy file: " + sourcePath + " to " + newDestPath);
            }

        } else if (fileInfo.isDir()) {
            // Реализуем перемещение директорий рекурсивно
            QString destPath = model->filePath(ui->listView_right->rootIndex()) + '/' + fileInfo.fileName();
            copyDirectoryRecursively(fileInfo.absoluteFilePath(), destPath);
        }
    }
}

void MainWindow::on_listView_left_clicked(const QModelIndex &index)
{
    // Check if Ctrl/Cmd is pressed for multiple selection
    if (!(QApplication::keyboardModifiers() & Qt::ControlModifier)) {
        // Deselect all if Ctrl/Cmd is not pressed (single selection mode)
        ui->listView_left->selectionModel()->clearSelection();
    }

    // Check if Ctrl/Cmd is pressed for multiple selection
    if (QApplication::keyboardModifiers() & Qt::ControlModifier) {
        if (ui->listView_left->selectionModel()->isSelected(index)) {
            // Deselect the item if currently selected
            ui->listView_left->selectionModel()->select(index, QItemSelectionModel::Deselect);
        } else {
            // Select the item if not already selected (original behavior)
            ui->listView_left->selectionModel()->select(index, QItemSelectionModel::Select);
        }
    }
}

//Перемещение файлов слева-направо
void MainWindow::on_move_to_right_clicked() {

    // Получаем модель выбора из listView'ов
    QItemSelectionModel* selectionModelLeft = ui->listView_left->selectionModel();

    // Получаем список выбранных индексов
    QModelIndexList leftSelected = selectionModelLeft->selectedIndexes();

    // Проверяем, есть ли выделенные элементы
    if (leftSelected.isEmpty()) {
        QMessageBox::information(this, "Move", "No items selected for moving.");
        return;
    }

    // Переменная для отслеживания выбора пользователя "Да, заменить все"
    bool replaceAll = false;

    // Итерация по выбранным индексам
    foreach (const QModelIndex &index, leftSelected) {
        QFileInfo fileInfo = model->fileInfo(index);

        // Проверяем тип файла (файл или папка)
        if (fileInfo.isFile()) {
            QString sourcePath = fileInfo.absoluteFilePath();
            QString destPath = model->filePath(ui->listView_right->rootIndex()) + '/' + fileInfo.fileName();

            // Проверяем, существует ли файл в месте назначения
            if (QFile::exists(destPath)) {
                // Если файл существует, и пользователь еще не выбрал "Да, заменить все"
                if (!replaceAll) {
                    // Предложить выбор: пропустить, заменить, заменить все или отменить
                    QMessageBox::StandardButton choice = QMessageBox::question(
                        this,
                        "File exists",
                        "The file \"" + fileInfo.fileName() + "\" already exists in the destination. What would you like to do?",
                        QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel | QMessageBox::YesToAll,
                        QMessageBox::Yes // Устанавливаем начальную кнопку выбора (Yes - заменить)
                        );

                    if (choice == QMessageBox::Yes) {
                        // Если пользователь выбирает заменить, удаляем существующий файл
                        QFile::remove(destPath);
                    } else if (choice == QMessageBox::No) {
                        // Если пользователь выбирает пропустить, переходим к следующему файлу
                        continue;
                    } else if (choice == QMessageBox::Cancel) {
                        // Если пользователь выбирает отменить, прерываем перемещение
                        return;
                    } else if (choice == QMessageBox::YesToAll) {
                        // Если пользователь выбирает "Да, заменить все", устанавливаем флаг
                        replaceAll = true;
                        QFile::remove(destPath);
                    }
                } else {
                    // Если пользователь уже выбрал "Да, заменить все", просто удаляем существующий файл
                    QFile::remove(destPath);
                }
            }
            // Перемещение файла и обработка возможных ошибок
            if (!QFile::rename(sourcePath, destPath)) {
                QMessageBox::critical(this, "Move Error", "Failed to move file: " + sourcePath + " to " + destPath);
                // Проверка ошибки в QFile
                QFile file(sourcePath);
                if (file.error() != QFile::NoError) {
                    QMessageBox::critical(this, "Move Error", "Error: " + file.errorString());
                }
            }
        } else if (fileInfo.isDir()) {
            // Реализуем перемещение директорий рекурсивно
            QString destPath = model->filePath(ui->listView_right->rootIndex()) + '/' + fileInfo.fileName();
            moveDirectoryRecursively(fileInfo.absoluteFilePath(), destPath);
        }
    }
}

//Перемещение файлов справа-налево
void MainWindow::on_move_to_left_clicked()
{
    // Получаем модель выбора из listView'ов
    QItemSelectionModel* selectionModelRight = ui->listView_right->selectionModel();

    // Получаем список выбранных индексов
    QModelIndexList rightSelected = selectionModelRight->selectedIndexes();

    // Проверяем, есть ли выделенные элементы
    if (rightSelected.isEmpty()) {
        QMessageBox::information(this, "Move", "No items selected for moving.");
        return;
    }

    // Переменная для отслеживания выбора пользователя "Да, заменить все"
    bool replaceAll = false;

    // Итерация по выбранным индексам
    foreach (const QModelIndex &index, rightSelected) {
        QFileInfo fileInfo = model->fileInfo(index);

        // Проверяем тип файла (файл или папка)
        if (fileInfo.isFile()) {
            QString sourcePath = fileInfo.absoluteFilePath();
            QString destPath = model->filePath(ui->listView_left->rootIndex()) + '/' + fileInfo.fileName();

            // Проверяем, существует ли файл в месте назначения
            if (QFile::exists(destPath)) {
                // Если файл существует, и пользователь еще не выбрал "Да, заменить все"
                if (!replaceAll) {
                    // Предложить выбор: пропустить, заменить, заменить все или отменить
                    QMessageBox::StandardButton choice = QMessageBox::question(
                        this,
                        "File exists",
                        "The file \"" + fileInfo.fileName() + "\" already exists in the destination. What would you like to do?",
                        QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel | QMessageBox::YesToAll,
                        QMessageBox::Yes // Устанавливаем начальную кнопку выбора (Yes - заменить)
                        );

                    if (choice == QMessageBox::Yes) {
                        // Если пользователь выбирает заменить, удаляем существующий файл
                        QFile::remove(destPath);
                    } else if (choice == QMessageBox::No) {
                        // Если пользователь выбирает пропустить, переходим к следующему файлу
                        continue;
                    } else if (choice == QMessageBox::Cancel) {
                        // Если пользователь выбирает отменить, прерываем перемещение
                        return;
                    } else if (choice == QMessageBox::YesToAll) {
                        // Если пользователь выбирает "Да, заменить все", устанавливаем флаг
                        replaceAll = true;
                        QFile::remove(destPath);
                    }
                } else {
                    // Если пользователь уже выбрал "Да, заменить все", просто удаляем существующий файл
                    QFile::remove(destPath);
                }
            }
            // Перемещение файла и обработка возможных ошибок
            if (!QFile::rename(sourcePath, destPath)) {
                QMessageBox::critical(this, "Move Error", "Failed to move file: " + sourcePath + " to " + destPath);
                // Проверка ошибки в QFile
                QFile file(sourcePath);
                if (file.error() != QFile::NoError) {
                    QMessageBox::critical(this, "Move Error", "Error: " + file.errorString());
                }
            }
        } else if (fileInfo.isDir()) {
            // Реализуем перемещение директорий рекурсивно
            QString destPath = model->filePath(ui->listView_left->rootIndex()) + '/' + fileInfo.fileName();
            moveDirectoryRecursively(fileInfo.absoluteFilePath(), destPath);
        }
    }
}

// Функция для рекурсивного перемещения директорий
void MainWindow::moveDirectoryRecursively(const QString &sourceDir, const QString &destDir) {
    QDir source(sourceDir);
    QDir dest(destDir);

    // Проверяем, существует ли директория назначения, если нет - создаем ее
    if (!dest.exists()) {
        dest.mkpath(destDir);
    }

    // Перемещаем файлы и папки рекурсивно
    foreach (const QFileInfo &fileInfo, source.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries)) {
        QString srcFilePath = fileInfo.absoluteFilePath();
        QString dstFilePath = dest.filePath(fileInfo.fileName());

        if (fileInfo.isDir()) {
            moveDirectoryRecursively(srcFilePath, dstFilePath);
        } else {
            if (!QFile::rename(srcFilePath, dstFilePath)) {
                QMessageBox::critical(this, "Move Error", "Failed to move file: " + srcFilePath);
            }
        }
    }

    // Удаляем пустую исходную директорию
    source.rmdir(sourceDir);
}


// Функция для рекурсивного копирования директорий
void MainWindow::copyDirectoryRecursively(const QString &sourceDir, const QString &destDir) {
    QDir source(sourceDir);
    QDir dest(destDir);

    // Проверяем, существует ли директория назначения, если нет - создаем ее
    if (!dest.exists()) {
        dest.mkpath(destDir);
    }

    // Копируем файлы и папки рекурсивно
    foreach (const QFileInfo &fileInfo, source.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries)) {
        QString srcFilePath = fileInfo.absoluteFilePath();
        QString dstFilePath = dest.filePath(fileInfo.fileName());

        if (fileInfo.isDir()) {
            copyDirectoryRecursively(srcFilePath, dstFilePath);
        } else {
            if (!QFile::copy(srcFilePath, dstFilePath)) {
                QMessageBox::critical(this, "Copy Error", "Failed to copy file: " + srcFilePath);
            }
        }
    }
}

//Удаление, минуя корзину
void MainWindow::permanentlyDeleteFiles(const QStringList& filePaths) {
    // Итерация по списку путей к файлам и папкам
    foreach (const QString &filePath, filePaths) {
        QFileInfo fileInfo(filePath);

        if (fileInfo.isFile()) {
            // Если это файл, удаляем его
            QFile file(filePath);
            if (!file.remove()) {
                // Если возникла ошибка при удалении файла, выводим сообщение об ошибке
                QMessageBox::critical(this, "Delete Error", "Failed to permanently delete file: " + filePath);
            }
        } else if (fileInfo.isDir()) {
            // Если это папка, удаляем ее рекурсивно
            QDir dir(filePath);
            if (!dir.removeRecursively()) {
                // Если возникла ошибка при удалении папки, выводим сообщение об ошибке
                QMessageBox::critical(this, "Delete Error", "Failed to permanently delete directory: " + filePath);
            }
        }
    }
}

//Удаления слева
void MainWindow::on_delete_left_clicked()
{
    // Получаем модель выбора из listView'ов
    QItemSelectionModel* selectionModel = ui->listView_left->selectionModel();

    // Получаем список выбранных индексов
    QModelIndexList selectedIndexes = selectionModel->selectedIndexes();

    // Проверяем, есть ли выделенные элементы
    if (selectedIndexes.isEmpty()) {
        QMessageBox::information(this, "Delete", "No items selected for deletion.");
        return;
    }

    // Переменная для подтверждения удаления
    QMessageBox::StandardButton confirmDelete = QMessageBox::question(
        this,
        "Confirm Deletion",
        "Are you sure you want to delete the selected items?",
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No
        );

    if (confirmDelete == QMessageBox::Yes) {
        // Итерация по выбранным индексам
        foreach (const QModelIndex &index, selectedIndexes) {
            QFileInfo fileInfo = model->fileInfo(index);

            // Получаем путь к файлу или папке
            QString filePath = fileInfo.absoluteFilePath();

            // Перемещение файла или папки в корзину
            bool success = QFile::moveToTrash(filePath);

            // Проверка успеха перемещения
            if (!success) {
                QMessageBox::critical(this, "Delete Error", "Failed to delete file: " + filePath);
            }
        }
    }
}

//Удаление справа
void MainWindow::on_delete_right_clicked()
{
    // Получаем модель выбора из listView'ов
    QItemSelectionModel* selectionModel = ui->listView_right->selectionModel();

    // Получаем список выбранных индексов
    QModelIndexList selectedIndexes = selectionModel->selectedIndexes();

    // Проверяем, есть ли выделенные элементы
    if (selectedIndexes.isEmpty()) {
        QMessageBox::information(this, "Delete", "No items selected for deletion.");
        return;
    }

    // Переменная для подтверждения удаления
    QMessageBox::StandardButton confirmDelete = QMessageBox::question(
        this,
        "Confirm Deletion",
        "Are you sure you want to delete the selected items?",
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No
        );

    if (confirmDelete == QMessageBox::Yes) {
        // Итерация по выбранным индексам
        foreach (const QModelIndex &index, selectedIndexes) {
            QFileInfo fileInfo = model->fileInfo(index);

            // Получаем путь к файлу или папке
            QString filePath = fileInfo.absoluteFilePath();

            // Перемещение файла или папки в корзину
            bool success = QFile::moveToTrash(filePath);

            // Проверка успеха перемещения
            if (!success) {
                QMessageBox::critical(this, "Delete Error", "Failed to delete file: " + filePath);
            }
        }
    }
}

//Удаление слева перм
void MainWindow::on_permanent_del_left_clicked()
{
    // Получаем модель выбора из listView'ов
    QItemSelectionModel* selectionModel = ui->listView_left->selectionModel();

    // Получаем список выбранных индексов
    QModelIndexList selectedIndexes = selectionModel->selectedIndexes();

    // Проверяем, есть ли выделенные элементы
    if (selectedIndexes.isEmpty()) {
        QMessageBox::information(this, "Delete", "No items selected for deletion.");
        return;
    }

    // Запрос подтверждения удаления и выбор между корзиной и полным удалением
    QMessageBox::StandardButton choice = QMessageBox::question(
        this,
        "Confirm Deletion",
        "Do you want to delete the selected items permanently?",
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No

        );

    if (choice == QMessageBox::Yes) {
        // Список путей к выбранным файлам
        QStringList filePaths;

        foreach (const QModelIndex &index, selectedIndexes) {
            QFileInfo fileInfo = model->fileInfo(index);
            filePaths.append(fileInfo.absoluteFilePath());
        }

        // Полное удаление файлов
        permanentlyDeleteFiles(filePaths);
    } else if (choice == QMessageBox::No) return;
}

//Удаление справа перм
void MainWindow::on_permanent_del_right_clicked()
{
    // Получаем модель выбора из listView'ов
    QItemSelectionModel* selectionModel = ui->listView_right->selectionModel();

    // Получаем список выбранных индексов
    QModelIndexList selectedIndexes = selectionModel->selectedIndexes();

    // Проверяем, есть ли выделенные элементы
    if (selectedIndexes.isEmpty()) {
        QMessageBox::information(this, "Delete", "No items selected for deletion.");
        return;
    }

    // Запрос подтверждения удаления и выбор между корзиной и полным удалением
    QMessageBox::StandardButton choice = QMessageBox::question(
        this,
        "Confirm Deletion",
        "Do you want to delete the selected items permanently?",
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No

        );

    if (choice == QMessageBox::Yes) {
        // Список путей к выбранным файлам
        QStringList filePaths;

        foreach (const QModelIndex &index, selectedIndexes) {
            QFileInfo fileInfo = model->fileInfo(index);
            filePaths.append(fileInfo.absoluteFilePath());
        }

        // Полное удаление файлов
        permanentlyDeleteFiles(filePaths);
    } else if (choice == QMessageBox::No) return;
}

