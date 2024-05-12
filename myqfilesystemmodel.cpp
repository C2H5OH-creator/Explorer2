#include "myqfilesystemmodel.h"

#include <QFileInfo>
#include <QImageReader>
#include <QPixmap>
#include <QIcon>

MyQFileSystemModel::MyQFileSystemModel(QObject *parent, int iconWidth, int iconHeight)
    : QFileSystemModel(parent), m_iconWidth(iconWidth), m_iconHeight(iconHeight)
{
}
int coefficient = 200;

QVariant MyQFileSystemModel::data(const QModelIndex &index, int role) const
{

    // Проверяем роль и тип элемента
    if (role == Qt::DecorationRole && index.isValid()) {
        QString filePath = this->filePath(index);
        QFileInfo fileInfo(filePath);

        // Проверяем, является ли файл изображением
        if (fileInfo.isFile() && QImageReader::supportedImageFormats().contains(fileInfo.suffix().toLower().toUtf8())) {
            // Загружаем изображение
            QImage image(filePath);

            // Если изображение не пустое, создаем миниатюру
            if (!image.isNull()) {
                QSize iconSize(m_iconHeight + coefficient, m_iconWidth + coefficient); // Размер миниатюры
                //QPixmap thumbnail = QPixmap::fromImage(image.scaled(iconSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                QPixmap thumbnail = QPixmap::fromImage(image.scaled(iconSize, Qt::KeepAspectRatio, Qt::FastTransformation));
                QIcon icon(thumbnail);
                return icon;
            }
        }
    }

    // Если это не изображение или что-то пошло не так, используем стандартное поведение
    return QFileSystemModel::data(index, role);
}
