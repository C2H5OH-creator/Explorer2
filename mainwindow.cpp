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
#include <QShortcut>
#include <QProgressDialog>


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

    //HotKeys

    keyCtrl_Up = new QShortcut(this);   // Инициализируем объект
    keyCtrl_Up->setKey(Qt::CTRL | Qt::Key_Plus);    // Устанавливаем код клавиши
    // цепляем обработчик нажатия клавиши
    connect(keyCtrl_Up, SIGNAL(activated()), this, SLOT(slotShortcutCtrl_Up()));

    keyCtrl_Eq = new QShortcut(this);   // Инициализируем объект
    keyCtrl_Eq->setKey(Qt::CTRL | Qt::Key_Equal);    // Устанавливаем код клавиши
    // цепляем обработчик нажатия клавиши
    connect(keyCtrl_Eq, SIGNAL(activated()), this, SLOT(slotShortcutCtrl_Eq()));

    keyCtrl_Down = new QShortcut(this); // Инициализируем объект
    keyCtrl_Down->setKey(Qt::CTRL | Qt::Key_Minus); // Устанавливаем сочетание клавиш
    // подключаем обработчик нажатия клавиши
    connect(keyCtrl_Down, SIGNAL(activated()), this, SLOT(slotShortcutCtrl_Down()));

}

MainWindow::~MainWindow()
{
    delete ui;
}
int view = 0;

//Обработка изменение мастшаба
void MainWindow::viewStop(int &view){
    if(view > 0){
        if (ui->listView_left->hasFocus()) {
            ui->listView_left->setViewMode(QListView::IconMode);
            ui->listView_left->setIconSize(QSize(20, 20));
            ui->listView_left->setResizeMode(QListView::Adjust);
        }else if (ui->listView_right->hasFocus()){
            ui->listView_right->setViewMode(QListView::IconMode);
            ui->listView_right->setIconSize(QSize(20, 20));
            ui->listView_right->setResizeMode(QListView::Adjust);
        }
    }else{
        if (ui->listView_left->hasFocus()) {
            ui->listView_left->setViewMode(QListView::ListMode);
        }else if (ui->listView_right->hasFocus()){
            ui->listView_right->setViewMode(QListView::ListMode);
        }
    }
}

//Обработка CRTL +
void MainWindow::slotShortcutCtrl_Up(){
    viewStop(view);
    if (view >= 50) view = 50;
    if (view < 0) view = 0;
    if (ui->listView_left->hasFocus()) {
        view += 5;
        ui->listView_left->setIconSize(QSize(20 + view, 20 + view));
    }else if (ui->listView_right->hasFocus()){
        view += 5;
        ui->listView_right->setIconSize(QSize(20 + view, 20 + view));
    }
}
//Обработка CRTL =
void MainWindow::slotShortcutCtrl_Eq(){
    viewStop(view);
    if (view >= 50) view = 50;
    if (view < 0) view = 0;
    if (ui->listView_left->hasFocus()) {
        view += 5;
        ui->listView_left->setIconSize(QSize(20 + view, 20 + view));
    }else if (ui->listView_right->hasFocus()){
        view += 5;
        ui->listView_right->setIconSize(QSize(20 + view, 20 + view));
    }
}
//Обработка CRTL -
void MainWindow::slotShortcutCtrl_Down(){
    viewStop(view);
    if (view >= 50) view = 50;
    if (view < 0) view = 0;
    if (ui->listView_left->hasFocus()) {
        view -= 5;
        ui->listView_left->setIconSize(QSize(20 + view, 20 + view));
    }else if (ui->listView_right->hasFocus()){
        view -= 5;
        ui->listView_right->setIconSize(QSize(20 + view, 20 + view));
    }
}
//Обработка CRTL + колёсико мыши
void MainWindow::wheelEvent(QWheelEvent *event) {
    viewStop(view);

    // Проверяем, нажата ли клавиша Ctrl
    if (event->modifiers() & Qt::CTRL) {
        // Получаем значение вращения колесика мыши (вертикальное)
        int delta = event->angleDelta().y();

       // Колесико вращается вверх
        if (delta > 0) {
            if (view >= 50) view = 50;
            if (view < 0) view = 0;
            if (ui->listView_left->hasFocus()) {
                view += 5;
                ui->listView_left->setIconSize(QSize(20 + view, 20 + view));
            }else if (ui->listView_right->hasFocus()){
                view += 5;
                ui->listView_right->setIconSize(QSize(20 + view, 20 + view));
            }
         // Колесико вращается вниз
        } else if (delta < 0) {
            if (view >= 50) view = 50;
            if (view < 0) view = 0;
            if (ui->listView_left->hasFocus()) {
                view -= 5;
                ui->listView_left->setIconSize(QSize(20 + view, 20 + view));
            }else if (ui->listView_right->hasFocus()){
                view -= 5;
                ui->listView_right->setIconSize(QSize(20 + view, 20 + view));
            }
        }
        // Помечаем событие как обработанное, чтобы оно не передавалось дальше
        event->accept();
    } else {
        // Если клавиша Ctrl не нажата, передаем событие базовому классу для обработки
        QWidget::wheelEvent(event);
    }
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
    //Получаем модель выбора из listView'ов
    QItemSelectionModel* selectionModelLeft = ui->listView_right->selectionModel();

    // Получаем список выбранных индексов
    QModelIndexList leftSelected = selectionModelLeft->selectedIndexes();

    // Проверяем, есть ли выделенные элементы
    if (leftSelected.isEmpty()) {
        QMessageBox::information(this, "Copy", "No items selected for copying.");
        return;
    }

    // Создаем окно прогресса
    QProgressDialog progressDialog("Copying files...", "Cancel", 0, leftSelected.size(), this);
    progressDialog.setWindowModality(Qt::WindowModal);
    progressDialog.setWindowTitle("Copying Files");

    unsigned long copiedFilesCount = 0;
    bool operationCanceled = false;

    // Итерация по выбранным индексам
    foreach (const QModelIndex &index, leftSelected) {
        QFileInfo fileInfo = model->fileInfo(index);

        // Если операция была отменена, выходим из цикла
        if (operationCanceled) {
            break;
        }

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
            } else {
                copiedFilesCount++;
            }

        } else if (fileInfo.isDir()) {
            // Реализуем перемещение директорий рекурсивно
            QString destPath = model->filePath(ui->listView_left->rootIndex()) + '/' + fileInfo.fileName();
            copyDirectoryRecursively(fileInfo.absoluteFilePath(), destPath);
        }

        // Обновляем прогресс
        progressDialog.setValue(copiedFilesCount);

        // Проверяем, была ли нажата кнопка отмены
        if (progressDialog.wasCanceled()) {
            operationCanceled = true;
        }
    }

    // Показываем сообщение о завершении операции
    if (!operationCanceled) {
        QMessageBox::information(this, "Copying Files", "All files copied successfully!");
    } else {
        QMessageBox::warning(this, "Copying Files", "Copying operation was canceled!");
    }
}

//Копирование файлов справа-налево
void MainWindow::on_copy_to_right_clicked()
{
    //int copyIndex = 1;

    // Получаем модель выбора из listView'ов
    QItemSelectionModel* selectionModelLeft = ui->listView_left->selectionModel();

    // Получаем список выбранных индексов
    QModelIndexList leftSelected = selectionModelLeft->selectedIndexes();

    // Проверяем, есть ли выделенные элементы
    if (leftSelected.isEmpty()) {
        QMessageBox::information(this, "Copy", "No items selected for copying.");
        return;
    }

    // Создаем окно прогресса
    QProgressDialog progressDialog("Copying files...", "Cancel", 0, leftSelected.size(), this);
    progressDialog.setWindowModality(Qt::WindowModal);
    progressDialog.setWindowTitle("Copying Files");

    unsigned long copiedFilesCount = 0;
    bool operationCanceled = false;

    // Итерация по выбранным индексам
    foreach (const QModelIndex &index, leftSelected) {
        QFileInfo fileInfo = model->fileInfo(index);

        // Если операция была отменена, выходим из цикла
        if (operationCanceled) {
            break;
        }

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
            } else {
                copiedFilesCount++;
            }

        } else if (fileInfo.isDir()) {
            // Реализуем перемещение директорий рекурсивно
            QString destPath = model->filePath(ui->listView_left->rootIndex()) + '/' + fileInfo.fileName();
            copyDirectoryRecursively(fileInfo.absoluteFilePath(), destPath);
        }

        // Обновляем прогресс
        progressDialog.setValue(copiedFilesCount);

        // Проверяем, была ли нажата кнопка отмены
        if (progressDialog.wasCanceled()) {
            operationCanceled = true;
        }
    }

    // Показываем сообщение о завершении операции
    if (!operationCanceled) {
        QMessageBox::information(this, "Copying Files", "All files copied successfully!");
    } else {
        QMessageBox::warning(this, "Copying Files", "Copying operation was canceled!");
    }
}
/*
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
*/

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

    // Создаем окно прогресса
    QProgressDialog progressDialog("Moving files...", "Cancel", 0, leftSelected.size(), this);
    progressDialog.setWindowModality(Qt::WindowModal);
    progressDialog.setWindowTitle("Moving Files");

    unsigned long copiedFilesCount = 0;
    bool operationCanceled = false;

    // Переменная для отслеживания выбора пользователя "Да, заменить все"
    bool replaceAll = false;

    // Итерация по выбранным индексам
    foreach (const QModelIndex &index, leftSelected) {
        QFileInfo fileInfo = model->fileInfo(index);

        // Если операция была отменена, выходим из цикла
        if (operationCanceled) {
            break;
        }

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
            }else {
                copiedFilesCount++;
            }
        } else if (fileInfo.isDir()) {
            // Реализуем перемещение директорий рекурсивно
            QString destPath = model->filePath(ui->listView_right->rootIndex()) + '/' + fileInfo.fileName();
            moveDirectoryRecursively(fileInfo.absoluteFilePath(), destPath);
        }
        // Обновляем прогресс
        progressDialog.setValue(copiedFilesCount);

        // Проверяем, была ли нажата кнопка отмены
        if (progressDialog.wasCanceled()) {
            operationCanceled = true;
        }
    }
    // Показываем сообщение о завершении операции
    if (!operationCanceled) {
        QMessageBox::information(this, "Moving Files", "All files moved successfully!");
    } else {
        QMessageBox::warning(this, "Moving Files", "Moving operation was canceled!");
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

    // Создаем окно прогресса
    QProgressDialog progressDialog("Copying files...", "Cancel", 0, rightSelected.size(), this);
    progressDialog.setWindowModality(Qt::WindowModal);
    progressDialog.setWindowTitle("Copying Files");

    unsigned long copiedFilesCount = 0;
    bool operationCanceled = false;

    // Переменная для отслеживания выбора пользователя "Да, заменить все"
    bool replaceAll = false;

    // Итерация по выбранным индексам
    foreach (const QModelIndex &index, rightSelected) {
        QFileInfo fileInfo = model->fileInfo(index);

        // Если операция была отменена, выходим из цикла
        if (operationCanceled) {
            break;
        }


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
                else {
                    copiedFilesCount++;
                }
            }
        } else if (fileInfo.isDir()) {
            // Реализуем перемещение директорий рекурсивно
            QString destPath = model->filePath(ui->listView_left->rootIndex()) + '/' + fileInfo.fileName();
            moveDirectoryRecursively(fileInfo.absoluteFilePath(), destPath);
        }
        // Обновляем прогресс
        progressDialog.setValue(copiedFilesCount);

        // Проверяем, была ли нажата кнопка отмены
        if (progressDialog.wasCanceled()) {
            operationCanceled = true;
        }
    }
    // Показываем сообщение о завершении операции
    if (!operationCanceled) {
        QMessageBox::information(this, "Moving Files", "All files moved successfully!");
    } else {
        QMessageBox::warning(this, "Moving Files", "Moving operation was canceled!");
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


void MainWindow::on_sd_actions_left_clicked()
{
    SD_settings sd_settings_right(this);
    connect(&sd_settings_right, &SD_settings::sendLeftData, this, &MainWindow::receiveLeftData);
    sd_settings_right.exec();
}

void MainWindow::receiveLeftData(int *settings)
{

    /*
        settings[0] - Copy_type
        settings[1] - Move_type
        settings[2] - folder_settings
        settings[3] - Raw_to_JPEG
        settings[4] - MTS_to_MP4

    */

    // Создаем объект QDir для исходной директории
    QDir sourceDir(model->filePath(ui->listView_left->rootIndex())+ "\\DCIM\\103MSDCF");

    // Создаем объект QDir для исходной директории
    QDir destDir(model->filePath(ui->listView_right->rootIndex()));

    // Получаем список файлов в исходной директории
    QStringList fileList = sourceDir.entryList(QDir::Files);

    // Создаем окно прогресса
    QProgressDialog progressDialog("Copying files...", "Cancel", 0, fileList.size(), this);
    progressDialog.setWindowModality(Qt::WindowModal);
    progressDialog.setWindowTitle("Copying Files");

    unsigned long copiedFilesCount = 0;
    bool operationCanceled = false;

    // Проходим по каждому файлу
    foreach (const QString &fileName, fileList) {
        // Получаем полный путь к файлу
        QString filePath = sourceDir.filePath(fileName);

        // Получаем информацию о файле
        QFileInfo fileInfo(filePath);

        // Получаем формат файла
        QString format = fileInfo.suffix().toLower();
        QDir destinationFormatDir(destDir);

        // Если операция была отменена, выходим из цикла
        if (operationCanceled) {
            break;
        }
        qDebug() << settings[2];
        // Если пользователь выбрал создавать папки
        if (settings[2] == 1){
            // Создаем папку для данного формата, если она еще не существует
            if (!destinationFormatDir.exists(format)) {
                if (!destinationFormatDir.mkdir(format)) {
                    qDebug() << "Failed to create directory for format:" << format;
                    continue;
                }
            }
            // Копируем файл в соответствующую папку
            if(settings[0] == 1){
                QString destinationFilePath = destinationFormatDir.filePath(format + "/" + fileName);
                if (!QFile::copy(filePath, destinationFilePath)) {
                    qDebug() << "Failed to copy file:" << filePath << "to" << destinationFilePath;
                } else {
                    copiedFilesCount++;
                }
                // Перемещаем файл в соответствующую папку
            } else if (settings[1] == 1){
                QString destinationFilePath = destinationFormatDir.filePath(format + "/" + fileName);
                if (!QFile::rename(filePath, destinationFilePath)) {
                    qDebug() << "Failed to move file:" << filePath << "to" << destinationFilePath;
                } else {
                    copiedFilesCount++;
                }
            }
        }
        if (settings[2] == 0){
            // Копируем файл в соответствующую папку
            if(settings[0] == 1){
                QString destinationFilePath = destDir.filePath(fileName); // Используем filePath() вместо path()
                if (!QFile::copy(filePath, destinationFilePath)) {
                    qDebug() << "Failed to copy file:" << filePath << "to" << destinationFilePath;
                } else {
                    copiedFilesCount++;
                }
                // Перемещаем файл в соответствующую папку
            } else if (settings[1] == 1){
                QString destinationFilePath = destDir.filePath(fileName); // Используем filePath() вместо path()
                if (!QFile::rename(filePath, destinationFilePath)) {
                    qDebug() << "Failed to move file:" << filePath << "to" << destinationFilePath;
                } else {
                    copiedFilesCount++;
                }
            }
        }

        // Обновляем прогресс
        progressDialog.setValue(copiedFilesCount);

        // Проверяем, была ли нажата кнопка отмены
        if (progressDialog.wasCanceled()){
            operationCanceled = true;
        }
    }

    // Показываем сообщение о завершении операции
    if (copiedFilesCount == fileList.size() && !operationCanceled) {
        QMessageBox::information(this, "Copying Files", "All files copied successfully!");
    } else {
        QMessageBox::warning(this, "Copying Files", "Copying operation was canceled or failed!");
    }

    qDebug() << "Organizing images completed.";
    delete[] settings;
}


void MainWindow::on_sd_actions_right_clicked()
{
    SD_settings sd_settings_right(this);
    connect(&sd_settings_right, &SD_settings::sendRightData, this, &MainWindow::receiveRightData);
    sd_settings_right.exec();
}

void MainWindow::receiveRightData(int *settings)
{
    /*
        settings[0] - Copy_type
        settings[1] - Move_type
        settings[2] - folder_settings
        settings[3] - Raw_to_JPEG
        settings[4] - MTS_to_MP4

    */

    //\DCIM\103MSDCF - путь для фото
    //\PRIVATE\AVCHD\BDMV\STREAM - путь для видео

    // Создаем объект QDir для исходной директории
    QDir sourceDir(model->filePath(ui->listView_right->rootIndex())+ "\\DCIM\\103MSDCF");

    // Создаем объект QDir для исходной директории
    QDir destDir(model->filePath(ui->listView_left->rootIndex()));

    // Получаем список файлов в исходной директории
    QStringList fileList = sourceDir.entryList(QDir::Files);

    // Создаем окно прогресса
    QProgressDialog progressDialog("Copying files...", "Cancel", 0, fileList.size(), this);
    progressDialog.setWindowModality(Qt::WindowModal);
    progressDialog.setWindowTitle("Copying Files");

    unsigned long copiedFilesCount = 0;

    // Проходим по каждому файлу
    foreach (const QString &fileName, fileList) {
        // Получаем полный путь к файлу
        QString filePath = sourceDir.filePath(fileName);

        // Получаем информацию о файле
        QFileInfo fileInfo(filePath);

        // Получаем формат файла
        QString format = fileInfo.suffix().toLower();
        QDir destinationFormatDir(destDir);

        //Если пользователь выбрал создавать папки
        if (settings[2] == 1){
            // Создаем папку для данного формата, если она еще не существует
            if (!destinationFormatDir.exists(format)) {
                if (!destinationFormatDir.mkdir(format)) {
                    qDebug() << "Failed to create directory for format:" << format;
                    continue;
                }
            }
            // Копируем файл в соответствующую папку
            if(settings[0] == 1){
                QString destinationFilePath = destinationFormatDir.filePath(format + "\\" + fileName);
                if (!QFile::copy(filePath, destinationFilePath)) {
                    qDebug() << "Failed to copy file:" << filePath << "to" << destinationFilePath;
                }else copiedFilesCount++;
                // Перемещаем файл в соответствующую папку
            }else if (settings[1] == 1){
                QString destinationFilePath = destinationFormatDir.filePath(format + "\\" + fileName);
                if (!QFile::rename(filePath, destinationFilePath)) {
                    qDebug() << "Failed to move file:" << filePath << "to" << destinationFilePath;
                }else copiedFilesCount++;
            }
        }
        else if (settings[2] == 0){
            // Копируем файл в соответствующую папку
            if(settings[0] == 1){
                if (!QFile::copy(filePath, destDir.absolutePath())) {
                    qDebug() << "Failed to copy file:" << filePath << "to" << destDir.absolutePath();
                }else copiedFilesCount++;
                // Перемещаем файл в соответствующую папку
            }else if (settings[1] == 1){
                if (!QFile::rename(filePath, destDir.absolutePath())) {
                    qDebug() << "Failed to move file:" << filePath << "to" << destDir.absolutePath();
                }else copiedFilesCount++;
            }
            // Обновляем прогресс
            progressDialog.setValue(copiedFilesCount);

            // Проверяем, была ли нажата кнопка отмены
            if (progressDialog.wasCanceled())
                break;
        }

        // Показываем сообщение о завершении операции
        if (copiedFilesCount == fileList.size()) {
            QMessageBox::information(this, "Copying Files", "All files copied successfully!");
        } else {
            QMessageBox::warning(this, "Copying Files", "Copying operation was canceled or failed!");
        }
    }

    qDebug() << "Organizing images completed.";
    delete[] settings;
}

//Жонглирование путями
void MainWindow::on_switch_paths_clicked()
{
    QModelIndex sourcePath = ui->listView_left->rootIndex();
    QModelIndex destDirPath = ui->listView_right->rootIndex();

    ui->listView_left->setRootIndex(destDirPath);
    ui->listView_right->setRootIndex(sourcePath);

}
