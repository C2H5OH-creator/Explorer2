#include "myqfilesystemmodel.h"

#include <QFileInfo>
#include <QImageReader>
#include <QPixmap>
#include <QIcon>
#include <QMap>
#include <QMimeData>

MyQFileSystemModel::MyQFileSystemModel(QObject *parent)
    : QFileSystemModel(parent)
{
}

int coefficient = 200;

int count = 0;

int l_viewType = 0; // 0 = List | 1 = Icon

int l_iconHeight = 20;
int l_iconWidth = 20;

void MyQFileSystemModel::receiveToModel(int *arr){

    l_viewType = arr[0];
    l_iconHeight = arr[1];
    l_iconWidth = arr[2];

    qDebug() << count++ << arr[0];
    qDebug() << arr[1] << arr[2];
    delete[] arr;

}

//Поскольку я переопределил data() начал работать Drag'n'drop. Я его выключил
QMimeData *MyQFileSystemModel::mimeData(const QModelIndexList &indexes) const
{
    return new QMimeData;
}

//Переопределение даты
QVariant MyQFileSystemModel::data(const QModelIndex &index, int role) const
{

    //qDebug() << count++ << l_viewType;
    //qDebug() << l_iconHeight << l_iconWidth;

    // Проверяем роль и тип элемента
    if (role == Qt::DecorationRole && index.isValid() && l_viewType == 1) {
        QString filePath = this->filePath(index);

        // Проверяем, есть ли уже миниатюра для этого файла в кэше
        if (thumbnailCache.contains(filePath)) {
            return thumbnailCache.value(filePath).icon;
        }

        QFileInfo fileInfo(filePath);
        // Проверяем, является ли файл изображением
        if (fileInfo.isFile() && QImageReader::supportedImageFormats().contains(fileInfo.suffix().toLower().toUtf8())) {
            // Загружаем изображение
            QImage image(filePath);

            // Если изображение не пустое, создаем миниатюру
            if (!image.isNull()) {
                QSize iconSize(l_iconHeight + coefficient, l_iconWidth + coefficient); // Размер миниатюры
                QPixmap thumbnail = QPixmap::fromImage(image.scaled(iconSize, Qt::KeepAspectRatio, Qt::FastTransformation));
                QIcon icon(thumbnail);

                // Сохраняем миниатюру в кэше
                ThumbnailInfo thumbnailInfo;
                thumbnailInfo.icon = icon;
                thumbnailInfo.filePath = filePath;
                (*mutableThumbnailCache).insert(filePath, thumbnailInfo);
                return icon;
            }
        }
    }
        // Если это не изображение или что-то пошло не так, используем стандартное поведение
        return QFileSystemModel::data(index, role);
}
