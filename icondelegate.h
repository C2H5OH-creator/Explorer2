#ifndef IMAGEDELEGATE_H
#define IMAGEDELEGATE_H

#include <QItemDelegate>

class ImageDelegate : public QItemDelegate {
public:
    QVariant data(const QModelIndex& index, int role) const;
};

#endif // IMAGEDELGATE_H
