#ifndef GOTIME_UI_FRAMETABLEVIEWMODEL_H
#define GOTIME_UI_FRAMETABLEVIEWMODEL_H

#include <QtCore/QAbstractTableModel>
#include <QIcon>

#include <gotime/TomControl.h>

#include "data/Frame.h"

static const QString &FRAMES_MIME_TYPE = "application/x-tom-frames";

class FrameTableViewModel : public QAbstractTableModel {
Q_OBJECT
public:
    explicit FrameTableViewModel(TomControl *control, QObject *parent);

    ~FrameTableViewModel() override;

    void loadFrames(const Project &project);

    bool setData(const QModelIndex &index, const QVariant &value, int role) override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    Frame *frameAt(const QModelIndex &index) const;

    bool removeRows(int row, int count, const QModelIndex &parent) override;

    Qt::DropActions supportedDragActions() const override;

    QStringList mimeTypes() const override;

    QMimeData *mimeData(const QModelIndexList &indexes) const override;

    int findRow(const QString &frameID);

    static const int COL_ARCHIVED = 0;
    static const int COL_START_DATE = 1;
    static const int COL_START = 2;
    static const int COL_END = 3;
    static const int COL_DURATION = 4;
    static const int COL_TAGS = 5;
    static const int COL_SUBPROJECT = 6;
    static const int COL_LAST_UPDATED = 7;
    static const int COL_NOTES = 8;

    static const int FIRST_COL = 0;
    static const int LAST_COL = COL_NOTES;
    static const int COLUMN_COUNT = LAST_COL + 1;

public slots:

    void setShowArchived(bool showArchived);

signals:

    void subprojectStatusChange(bool available);

private slots:

    void onFramesUpdates(const QStringList &frameIDs, const QStringList &projectIDs);

    void onFramesRemoved(const QStringList &frameIDs, const QStringList &projectIDs);

    void onFramesMoved(const QStringList &frameIDs, const QStringList &oldProjectIDs, const QString &newProjectID);

    void onFramesArchived(const QStringList& frameIDs, const QStringList &projectIDs, bool nowArchived);

    void onProjectFramesArchived(const QStringList &projectIDs);

    void onProjectUpdated(const Project &project);

    void onProjectStatusChanged(const Project &started, const Project &stopped);

    void onUpdateActiveFrames();

    void onProjectHierarchyChange();

private:
    void startTimer();

    void stopTimer();

    int rowCount(const QModelIndex &parent) const override;

    int columnCount(const QModelIndex &parent) const override;

    QVariant data(const QModelIndex &index, int role) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    QModelIndex buddy(const QModelIndex &index) const override;

    TomControl *_control;

    QList<Frame *> _frames;
    Project _currentProject;

    bool _showArchived = true;
    QTimer *_frameUpdateTimer;

    QPixmap _archiveIcon;

    void removeFrameRows(const QStringList &ids);

    void updateFrames(const QStringList &ids);
};


#endif //GOTIME_UI_FRAMETABLEVIEWMODEL_H
