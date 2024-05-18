#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "list.h"
#include "myqfilesystemmodel.h"

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
#include <QImage>
#include <QBuffer>

int view = 0;
int viewType = 0; // 0 = List | 1 = Icon

int iconDefHieght = 20;
int iconDefWidth = 20;

int iconHieght = iconDefHieght + view;
int iconWidth = iconDefWidth + view;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    model = new MyQFileSystemModel(this);
    connect(this, &MainWindow::sendToModel, model, &MyQFileSystemModel::receiveToModel);
    model->setRootPath("");
    ui->rename_enter->hide();
    ui->new_name->hide();

    //Left
    ui->listView_left->setModel(model);
    ui->listView_left->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->left_path->setPlaceholderText("");
    ui->left_path->setReadOnly(true);
    ui->left_path->hide();
    ui->listView_left->viewport()->setAcceptDrops(false);

    //Right
    ui->listView_right->setModel(model);
    ui->listView_right->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->right_path->setPlaceholderText("");
    ui->right_path->setReadOnly(true);
    ui->right_path->hide();
    ui->listView_right->viewport()->setAcceptDrops(false);

    //HotKeys

    keyCtrl_Up = new QShortcut(this);   // Инициализируем объект
    keyCtrl_Up->setKey(Qt::CTRL | Qt::Key_Plus);    // Устанавливаем код клавиши
    // цепляем обработчик нажатия клавиши
    connect(keyCtrl_Up, SIGNAL(activated()), this, SLOT(slotShortcutCtrl_Up()));

    keyCtrl_Eq = new QShortcut(this);
    keyCtrl_Eq->setKey(Qt::CTRL | Qt::Key_Equal);    // Устанавливаем код клавиши
    // цепляем обработчик нажатия клавиши
    connect(keyCtrl_Eq, SIGNAL(activated()), this, SLOT(slotShortcutCtrl_Eq()));

    keyCtrl_Down = new QShortcut(this);
    keyCtrl_Down->setKey(Qt::CTRL | Qt::Key_Minus); // Устанавливаем сочетание клавиш
    // подключаем обработчик нажатия клавиши
    connect(keyCtrl_Down, SIGNAL(activated()), this, SLOT(slotShortcutCtrl_Down()));

    // шорткат bin_delete
    QShortcut *bin_del = new QShortcut(QKeySequence(Qt::Key_Delete), this);
    connect(bin_del, &QShortcut::activated, this, &MainWindow::on_delete_2_clicked);

    // шорткат perm_delete
    QShortcut *perm_del = new QShortcut(QKeySequence(Qt::SHIFT | Qt::Key_Delete), this);
    connect(perm_del, &QShortcut::activated, this, &MainWindow::on_permanent_del_clicked);
}

MainWindow::~MainWindow()
{
    delete ui;
}

List left_history = List();
List right_history = List();

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
    //qDebug() << viewType;
    //qDebug() << view;
    //qDebug() << iconHieght;
    //qDebug() << iconWidth;

    int *set_arr = new int[3];

    set_arr[0] = viewType;
    set_arr[1] = iconHieght;
    set_arr[2] = iconWidth;

    emit sendToModel(set_arr);


    if(view > 0){
        viewType = 1;
        if (ui->listView_left->hasFocus()) {
            ui->listView_left->setViewMode(QListView::IconMode);
            ui->listView_left->setIconSize(QSize(iconDefHieght, iconDefWidth));
            ui->listView_left->setResizeMode(QListView::Adjust);
        }else if (ui->listView_right->hasFocus()){
            ui->listView_right->setViewMode(QListView::IconMode);
            ui->listView_right->setIconSize(QSize(iconDefHieght, iconDefWidth));
            ui->listView_right->setResizeMode(QListView::Adjust);
        }
    }else{
        viewType = 0;
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
    if (view >= 80) view = 80;
    if (view < 0) view = 0;

    iconHieght = iconDefHieght + view; // Обновляем значение iconHieght
    iconWidth = iconDefWidth + view;   // Обновляем значение iconWidth

    if (ui->listView_left->hasFocus()) {
        view += 5;
        ui->listView_left->setIconSize(QSize(iconHieght, iconWidth));
    }else if (ui->listView_right->hasFocus()){
        view += 5;
        ui->listView_right->setIconSize(QSize(iconHieght, iconWidth));
    }
}

//Обработка CRTL =
void MainWindow::slotShortcutCtrl_Eq(){
    viewStop(view);

    if (view >= 80) view = 80;
    if (view < 0) view = 0;

    iconHieght = iconDefHieght + view; // Обновляем значение iconHieght
    iconWidth = iconDefWidth + view;   // Обновляем значение iconWidth

    if (ui->listView_left->hasFocus()) {
        view += 5;
        ui->listView_left->setIconSize(QSize(iconHieght, iconWidth));
    }else if (ui->listView_right->hasFocus()){
        view += 5;
        ui->listView_right->setIconSize(QSize(iconHieght, iconWidth));
    }
}

//Обработка CRTL -
void MainWindow::slotShortcutCtrl_Down(){
    viewStop(view);

     if (view >= 80) view = 80;
    if (view < 0) view = 0;

    iconHieght = iconDefHieght + view; // Обновляем значение iconHieght
    iconWidth = iconDefWidth + view;   // Обновляем значение iconWidth

    if (ui->listView_left->hasFocus()) {
        view -= 5;
        ui->listView_left->setIconSize(QSize(iconHieght, iconWidth));
    }else if (ui->listView_right->hasFocus()){
        view -= 5;
        ui->listView_right->setIconSize(QSize(iconHieght, iconWidth));
    }
}

//Обработка CRTL + колёсико мыши
void MainWindow::wheelEvent(QWheelEvent *event) {

    viewStop(view);

    iconHieght = iconDefHieght + view; // Обновляем значение iconHieght
    iconWidth = iconDefWidth + view;   // Обновляем значение iconWidth


    // Проверяем, нажата ли клавиша Ctrl
    if (event->modifiers() & Qt::CTRL) {
        // Получаем значение вращения колесика мыши (вертикальное)
        int delta = event->angleDelta().y();

        // Колесико вращается вверх
        if (delta > 0) {
             if (view >= 80) view = 80;
            if (view < 0) view = 0;
            if (ui->listView_left->hasFocus()) {
                view += 5;
                ui->listView_left->setIconSize(QSize(iconHieght, iconWidth));
            }else if (ui->listView_right->hasFocus()){
                view += 5;
                ui->listView_right->setIconSize(QSize(iconHieght, iconWidth));
            }
            // Колесико вращается вниз
        } else if (delta < 0) {
             if (view >= 80) view = 80;
            if (view < 0) view = 0;
            if (ui->listView_left->hasFocus()) {
                view -= 5;
                ui->listView_left->setIconSize(QSize(iconHieght, iconWidth));
            }else if (ui->listView_right->hasFocus()){
                view -= 5;
                ui->listView_right->setIconSize(QSize(iconHieght, iconWidth));
            }
        }
        // Помечаем событие как обработанное, чтобы оно не передавалось дальше
        event->accept();
    } else {
        // Если клавиша Ctrl не нажата, передаем событие базовому классу для обработки
        QWidget::wheelEvent(event);
    }
}

//Установка пути в сточку (виджет старого отображения пути, оставлен как очень удобный костыль :D )
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
        QModelIndex rootIndex = model->index(directoryPath);
        ui->listView_left->setRootIndex(rootIndex);
    }
}

//Открытие папки/файла слева
void MainWindow::on_listView_left_doubleClicked(const QModelIndex &index)
{
    QFileInfo fileInfo = model->fileInfo(index);
    properFocus();

    if (fileInfo.isDir()){
        left_history.add_to_start(model->filePath(index));
        Lhindex = 0;
        ui->comboBox_Lhistory->clear();
        for(int i = 0; i < left_history.length(); i++){
            ui->comboBox_Lhistory->addItem(left_history.get(i));
        }
        ui->comboBox_Lhistory->setCurrentIndex(0);
        ui->listView_left->setRootIndex(index);
        setPathView(0, model->filePath(index));
    }
    else if (QFileInfo(fileInfo.path()).exists()) {
        QDesktopServices::openUrl(QUrl::fromLocalFile(fileInfo.absoluteFilePath()));
    }
    else {
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

    if (parentPath == "") {
        ui->listView_left->setRootIndex(model->index(""));
    }
    else{
        QDir dir = fileInfo.dir();
        dir.cd(left_history.get(++Lhindex));
        qDebug() << Lhindex;
        ui->comboBox_Lhistory->clear();

        for(int i = 0; i < left_history.length(); i++){
            ui->comboBox_Lhistory->addItem(left_history.get(i));
        }
        ui->comboBox_Lhistory->setCurrentText(left_history.get(Lhindex));
        ui->listView_left->setRootIndex(model->index(dir.absolutePath()));
        ui->left_path->setText(dir.absolutePath());
    }
}

void MainWindow::on_comboBox_Lhistory_currentIndexChanged(int index)
{
    // Получаем нынешний root index
    QModelIndex currentRootIndex = ui->listView_left->rootIndex();
    QFileInfo fileInfo = model->fileInfo(currentRootIndex);
    QDir dir = fileInfo.dir();
    dir.cd(left_history.get(index));
    ui->listView_left->setRootIndex(model->index(dir.absolutePath()));
    if (index > 0) Lhindex = index;

    //qDebug() <<"Lhindex" << Lhindex;
}

void MainWindow::on_forward_left_clicked()
{
    // Получаем нынешний root index
    QModelIndex currentRootIndex = ui->listView_left->rootIndex();
    QFileInfo fileInfo = model->fileInfo(currentRootIndex);
    QString parentPath = currentRootIndex.parent().data(QFileSystemModel::FilePathRole).toString();
    //Обновление пути
    setPathView(0, parentPath);

    QDir dir = fileInfo.dir();

    if (Lhindex <= 0) {
        Lhindex = 0;
    }
    else{
        Lhindex--;
    }
    dir.cd(left_history.get(Lhindex));
    qDebug() << Lhindex;
    ui->comboBox_Lhistory->clear();

    for(int i = 0; i < left_history.length(); i++){
        ui->comboBox_Lhistory->addItem(left_history.get(i));
    }
    ui->comboBox_Lhistory->setCurrentText(left_history.get(Lhindex));
    ui->listView_left->setRootIndex(model->index(dir.absolutePath()));
    ui->left_path->setText(dir.absolutePath());
}

//Открытие папки/файла справа
void MainWindow::on_listView_right_doubleClicked(const QModelIndex &index)
{
    QFileInfo fileInfo = model->fileInfo(index);
    properFocus();

    if (fileInfo.isDir()){
        right_history.add_to_start(model->filePath(index));
        Rhindex = 0;
        ui->comboBox_Rhistory->clear();
        for(int i = 0; i < right_history.length(); i++){
            ui->comboBox_Rhistory->addItem(right_history.get(i));
        }
        ui->comboBox_Rhistory->setCurrentIndex(0);

        ui->listView_right->setRootIndex(index);
        //Обновление пути
        setPathView(1, model->filePath(index));
    }
    else if (QFileInfo(fileInfo.path()).exists()) {
        QDesktopServices::openUrl(QUrl::fromLocalFile(fileInfo.absoluteFilePath()));
    }
    else {
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
        dir.cd(right_history.get(++Rhindex));
        qDebug() << Rhindex;
        ui->comboBox_Rhistory->clear();

        for(int i = 0; i < right_history.length(); i++){
            ui->comboBox_Rhistory->addItem(right_history.get(i));
        }
        ui->comboBox_Rhistory->setCurrentText(right_history.get(Rhindex));
        ui->listView_right->setRootIndex(model->index(dir.absolutePath()));
    }
}

void MainWindow::on_comboBox_Rhistory_currentIndexChanged(int index)
{
    // Получаем нынешний root index
    QModelIndex currentRootIndex = ui->listView_right->rootIndex();
    QFileInfo fileInfo = model->fileInfo(currentRootIndex);
    QDir dir = fileInfo.dir();
    dir.cd(right_history.get(index));
    ui->listView_right->setRootIndex(model->index(dir.absolutePath()));
    if (index > 0) Rhindex = index;

    //qDebug() <<"Rhindex" << Rhindex;
}

void MainWindow::on_forward_right_clicked()
{
    // Получаем нынешний root index
    QModelIndex currentRootIndex = ui->listView_right->rootIndex();
    QFileInfo fileInfo = model->fileInfo(currentRootIndex);
    QString parentPath = currentRootIndex.parent().data(QFileSystemModel::FilePathRole).toString();
    //Обновление пути
    setPathView(0, parentPath);

    QDir dir = fileInfo.dir();

    if (Rhindex <= 0) {
        Rhindex = 0;
    }
    else{
        Rhindex--;
    }
    dir.cd(right_history.get(Rhindex));
    qDebug() << Rhindex;
    ui->comboBox_Rhistory->clear();

    for(int i = 0; i < right_history.length(); i++){
        ui->comboBox_Rhistory->addItem(right_history.get(i));
    }
    ui->comboBox_Rhistory->setCurrentText(right_history.get(Rhindex));
    ui->listView_right->setRootIndex(model->index(dir.absolutePath()));
    ui->right_path->setText(dir.absolutePath());
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
        QModelIndex rootIndex = model->index(directoryPath);
        ui->listView_left->setRootIndex(rootIndex);
    }
}

//Открытие папки справа через проводник
void MainWindow::on_output_clicked()
{
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::Directory);
    QString directoryPath = dialog.getExistingDirectory();

    if (!directoryPath.isEmpty()) {
        //Устанавливаем в модель нужный нам путь
        model->setRootPath(directoryPath);
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
    QDir source = sourceDir;
    QDir dest = destDir;

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
    /*
     connect(откуда, кто отправит, кому, кто примит);
     1+2 потом 3+4/ Первые два слота для отправителя, вторые два для получателя
     откуда.h -- signals: send
     куда.h -- public slots: receive
    */

    SD_settings sd_settings(this);
    connect(&sd_settings, &SD_settings::sendSDActionsData, this, &MainWindow::receiveSDActionsData);

    connect(this, &MainWindow::sendFromPath, &sd_settings, &SD_settings::receiveFromPath);

    emit sendToPath(ui->right_path->text());
    emit sendFromPath(ui->left_path->text());

    sd_settings.exec();
}

//Обработка действий из SD actions
void MainWindow::receiveSDActionsData(int *settings)
{

    /*
        settings[0] - Copy_type
        settings[1] - Move_type
        settings[2] - folder_settings
        settings[3] - Raw_to_JPEG
        settings[4] - MTS_to_MP4
        settings[5] - 0 - Photo && video || 1 - Only photo || 2 - Only video

    */

    //Обработка и фото, и видео
    if(settings[5] == 0){
        //Директории для фото и видео
        QDir sourcePhotoDir = model->filePath(ui->listView_left->rootIndex()) + "/DCIM/103MSDCF";
        QDir sourceVideoDir = model->filePath(ui->listView_left->rootIndex()) + "/PRIVATE/AVCHD/BDMV/STREAM";
        QDir destDir = model->filePath(ui->listView_right->rootIndex());

        // Получаем список файлов в исходной директории
        QStringList filePhotoList = sourcePhotoDir.entryList(QDir::Files);
        QStringList fileVideoList = sourceVideoDir.entryList(QDir::Files);

        // Создаем окно прогресса
        QProgressDialog progressDialog("Doing things...", "Cancel", 0, filePhotoList.size() + fileVideoList.size(), this);
        progressDialog.setWindowModality(Qt::WindowModal);
        if(settings[1] == 1) {
            progressDialog.setWindowTitle("Moving Files");
        }
        else if(settings[0] == 1) progressDialog.setWindowTitle("Copying Files");

        unsigned long copiedFilesCount = 0;
        bool operationCanceled = false;

        // Проходим по каждому фото
        foreach (const QString &fileName, filePhotoList) {
            // Получаем полный путь к файлу
            QString filePath = sourcePhotoDir.filePath(fileName);

            // Получаем информацию о файле
            QFileInfo fileInfo(filePath);

            // Получаем формат файла
            QString format = fileInfo.suffix().toLower();
            QDir destinationFormatDir = destDir;

            // Если операция была отменена, выходим из цикла
            if (operationCanceled) {
                break;
            }

            // Если пользователь выбрал создавать папки
            if (settings[2] == 1){
                // Создаем папку для данного формата, если она еще не существует
                if (!destinationFormatDir.exists("Photo")) {
                    if (!destinationFormatDir.mkdir("Photo")) {
                        qDebug() << "Failed to create directory Photo :" << format;
                        continue;
                    }
                }
                QDir destinationPhotoFormatDir = model->filePath(ui->listView_right->rootIndex()) + "/Photo";
                // Создаем папку для данного формата, если она еще не существует
                if (!destinationPhotoFormatDir.exists(format)) {
                    if (!destinationPhotoFormatDir.mkdir(format)) {
                        qDebug() << "Failed to create directory for format:" << format;
                        continue;
                    }
                }
                // Копируем файл в соответствующую папку
                if(settings[0] == 1){
                    QString destinationFilePath = destinationPhotoFormatDir.filePath(format + "/" + fileName);
                    if (!QFile::copy(filePath, destinationFilePath)) {
                        qDebug() << "Failed to copy file:" << filePath << "to" << destinationFilePath;
                    } else {
                        copiedFilesCount++;
                    }
                    // Перемещаем файл в соответствующую папку
                } else if (settings[1] == 1){
                    QString destinationFilePath = destinationPhotoFormatDir.filePath(format + "/" + fileName);
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
                    QString destinationFilePath = destDir.filePath(fileName);
                    if (!QFile::copy(filePath, destinationFilePath)) {
                        qDebug() << "Failed to copy file:" << filePath << "to" << destinationFilePath;
                    } else {
                        copiedFilesCount++;
                    }
                    // Перемещаем файл в соответствующую папку
                } else if (settings[1] == 1){
                    QString destinationFilePath = destDir.filePath(fileName);
                    if (!QFile::rename(filePath, destinationFilePath)) {
                        qDebug() << "Failed to move file:" << filePath << "to" << destinationFilePath;
                    } else {
                        copiedFilesCount++;
                    }
                }
            }
            //Обновляем прогресс
            progressDialog.setValue(copiedFilesCount);

            // Проверяем, была ли нажата кнопка отмены
            if (progressDialog.wasCanceled()){
                operationCanceled = true;
            }
        }

        // Проходим по каждому видео
        foreach (const QString &fileName, fileVideoList) {
            // Получаем полный путь к файлу
            QString filePath = sourceVideoDir.filePath(fileName);

            // Получаем информацию о файле
            QFileInfo fileInfo(filePath);

            // Получаем формат файла
            QString format = fileInfo.suffix().toLower();
            QDir destinationFormatDir = destDir;

            // Если операция была отменена, выходим из цикла
            if (operationCanceled) {
                break;
            }

            // Если пользователь выбрал создавать папки
            if (settings[2] == 1){
                // Создаем папку для данного формата, если она еще не существует
                if (!destinationFormatDir.exists("Video")) {
                    if (!destinationFormatDir.mkdir("Video")) {
                        qDebug() << "Failed to create directory Video :" << format;
                        continue;
                    }
                }
                QDir destinationPhotoFormatDir = model->filePath(ui->listView_right->rootIndex()) + "/Video";
                // Создаем папку для данного формата, если она еще не существует
                if (!destinationPhotoFormatDir.exists(format)) {
                    if (!destinationPhotoFormatDir.mkdir(format)) {
                        qDebug() << "Failed to create directory for format:" << format;
                        continue;
                    }
                }
                // Копируем файл в соответствующую папку
                if(settings[0] == 1){
                    QString destinationFilePath = destinationPhotoFormatDir.filePath(format + "/" + fileName);
                    if (!QFile::copy(filePath, destinationFilePath)) {
                        qDebug() << "Failed to copy file:" << filePath << "to" << destinationFilePath;
                    } else {
                        copiedFilesCount++;
                    }
                    // Перемещаем файл в соответствующую папку
                } else if (settings[1] == 1){
                    QString destinationFilePath = destinationPhotoFormatDir.filePath(format + "/" + fileName);
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
                    QString destinationFilePath = destDir.filePath(fileName);
                    if (!QFile::copy(filePath, destinationFilePath)) {
                        qDebug() << "Failed to copy file:" << filePath << "to" << destinationFilePath;
                    } else {
                        copiedFilesCount++;
                    }
                    // Перемещаем файл в соответствующую папку
                } else if (settings[1] == 1){
                    QString destinationFilePath = destDir.filePath(fileName);
                    if (!QFile::rename(filePath, destinationFilePath)) {
                        qDebug() << "Failed to move file:" << filePath << "to" << destinationFilePath;
                    } else {
                        copiedFilesCount++;
                    }
                }
            }
            //Обновляем прогресс
            progressDialog.setValue(copiedFilesCount);

            // Проверяем, была ли нажата кнопка отмены
            if (progressDialog.wasCanceled()){
                operationCanceled = true;
            }
        }

        // Показываем сообщение о завершении операции
        if (copiedFilesCount == filePhotoList.size() + fileVideoList.size() && !operationCanceled) {
            if(settings[1] == 1) QMessageBox::information(this, "Moving Files", "All files moved successfully!");
            if(settings[0] == 1) QMessageBox::information(this, "Copying Files", "All files copied successfully!");
        } else {
            if(settings[1] == 1) QMessageBox::warning(this, "Moving Files", "Moving operation was canceled or failed!");
            if(settings[0] == 1) QMessageBox::warning(this, "Copying Files", "Copying operation was canceled or failed!");
        }

        qDebug() << "Organizing images completed.";
        delete[] settings;

    }
    //Обработка фото или видео
    else{
        //Если sett[5] == 1, то фотки, если sett[5] == 2, то видео
        QDir sourceDir = (settings[5] == 1) ? (model->filePath(ui->listView_left->rootIndex()) + "/DCIM/103MSDCF")
                                            : ((settings[5] == 2)
                                                   ? (model->filePath(ui->listView_left->rootIndex()) + "/PRIVATE/AVCHD/BDMV/STREAM")
                                                   : (NULL));

        QDir destDir = model->filePath(ui->listView_right->rootIndex());

        // Получаем список файлов в исходной директории
        QStringList fileList = sourceDir.entryList(QDir::Files);

        QProgressDialog progressDialog("Doing things...", "Cancel", 0, fileList.size(), this);
        progressDialog.setWindowModality(Qt::WindowModal);
        if(settings[1] == 1) {
            progressDialog.setWindowTitle("Moving Files");
        }
        else if(settings[0] == 1) progressDialog.setWindowTitle("Copying Files");


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
                    QString destinationFilePath = destDir.filePath(fileName);
                    if (!QFile::copy(filePath, destinationFilePath)) {
                        qDebug() << "Failed to copy file:" << filePath << "to" << destinationFilePath;
                    } else {
                        copiedFilesCount++;
                    }
                    // Перемещаем файл в соответствующую папку
                } else if (settings[1] == 1){
                    QString destinationFilePath = destDir.filePath(fileName);
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
            if(settings[1] == 1) QMessageBox::information(this, "Moving Files", "All files moved successfully!");
            if(settings[0] == 1) QMessageBox::information(this, "Copying Files", "All files copied successfully!");
        } else {
            if(settings[1] == 1) QMessageBox::warning(this, "Moving Files", "Moving operation was canceled or failed!");
            if(settings[0] == 1) QMessageBox::warning(this, "Copying Files", "Copying operation was canceled or failed!");
        }

        qDebug() << "Organizing images completed.";
        delete[] settings;

    }

}

//Жонглирование путями
void MainWindow::on_switch_paths_clicked()
{
    QModelIndex sourcePath = ui->listView_left->rootIndex();
    QModelIndex destDirPath = ui->listView_right->rootIndex();

    ui->listView_left->setRootIndex(destDirPath);
    ui->listView_right->setRootIndex(sourcePath);

    List temp_list = List();

    for(int i = 0; i < left_history.length(); i++){
        temp_list.add_to_start(left_history.get(i));
    }
    left_history.delete_list();
    left_history = List();
    for (int i = 0; i < right_history.length(); i++){
        left_history.add_to_start(right_history.get(i));
    }
    right_history.delete_list();
    right_history = List();
    for (int i = 0; i < temp_list.length(); i++){
        right_history.add_to_start(temp_list.get(i));
    }
    temp_list.delete_list();

    ui->comboBox_Rhistory->clear();
    for(int i = 0; i < right_history.length(); i++){
        ui->comboBox_Rhistory->addItem(right_history.get(i));
    }

    ui->comboBox_Lhistory->clear();
    for(int i = 0; i < left_history.length(); i++){
        ui->comboBox_Lhistory->addItem(left_history.get(i));
    }


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

//Обработка фокуса -1
void MainWindow::on_listView_left_clicked(const QModelIndex &index)
{
    properFocus();
}

//Обработка фокуса -1
void MainWindow::on_listView_right_clicked(const QModelIndex &index)
{
    properFocus();
}

//Вид списка слева
void MainWindow::on_list_view_left_clicked()
{
    viewType = 0;
    ui->listView_left->setUniformItemSizes(false);
    ui->listView_left->setViewMode(QListView::ListMode);
}

void MainWindow::on_icon_view_left_clicked()
{
    viewType = 1;
    ui->listView_left->setUniformItemSizes(true);
    ui->listView_left->setWordWrap(true);
    ui->listView_left->setViewMode(QListView::IconMode);
    refreshIcons();
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
    refreshIcons();
}

//Меню ПКМ
void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{

    // Остальной код обработки контекстного меню
    QItemSelectionModel* selectionModel = getModelFromFocusedListView();

    QMenu menu(this); // Создаем объект контекстного меню

    QAction *open = menu.addAction("Open");

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
    else if(selectedAction == actionSortByName){
        //Сортировка по имени
        //Sample text

    }
    else if(selectedAction == actionSortByDate){
        //Сортировка по дате
        //Sample text

    }
    else if(selectedAction == actionSortBySize){
        //Сортировка по размеру
        //Sample text

    }
    else if(selectedAction == listView){
        //Вид как список
        if (listViewFocus == -1) {
            on_list_view_left_clicked();
            ui->listView_left->setIconSize(QSize(15, 15));
            ui->listView_left->setResizeMode(QListView::Adjust);
        }
        if (listViewFocus == 1) {
            on_list_view_right_clicked();
            ui->listView_right->setIconSize(QSize(15, 15));
            ui->listView_right->setResizeMode(QListView::Adjust);
        }
    }
    else if(selectedAction == iconView_EXL){
        //Вид как очень большие иконки
        if (listViewFocus == -1) {
            on_icon_view_left_clicked();
            ui->listView_left->setIconSize(QSize(70, 70));
            ui->listView_left->setResizeMode(QListView::Adjust);
        }
        if (listViewFocus == 1) {
            on_icon_view_right_clicked();
            ui->listView_right->setIconSize(QSize(70, 70));
            ui->listView_right->setResizeMode(QListView::Adjust);
        }
    }
    else if(selectedAction == iconView_L){
        //Вид как большие иконки
        if (listViewFocus == -1) {
            on_icon_view_left_clicked();
            ui->listView_left->setIconSize(QSize(50, 50));
            ui->listView_left->setResizeMode(QListView::Adjust);
        }
        if (listViewFocus == 1) {
            on_icon_view_right_clicked();
            ui->listView_right->setIconSize(QSize(50, 50));
            ui->listView_right->setResizeMode(QListView::Adjust);
        }

    }
    else if(selectedAction == iconView_M){
        //Вид как средние иконки
        if (listViewFocus == -1) {
            on_icon_view_left_clicked();
            ui->listView_left->setIconSize(QSize(30, 30));
            ui->listView_left->setResizeMode(QListView::Adjust);
        }
        if (listViewFocus == 1) {
            on_icon_view_right_clicked();
            ui->listView_right->setIconSize(QSize(30, 30));
            ui->listView_right->setResizeMode(QListView::Adjust);
        }

    }
    else if(selectedAction == iconView_S){
        //Вид как маленькие иконки
        if (listViewFocus == -1) {
            on_icon_view_left_clicked();
            ui->listView_left->setIconSize(QSize(20, 20));
            ui->listView_left->setResizeMode(QListView::Adjust);
        }
        if (listViewFocus == 1) {
            on_icon_view_right_clicked();
            ui->listView_right->setIconSize(QSize(20, 20));
            ui->listView_right->setResizeMode(QListView::Adjust);
        }

    }
    else if(selectedAction == refresh){
        //Обновление
        if (listViewFocus == -1) ui->listView_left->update();
        if (listViewFocus == 1) ui->listView_right->update();
    }
    else if (selectedAction == newFolder) {
        //Создание новой папки
        new_dir();
    }
    else if (selectedAction == newFile) {
        //Создание нового файла
        new_file();
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
        //Окно свойств
        //Sample text
    }
    else if (selectedAction == editMeta) {
        //Редактор метаданных
        //Sample text

    }
}

//Открытие через вернее меню
void MainWindow::on_actionSD_actions_triggered()
{
    on_sd_actions_left_clicked();
}

bool createDir = true; //Переменная для понимания, что создавать

//Создание новой папки
void MainWindow::new_dir(){
    createDir = true;
    ui->new_name->show();
}

//Создание нового файла
void MainWindow::new_file(){
    createDir = false;
    ui->new_name->show();
}

//Принудительный выход из создания нового файла
void MainWindow::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Escape) {
        ui->new_name->hide();
        ui->new_name->clear();
    }
    QWidget::keyPressEvent(event);
}

//Введение нового имени
void MainWindow::on_new_name_returnPressed()
{
    QString element_name = ui->new_name->text();
    QString parent_path = (listViewFocus == -1) ? (ui->left_path->text()) : (ui->right_path->text());

    if (createDir == true){
        //qDebug() << parent_path;
        QDir dir = parent_path;

        if (QDir(parent_path + "/" + element_name).exists()) {
            QMessageBox::information(nullptr, "Информация", "Папка с таким именем уже существует.");
            return;
        }

        if (!dir.mkdir(element_name)) {
            QMessageBox::critical(nullptr, "Ошибка", "Не удалось создать папку.");
            return;
        }

        ui->new_name->hide();
        ui->new_name->clear();
    }
    else if (createDir == false){
        QString file_path = parent_path + "/" + element_name;
        // Проверка корректности пути к файлу
        if (file_path.isEmpty()) {
            QMessageBox::critical(nullptr, "Ошибка", "Путь к файлу пустой.");
            return;
        }

        // Проверка существования файла
        QFile file = file_path;
        if (file.exists()) {
            QMessageBox::information(nullptr, "Информация", "Файл с таким именем уже существует.");
            return;
        }

        // Создание нового пустого файла
        bool success = file.open(QIODevice::WriteOnly);
        if (!success) {
            QMessageBox::critical(nullptr, "Ошибка", "Не удалось создать файл.");
            return;
        }

        // Закрытие файла
        file.close();

        ui->new_name->hide();
        ui->new_name->clear();
    }
}

//Сжатие изображения для иконок
QImage scaleImage(const QImage &originalImage, const QSize &targetSize) {
    return originalImage.scaled(targetSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

//Функция обновления иконок
void MainWindow::refreshIcons(){
    // Получаем модель выбора из listView'ов
    //QItemSelectionModel* selectionModel = getModelFromFocusedListView();
    QItemSelectionModel* selectionModel = ui->listView_left->selectionModel();
    if (!selectionModel)
        return;

    // Получаем связанную с QListView модель
    QListView* listView = qobject_cast<QListView*>(selectionModel->parent());
    if (!listView)
        return;

    QFileSystemModel* model = qobject_cast<QFileSystemModel*>(listView->model());
    if (!model)
        return;

    // Получаем корневой индекс текущей отображаемой папки
    QModelIndex rootIndex = listView->rootIndex();

    // Получаем список файлов в текущей отображаемой папке
    int rowCount = model->rowCount(rootIndex);
    for (int row = 0; row < rowCount; ++row) {
        QModelIndex index = model->index(row, 0, rootIndex); // Получаем индекс текущего файла в текущей папке
        QFileInfo fileInfo = model->fileInfo(index);
        QImage temp(fileInfo.absoluteFilePath());
        if (fileInfo.isFile()) {
            QSize size(iconHieght, iconWidth);
            QPixmap scaledPixmap = QPixmap::fromImage(temp.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            QIcon newIcon(scaledPixmap);
        }
    }
    ui->listView_left->update();
    ui->listView_right->update();
}

