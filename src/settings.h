//
// C++ Interface: settings
//
// Author: Oliver Groß <z.o.gross@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
#ifndef QBAT_SETTINGS_H
#define QBAT_SETTINGS_H

#include <QDialog>
#include "ui_settingsdialog.h"
#include "common.h"

namespace qbat {
	class CSettings : public QDialog {
		Q_OBJECT
	private:
		Ui::settingsDialog ui;
		QButtonGroup colorSelectButtons;
		Settings * m_Settings;
		
		QRgb colors[UI_COUNT_COLORS];
		
		void applySettings();
	public:
		CSettings(QWidget * parent = 0);
		~CSettings();
		
		bool execute(Settings * settings);
	private slots:
		void editColor(int id);
	};
}

#endif
