#include "icondelegate.h"
#include <QFileInfo>
#include <QFileSystemModel>

QVariant ImageDelegate::data(const QModelIndex& index, int role) const
{
    if(role == Qt::DecorationRole)
        return QPixmap(":/icons/TAG_Int.png");

    return QFileSystemModel::data(index, role);
}
