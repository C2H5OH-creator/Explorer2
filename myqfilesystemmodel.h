#ifndef MYQFILESYSTEMMODEL_H
#define MYQFILESYSTEMMODEL_H

#include <QFileSystemModel>

class MyQFileSystemModel : public QFileSystemModel
{
    Q_OBJECT

public:
    MyQFileSystemModel(QObject *parent = nullptr, int iconWidth = 100, int iconHeight = 100);

    QVariant data(const QModelIndex &index, int role) const override;

private:
    int m_iconWidth;
    int m_iconHeight;
};

#endif // MYQFILESYSTEMMODEL_H
