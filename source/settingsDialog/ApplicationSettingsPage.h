#ifndef TOM_UI_APPLICATIONSETTINGSPAGE_H
#define TOM_UI_APPLICATIONSETTINGSPAGE_H


#include <source/settings/TomSettings.h>
#include "ui_ApplicationSettingsPage.h"

class ApplicationSettingsPage : public QWidget, private Ui::ApplicationSettingsPage {
    Q_OBJECT
public:
    explicit ApplicationSettingsPage(QWidget* parent = nullptr);

    void setup(TomSettings* settings);
};


#endif //TOM_UI_APPLICATIONSETTINGSPAGE_H
