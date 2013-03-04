#ifndef QBAT_CONSTANTS_H
#define QBAT_CONSTANTS_H

#define UI_VERSION                      "0.2.2"
#define UI_NAME                         tr("QBat - Qt Battery Monitor")

#define UI_DIR_WORK                     ".qbat/"

#define UI_PATH_SYSFS_DIR               "/sys/class/power_supply"
#define UI_PATH_TRANSLATIONS            "/usr/share/qbat/lang/"
#define UI_PATH_ICONS                   "/usr/share/qbat/icons/"
#define UI_PATH_WORK                    (QDir::toNativeSeparators(QDir::homePath()) + "/" UI_DIR_WORK)

#define UI_FILE_CONFIG                  (UI_PATH_WORK + "qbat.conf")
#define UI_CAPTION_ENERGY               "/energy"
#define UI_CAPTION_CHARGE               "/charge"
#define UI_CAPTION_VOLTAGE              "/voltage"
#define UI_CAPTION_NOW(a)               a + "_now"
#define UI_CAPTION_FULL(a)              a + "_full"
#define UI_CAPTION_DESIGN(a)            UI_CAPTION_FULL(a) + "_design"

#define UI_ICON_QBAT_LARGE              UI_PATH_ICONS "qbat2_large.svg"
#define UI_ICON_QBAT_SMALL              UI_PATH_ICONS "qbat2.svg"

#define UI_BATTERY_CHARGING             1
#define UI_BATTERY_DISCHARGING          2
#define UI_BATTERY_FULL                 3

#define UI_COUNT_COLORS                 9

#define UI_COLOR_PEN_CHARGE             0
#define UI_COLOR_PEN_DISCHARGE          1
#define UI_COLOR_PEN_FULL               2
#define UI_COLOR_BRUSH_CHARGED          3
#define UI_COLOR_BRUSH_EMPTY            4
#define UI_COLOR_BRUSH_FULL             5
#define UI_COLOR_BRUSH_POLE_CHARGE      6
#define UI_COLOR_BRUSH_POLE_DISCHARGE   7
#define UI_COLOR_BRUSH_POLE_FULL        8

#endif
