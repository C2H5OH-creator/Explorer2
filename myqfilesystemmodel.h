#ifndef MYQFILESYSTEMMODEL_H
#define MYQFILESYSTEMMODEL_H

#include <QFileSystemModel>
#include <QMap>

struct ThumbnailInfo {
    QIcon icon;
    QString filePath;
};

class MyQFileSystemModel : public QFileSystemModel
{
    Q_OBJECT

public:
    MyQFileSystemModel(QObject *parent = nullptr);
    QVariant data(const QModelIndex &index, int role) const override;
    QMimeData *mimeData(const QModelIndexList &indexes) const override;


public slots:
     void receiveToModel(int *arr);

private:
     QMap<QString, ThumbnailInfo> thumbnailCache;
     mutable QMap<QString, ThumbnailInfo> *mutableThumbnailCache = &thumbnailCache;

};

#endif // MYQFILESYSTEMMODEL_H
