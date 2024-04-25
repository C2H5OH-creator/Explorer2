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
#include <QImageReader>
#include <QStandardItem>
#include <QMenu>
#include <QContextMenuEvent>
#include <QProcess>
#include <Windows.h>

#define VERB_OPEN 0x00000000

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    model = new QFileSystemModel(this);
    model->setRootPath("");
    ui->rename_enter->hide();

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
int listViewFocus = 0; // -1 left / +1 right

//Сохранение последнего фокуса
void MainWindow::properFocus()
{
    if (ui->listView_left->hasFocus()){
        listViewFocus = -1;
    }
    else if (ui->listView_right->hasFocus()){
        listViewFocus = 1;
    }else {
        listViewFocus = 0;
    }
}

//Сохранение модели относительно последнего фокуса
QItemSelectionModel* MainWindow::getModelFromFocusedListView() {
    if (listViewFocus == -1) {
        return ui->listView_left->selectionModel();
    }else if (listViewFocus == 1) {
        return ui->listView_right->selectionModel();
    } else if (listViewFocus == 0) {
        return nullptr;
    }
}

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

//Открытие папки слева через проводник
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

bool isImage(const QString &filePath) {
    QImageReader imageReader(filePath);
    return imageReader.canRead(); // Возвращает true, если файл является изображением
}


//Оккрытие папки/файла слева
void MainWindow::on_listView_left_doubleClicked(const QModelIndex &index)
{
    QFileInfo fileInfo = model->fileInfo(index);
    properFocus();
/*
    // Получаем модель выбора из listView'ов
    QItemSelectionModel* selectionModel = getModelFromFocusedListView();

    // Получаем список выбранных индексов
    QModelIndexList selected = selectionModel->selectedIndexes();

    QString filePath = fileInfo.filePath(); // Получаем путь к файлу

    // Проверяем, является ли файл изображением
    if (isImage(filePath)) {
        // Получаем соответствующий элемент модели по индексу
        QStandardItem *item = model->itemFromIndex(index);
        if (item) {
            // Загружаем изображение в QPixmap
            QPixmap originalPixmap(filePath);
            // Устанавливаем размер для маленькой версии изображения
            int width = 100; // Новая ширина
            int height = 100; // Новая высота
            // Изменяем размер изображения на указанный
            QPixmap smallPixmap = originalPixmap.scaled(width, height, Qt::KeepAspectRatio);

            // Устанавливаем иконку для элемента
            item->setIcon(QIcon(smallPixmap));
        }
    }
*/
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

//Открытие папки/файла справа
void MainWindow::on_listView_right_doubleClicked(const QModelIndex &index)
{
    QFileInfo fileInfo = model->fileInfo(index);
    properFocus();

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
    QItemSelectionModel* selectionModelRight = ui->listView_right->selectionModel();

    // Получаем список выбранных индексов
    QModelIndexList rightSelected = selectionModelRight->selectedIndexes();

    // Проверяем, есть ли выделенные элементы
    if (rightSelected.isEmpty()) {
        QMessageBox::information(this, "Copy", "No items selected for copying.");
        return;
    }

    // Создаем окно прогресса
    QProgressDialog progressDialog("Copying files...", "Cancel", 0, rightSelected.size(), this);
    progressDialog.setWindowModality(Qt::WindowModal);
    progressDialog.setWindowTitle("Copying Files");

    unsigned long copiedFilesCount = 0;
    bool operationCanceled = false;

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
            QString destPath = model->filePath(ui->listView_right->rootIndex()) + '/' + fileInfo.fileName();
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

//Функция рекурсивного копировния директорий
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
            // Рекурсивно копируем поддиректории
            copyDirectoryRecursively(srcFilePath, dstFilePath);
        } else {
            // Копируем файлы
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

//Удаление файлов
void MainWindow::on_delete_2_clicked()
{
    // Получаем модель выбора из listView'ов
    QItemSelectionModel* selectionModel = getModelFromFocusedListView();

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

//Удаление перм
void MainWindow::on_permanent_del_clicked()
{
    // Получаем модель выбора из listView'ов
    QItemSelectionModel* selectionModel = getModelFromFocusedListView();

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

//Открытие окна SD actions
void MainWindow::on_sd_actions_left_clicked()
{
    SD_settings sd_settings_right(this);
    connect(&sd_settings_right, &SD_settings::sendLeftData, this, &MainWindow::receiveLeftData);
    sd_settings_right.exec();
}

//Обработка действий из SD actions
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

//Переименование файлов часть 1
void MainWindow::on_rename_clicked()
{
    // Получаем модель выбора из listView'ов
    QItemSelectionModel* selectionModel = getModelFromFocusedListView();
    // Получаем список выбранных индексов
    QModelIndexList selected = selectionModel->selectedIndexes();
    ui->rename_enter->show();

    // Итерация по выбранным индексам
    foreach (const QModelIndex &index, selected) {
        if (selected.size() == 1){
            QFileInfo fileInfo = model->fileInfo(index);
            QString oldFileName = fileInfo.fileName();
            ui->rename_enter->setText(oldFileName);
            break;
        }else{
            QMessageBox::information(this, "Renaming", "You cannot rename multiple files at once!");
            break;
        }
    }
}

//Переименование файлов часть 2
void MainWindow::on_rename_enter_returnPressed()
{
    // Получаем модель выбора из listView'ов
    QItemSelectionModel* selectionModel = getModelFromFocusedListView();

    // Получаем список выбранных индексов
    QModelIndexList selected = selectionModel->selectedIndexes();

    // Итерация по выбранным индексам
    foreach (const QModelIndex &index, selected) {
        QFileInfo fileInfo = model->fileInfo(index);
        QString filePath = fileInfo.path();
        QString newSingleFileName = ui->rename_enter->text();
        QString oldPath = model->filePath(index);
        QDir dir = model->fileInfo(index).dir();

        ui->rename_enter->clear();
        ui->rename_enter->hide();

        // Проверяем тип файла (файл или папка)
        if (fileInfo.isFile()) {
            QString sourcePath = fileInfo.absoluteFilePath();
            // Проверяем, существует ли файл в месте назначения
            if (QFile::exists(sourcePath)) {
                if (selected.size() == 1){
                    QFile::rename (oldPath,  filePath + "\\" + newSingleFileName);
                    break;
                }
            }
        }
        if (fileInfo.isDir()) {
            QString destinationPath = dir.absolutePath() + "/" + newSingleFileName;
            // Проверяем, существует ли файл или папка с таким же именем в месте назначения
            if (!QDir(destinationPath).exists()) {
                if (selected.size() == 1) {
                    qDebug() << oldPath;
                    dir.rename(oldPath, newSingleFileName);
                    break;
                }
            } else {
                qDebug() << "Папка с именем" << newSingleFileName << "уже существует в этой директории.";
                // Здесь можно добавить обработку случая, когда папка с таким именем уже существует
            }
        }
    }
}

//Обработка фокуса-1
void MainWindow::on_listView_left_clicked(const QModelIndex &index)
{
    properFocus();
}

//Обработка фокуса-1
void MainWindow::on_listView_right_clicked(const QModelIndex &index)
{
    properFocus();
}

//Вид списка слева
void MainWindow::on_list_view_left_clicked()
{
    ui->listView_left->setUniformItemSizes(false);
    ui->listView_left->setViewMode(QListView::ListMode);
}


void MainWindow::on_icon_view_left_clicked()
{
    ui->listView_left->setUniformItemSizes(true);
    ui->listView_left->setWordWrap(true);
    ui->listView_left->setViewMode(QListView::IconMode);
}

//Вид списка справа
void MainWindow::on_list_view_right_clicked()
{
    ui->listView_right->setUniformItemSizes(false);
    ui->listView_right->setViewMode(QListView::ListMode);
}

//Вид иконок справа
void MainWindow::on_icon_view_right_clicked()
{
    ui->listView_right->setUniformItemSizes(true);
    ui->listView_right->setWordWrap(true);
    ui->listView_right->setViewMode(QListView::IconMode);
}

void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
    // Получаем модель выбора из listView'ов
    QItemSelectionModel* selectionModel = getModelFromFocusedListView();

    QMenu menu(this); // Создаем объект контекстного меню

    QAction *open = menu.addAction("Open");
    QAction *open_with = menu.addAction("Open with...");

    //Сортировка
    menu.addSeparator();
    QMenu *sortMenu = menu.addMenu("Sort by...");
    QAction *actionSortByName = sortMenu->addAction("Name");
    QAction *actionSortByDate = sortMenu->addAction("Data");
    QAction *actionSortBySize = sortMenu->addAction("Size");

    //Вид
    QMenu *viewMenu = menu.addMenu("View...");
    QAction *listView = viewMenu->addAction("List");
    QAction *iconView_EXL = viewMenu->addAction("Extra large icons");
    QAction *iconView_L = viewMenu->addAction("Large icons");
    QAction *iconView_M = viewMenu->addAction("Medium icons");
    QAction *iconView_S = viewMenu->addAction("Smal icons");

    QAction *refresh = menu.addAction("Refresh");
    menu.addSeparator();

    //Разное
    QAction *rename = menu.addAction("Rename");
    QAction *bin_delete = menu.addAction("Delete");
    QAction *perm_delete = menu.addAction("Peramnent delete");

    //Создание
    menu.addSeparator();
    QMenu *createMenu = menu.addMenu("New...");
    QAction *newFolder = createMenu->addAction("Folder");
    QAction *newFile = createMenu->addAction("File");
    menu.addSeparator();

    //Метаданные
    QAction *properties = menu.addAction("Properties");
    QAction *editMeta = menu.addAction("Edit metadata");

    // Выбираем действие, которое было выбрано пользователем
    QAction *selectedAction = menu.exec(event->globalPos());

    // Обрабатываем выбранное действие
    if (selectedAction == open){
        QModelIndexList selectedIndexes = selectionModel->selectedIndexes();
        foreach (const QModelIndex &index, selectedIndexes) {
            QFileInfo fileInfo = model->fileInfo(index);
            QDesktopServices::openUrl(QUrl::fromLocalFile(fileInfo.absoluteFilePath()));
            break;
        }
    }
    else if(selectedAction == open_with){
        QModelIndexList selectedIndexes = selectionModel->selectedIndexes();
        foreach (const QModelIndex &index, selectedIndexes) {
            QFileInfo fileInfo = model->fileInfo(index);
            //HINSTANCE hInstance = ShellExecuteA(0, VERB_OPEN, fileInfo.absoluteFilePath().toUtf8().constData(), 0, 0, SW_SHOWDEFAULT);
            //QDesktopServices::openUrl(QUrl::fromUserInput(fileInfo.absoluteFilePath()));
            //QString param = "/select,\"" + QDir::toNativeSeparators(fileInfo.absoluteFilePath()) + "\"";
            //QProcess::startDetached("explorer.exe", QStringList() << param);
            break;
        }
    }
    else if(selectedAction == actionSortByName){
        //Сортировка по имени

    }
    else if(selectedAction == actionSortByDate){
        //Сортировка по дате

    }
    else if(selectedAction == actionSortBySize){
        //Сортировка по размеру

    }
    else if(selectedAction == listView){
        //Вид как список
        if (listViewFocus == -1) on_list_view_left_clicked();
        if (listViewFocus == 1) on_list_view_right_clicked();
    }
    else if(selectedAction == iconView_EXL){
        //Вид как очень большие иконки
        if (listViewFocus == -1) on_icon_view_left_clicked();
        if (listViewFocus == 1) on_icon_view_right_clicked();

    }
    else if(selectedAction == iconView_L){
        //Вид как большие иконки
        if (listViewFocus == -1) on_icon_view_left_clicked();
        if (listViewFocus == 1) on_icon_view_right_clicked();

    }
    else if(selectedAction == iconView_M){
        //Вид как средние иконки
        if (listViewFocus == -1) on_icon_view_left_clicked();
        if (listViewFocus == 1) on_icon_view_right_clicked();

    }
    else if(selectedAction == iconView_S){
        //Вид как маленькие иконки
        if (listViewFocus == -1) on_icon_view_left_clicked();
        if (listViewFocus == 1) on_icon_view_right_clicked();

    }
    else if(selectedAction == refresh){
        //Обновление
        if (listViewFocus == -1) ui->listView_left->update();
        if (listViewFocus == 1) ui->listView_right->update();
    }
    else if (selectedAction == newFolder) {
        //Создание новой папки

    }
    else if (selectedAction == newFile) {
        //Создание нового файла

    }
    else if (selectedAction == bin_delete) {
        //Перемещение в корзину
        on_delete_2_clicked();
    }
    else if (selectedAction == rename){
        //Переименование
        on_rename_clicked();
    }
    else if (selectedAction == perm_delete) {
        //Удаление, минуя корзину
        on_permanent_del_clicked();
    }
    else if (selectedAction == properties) {
        //Свойства

    }
    else if (selectedAction == editMeta) {
        //Редактор метаданных

    }
}



