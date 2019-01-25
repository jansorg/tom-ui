//
// Created by jansorg on 19.01.19.
//

#ifndef TOM_UI_PROJECTTREEROOTITEM_H
#define TOM_UI_PROJECTTREEROOTITEM_H


#include "ProjectTreeItem.h"

class ProjectTreeRootItem  : public ProjectTreeItem {
public:
    explicit ProjectTreeRootItem(const ProjectStatusManager* statusManager, ProjectTreeItem* parent = nullptr);

    virtual ~ProjectTreeRootItem();

    QVariant data(int column) const override;

    bool setData(int column, const QVariant &value) override;
};

#endif //TOM_UI_PROJECTTREEROOTITEM_H
