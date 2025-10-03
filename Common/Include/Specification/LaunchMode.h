#ifndef LAUNCHMODE_H
#define LAUNCHMODE_H

#include <QStringList>


enum class LaunchMode {
    MainApp,
    UserManager,
    SettingManager,
    Demo,
	Developer,
    Unknown
};


inline LaunchMode ParseLaunchMode(const QStringList& args) {
    if (args.contains("usermgr", Qt::CaseInsensitive)) return LaunchMode::UserManager;
    if (args.contains("settingmgr", Qt::CaseInsensitive)) return LaunchMode::SettingManager;
    if (args.contains("demo", Qt::CaseInsensitive)) return LaunchMode::Demo;
	if (args.contains("developer", Qt::CaseInsensitive)) return LaunchMode::Developer;

    return LaunchMode::MainApp;
}
#endif // LAUNCHMODE_H
