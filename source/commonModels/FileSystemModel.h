#ifndef TOM_UI_FILESYSTEMMODEL_H
#define TOM_UI_FILESYSTEMMODEL_H

#include <QtWidgets/QFileSystemModel>

class FileSystemModel : public QFileSystemModel {
Q_OBJECT

public:
    explicit FileSystemModel(QObject *parent = 0);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
};

#endif //TOM_UI_FILESYSTEMMODEL_H
