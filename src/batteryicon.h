//
// C++ Interface: batteryicon
//
// Author: Oliver Groß <z.o.gross@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
#ifndef QBAT_BATTERYICON_H
#define QBAT_BATTERYICON_H

#include <QSystemTrayIcon>
#include <QDir>

namespace qbat {
	struct Settings;
	
	struct BatteryData {
		QString name;
		int fullCapacity;
		int designCapacity;
		int currentCapacity;
		int rate;
		int voltage;
		int status;
		bool energyUnits;
		qint8 relativeCharge;
		
		BatteryData() :
			fullCapacity(0),
			designCapacity(0),
			currentCapacity(0),
			rate(0),
			voltage(0),
			status(0),
			energyUnits(false),
			relativeCharge(-1)
		{}
	};
	
	class CBatteryIcon : public QSystemTrayIcon {
		Q_OBJECT
	public:
		static QDir sysfsDir;
		
		CBatteryIcon(Settings * settings, QString batteryName = QString(), QObject * parent = 0);
		~CBatteryIcon();
		
		
		BatteryData data() const { return m_Data; }
		
		void updateData(int currentCapacity, int fullCapacity, int designCapacity, int rate, int voltage, int status, bool energyUnits);
		void updateData();
		
		void updateToolTip();
		void updateIcon();
		
		QString message() const { return m_Message; }
		void setMessage(QString value);
	private:
		QString m_BatteryName;
		QPixmap m_Icon;
		Settings * m_Settings;
		
		QString m_Message;
		
		BatteryData m_Data;
	private slots:
		void handleClicks(QSystemTrayIcon::ActivationReason reason);
	};
}

#endif
