//
// Created by jansorg on 13.02.19.
//

#ifndef TOM_UI_TRANSLATEDSTRINGLISTMODEL_H
#define TOM_UI_TRANSLATEDSTRINGLISTMODEL_H


#include <QtCore/QStringListModel>

class TranslatedStringlistModel : public QStringListModel {
Q_OBJECT
public:
    TranslatedStringlistModel(const QStringList &data, QStringList translations, QObject *parent);

    QVariant data(const QModelIndex &index, int role) const override;

private:
    QStringList _translations;
};


#endif //TOM_UI_TRANSLATEDSTRINGLISTMODEL_H
