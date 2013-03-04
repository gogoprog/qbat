//
// C++ Interface: common
//
// Author: Oliver Groß <z.o.gross@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
#ifndef QBAT_COMMON_H
#define QBAT_COMMON_H

#include <QString>
#include <QColor>
#include "constants.h"

#define BUF_SIZE 256

namespace qbat {
	struct Settings {
		quint16 pollingRate;
		bool handleCritical;
		quint8 criticalCapacity;
		bool executeCommand;
		QString criticalCommand;
		bool confirmCommand;
		bool confirmWithTimeout;
		quint16 timeoutValue;
		QRgb colors[UI_COUNT_COLORS];
		bool showBalloon;
		bool mergeBatteries;
	};
	
	bool readStringFromFile(char * buffer, const char * fileName);
	int readIntSysFile(const char * fileName);
	int toStatusInt(const char * status);
	
	inline qint8 calcRelativeDef(int value, int border, qint8 defValue = -1) {
		return border ? (qint8)(100.0 * value / border) : defValue;
	}
}

#endif
