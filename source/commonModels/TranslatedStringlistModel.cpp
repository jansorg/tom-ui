#include <utility>

#include "TranslatedStringlistModel.h"

TranslatedStringlistModel::TranslatedStringlistModel(const QStringList &data, QStringList translations, QObject *parent) : QStringListModel(data, parent),
                                                                                                                           _translations(std::move(translations)) {
}

QVariant TranslatedStringlistModel::data(const QModelIndex &index, int role) const {
    if (index.isValid() && role == Qt::DisplayRole) {
        return _translations.at(index.row());
    }
    return QStringListModel::data(index, role);
}
