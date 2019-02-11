#include "projectstatuslabel.h"

ProjectStatusLabel::ProjectStatusLabel(QWidget *parent) : QLabel(parent), _control(nullptr) {
    setText("");
}

void ProjectStatusLabel::setup(TomControl *control) {
    _control = control;
    if (!_control) {
        return;
    }

    updateStatus();

    auto *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, [this] {
        updateStatus();
    });
    timer->start(1000);
}

void ProjectStatusLabel::updateStatus() {
    auto status = _control->cachedStatus();

    if (status.isValid) {
        setText(tr("%1: %2").arg(status.currentProject().getName()).arg(Timespan::of(status.startTime()).format()));
    } else {
        setText(tr("no active project"));
    }
}
