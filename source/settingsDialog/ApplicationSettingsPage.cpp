#include "ApplicationSettingsPage.h"

ApplicationSettingsPage::ApplicationSettingsPage(QWidget *parent) : QWidget(parent), Ui::ApplicationSettingsPage() {
    setupUi(this);
}

void ApplicationSettingsPage::setup(TomSettings *settings) {
    mainWindowStartup->setChecked(settings->openMainWindowAtStartup());

    connect(mainWindowStartup, &QCheckBox::toggled,
            settings, &TomSettings::setOpenMainWindowAtStartup);
}
