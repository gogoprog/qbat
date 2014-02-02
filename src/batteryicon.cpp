//
// C++ Implementation: batteryicon
//
// Author: Oliver Groß <z.o.gross@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
#include "batteryicon.h"
#include "common.h"
#include <QPainter>
#include <QMessageBox>

namespace qbat {
	QDir CBatteryIcon::sysfsDir(UI_PATH_SYSFS_DIR);
	
	CBatteryIcon::CBatteryIcon(Settings * settings, QString batteryName, QObject * parent) :
		QSystemTrayIcon(parent),
		m_Icon(28, 28),
		m_Settings(settings)
	{
		m_Data.name = batteryName;
		
		updateIcon();
		
		connect(this, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
			this, SLOT(handleClicks(QSystemTrayIcon::ActivationReason)));
	}
	
	CBatteryIcon::~CBatteryIcon() {
	}
	
	void CBatteryIcon::updateIcon() {
		m_Icon.fill(Qt::transparent);
		QPainter painter(&m_Icon);
		
		if (m_Data.currentCapacity != m_Data.fullCapacity || m_Data.status != UI_BATTERY_FULL) {
			if (m_Data.status == UI_BATTERY_DISCHARGING)
				painter.setPen(QColor(m_Settings->colors[UI_COLOR_PEN_DISCHARGE]));
			else
				painter.setPen(QColor(m_Settings->colors[UI_COLOR_PEN_CHARGE]));
			
			painter.setBrush(QColor(m_Settings->colors[UI_COLOR_BRUSH_EMPTY]));
			painter.drawRect(0, 4, 27, 23);
			
			int chargedPixels = (int)(22 * m_Data.relativeCharge / 100.0);
			
			painter.fillRect(1, 5 + 22 - chargedPixels, 26, chargedPixels, QColor(m_Settings->colors[UI_COLOR_BRUSH_CHARGED]));
			
			if (m_Data.status == UI_BATTERY_DISCHARGING)
				painter.setBrush(QColor(m_Settings->colors[UI_COLOR_BRUSH_POLE_DISCHARGE]));
			else
				painter.setBrush(QColor(m_Settings->colors[UI_COLOR_BRUSH_POLE_CHARGE]));
		}
		else {
			painter.setPen(QColor(m_Settings->colors[UI_COLOR_PEN_FULL]));
			painter.setBrush(QColor(m_Settings->colors[UI_COLOR_BRUSH_FULL]));
			painter.drawRect(0, 4, 27, 23);
			
			painter.setBrush(QColor(m_Settings->colors[UI_COLOR_BRUSH_POLE_FULL]));
		}
		painter.drawRect(7, 0, 13, 4);
		
		painter.setBrush(Qt::NoBrush);
		
		if (m_Data.relativeCharge < 100)
			((QFont&)painter.font()).setPixelSize(15);
		else
			((QFont&)painter.font()).setPixelSize(12);
		
		painter.setRenderHint(QPainter::TextAntialiasing);
		((QFont&)painter.font()).setBold(true);
		if (m_Data.relativeCharge == -1)
			painter.drawText(1, 9, 26, 16, Qt::AlignHCenter, QString('?'));
		else
			painter.drawText(1, 9, 26, 16, Qt::AlignHCenter, QString::number(m_Data.relativeCharge));
		
		setIcon(m_Icon);
	}
	
	void CBatteryIcon::updateToolTip() {
		QString newToolTip = tr("%1: %2%").arg(m_Data.name) +'\n';
		
		if (m_Data.relativeCharge == -1)
			newToolTip = newToolTip.arg('-');
		else
			newToolTip = newToolTip.arg(m_Data.relativeCharge);
		
		switch (m_Data.status) {
		case UI_BATTERY_DISCHARGING:
			newToolTip += tr("status: %1").arg(tr("discharging"));
			if (m_Data.rate) {
				newToolTip += '\n';
				qreal remainingTime  = (qreal)(m_Data.currentCapacity) / m_Data.rate;
				int remainingHours   = (int)remainingTime;
				int remainungMinutes = (int)(remainingTime * 60) % 60;
				newToolTip += tr("remaining time: %1:%2").arg(remainingHours, 2, 10, QChar('0')).arg(remainungMinutes, 2, 10, QChar('0'));
			}
			break;
		case UI_BATTERY_CHARGING:
			newToolTip += tr("status: %1").arg(tr("charging"));
			if (m_Data.rate && m_Data.fullCapacity) {
				newToolTip += '\n';
				qreal remainingTime  = (qreal)(m_Data.fullCapacity - m_Data.currentCapacity) / m_Data.rate;
				int remainingHours   = (int)remainingTime;
				int remainungMinutes = (int)(remainingTime * 60) % 60;
				newToolTip += tr("remaining time: %1:%2").arg(remainingHours, 2, 10, QChar('0')).arg(remainungMinutes, 2, 10, QChar('0'));
			}
			break;
		case UI_BATTERY_FULL:
			newToolTip += tr("status: %1").arg(tr("full"));
			break;
		default:
			newToolTip += tr("status: %1").arg(tr("unknown"));
			break;
		}
		newToolTip += '\n';
		
		if (m_Data.energyUnits) {
			if ((m_Data.rate) && (m_Data.status != UI_BATTERY_FULL)) {
				double rateW = qRound(m_Data.rate / 100000.0) / 10.0;
				if (m_Data.voltage) {
					double rateA = qRound((m_Data.rate / m_Data.voltage) / 1000.0) / 10.0;
					newToolTip += tr("current rate: %1W / %2A").arg(rateW).arg(rateA) + '\n';
				}
				else
					newToolTip += tr("current rate: %1W").arg(rateW) + '\n';
			}
			
			newToolTip += tr("current capacity: %1mWh").arg(m_Data.currentCapacity / 1000);
			
			if (m_Data.fullCapacity)
				newToolTip += '\n' + tr("last full capacity: %1mWh").arg(m_Data.fullCapacity / 1000);
			
			if (m_Data.designCapacity)
				newToolTip += '\n' + tr("design capacity: %1mWh").arg(m_Data.designCapacity / 1000);
		}
		else
		{
			if ((m_Data.rate) && (m_Data.status != UI_BATTERY_FULL)) {
				double rateA = m_Data.rate / 100000.0;
				if (m_Data.voltage) {
					double rateW = qRound(rateA * m_Data.voltage / 100.0) / 10.0;
					newToolTip += tr("current rate: %1W / %2A").arg(rateW).arg(qRound(rateA) / 10.0) + '\n';
				}
				else
					newToolTip += tr("current rate: %1A").arg(qRound(rateA) / 10.0) + '\n';
			}
			
			newToolTip += tr("current capacity: %1mAh").arg(m_Data.currentCapacity / 1000);
			
			if (m_Data.fullCapacity)
				newToolTip += '\n' + tr("last full capacity: %1mAh").arg(m_Data.fullCapacity / 1000);
			
			if (m_Data.designCapacity)
				newToolTip += '\n' + tr("design capacity: %1mAh").arg(m_Data.designCapacity / 1000);
		}
		
		if (m_Data.designCapacity && m_Data.fullCapacity < m_Data.designCapacity)
				newToolTip += '\n' + tr("capacity loss: %1%").arg((int)((1 - (double)m_Data.fullCapacity / m_Data.designCapacity) * 100));
		
		setMessage(newToolTip);
	}
	
	void CBatteryIcon::updateData(int currentCapacity, int fullCapacity, int designCapacity, int rate, int voltage, int status, bool energyUnits) {
		m_Data.energyUnits = energyUnits;
		
		bool noupdate = true;
		
		if (rate != m_Data.rate) {
			noupdate = false;
			m_Data.rate = rate;
		}
		
		if (voltage != m_Data.voltage) {
			noupdate = false;
			m_Data.voltage = voltage;
		}
		
		if (status != m_Data.status) {
			noupdate = false;
			m_Data.status = status;
		}
		
		if (fullCapacity != m_Data.fullCapacity) {
			noupdate = false;
			m_Data.fullCapacity = fullCapacity;
		}
		
		if (designCapacity != m_Data.designCapacity) {
			noupdate = false;
			m_Data.designCapacity = designCapacity;
		}
		
		if (currentCapacity != m_Data.currentCapacity) {
			noupdate = false;
			m_Data.currentCapacity = currentCapacity;
		}
		
		if (noupdate)
			return;
		
		qint8 newRelativeCharge = calcRelativeDef(currentCapacity, fullCapacity);
		
		if (newRelativeCharge != m_Data.relativeCharge) {
			m_Data.relativeCharge = newRelativeCharge;
			
			if (isVisible())
				updateIcon();
		}
		
		if (isVisible())
			updateToolTip();
	}
	
	void CBatteryIcon::updateData() {
		int currentCapacity = 0;
		int fullCapacity = 0;
		int designCapacity = 0;
		int rate = 0;
		int voltage = 0;
		int status = 0;
		bool energyUnits = sysfsDir.exists(m_Data.name + UI_CAPTION_NOW(UI_CAPTION_ENERGY));
		
		rate = readIntSysFile(sysfsDir.filePath(m_Data.name + "/current_now").toAscii().constData());
		voltage = readIntSysFile(sysfsDir.filePath(m_Data.name + UI_CAPTION_NOW(UI_CAPTION_VOLTAGE)).toAscii().constData()) / 10000;
		
		char buffer[BUF_SIZE];
		readStringFromFile(buffer, sysfsDir.filePath(m_Data.name + "/status").toAscii().constData());
		status = toStatusInt(buffer);
		
		if (energyUnits) {
			fullCapacity    = readIntSysFile(sysfsDir.filePath(m_Data.name + UI_CAPTION_FULL(UI_CAPTION_ENERGY)).toAscii().constData());
			designCapacity  = readIntSysFile(sysfsDir.filePath(m_Data.name + UI_CAPTION_DESIGN(UI_CAPTION_ENERGY)).toAscii().constData());
			currentCapacity = readIntSysFile(sysfsDir.filePath(m_Data.name + UI_CAPTION_NOW(UI_CAPTION_ENERGY)).toAscii().constData());
		}
		else {
			fullCapacity    = readIntSysFile(sysfsDir.filePath(m_Data.name + UI_CAPTION_FULL(UI_CAPTION_CHARGE)).toAscii().constData());
			designCapacity  = readIntSysFile(sysfsDir.filePath(m_Data.name + UI_CAPTION_DESIGN(UI_CAPTION_CHARGE)).toAscii().constData());
			currentCapacity = readIntSysFile(sysfsDir.filePath(m_Data.name + UI_CAPTION_NOW(UI_CAPTION_CHARGE)).toAscii().constData());
		}
		
		updateData(currentCapacity, fullCapacity, designCapacity, rate, voltage, status, energyUnits);
	}
	
	void CBatteryIcon::setMessage(QString value) {
		m_Message = value;
		setToolTip(tr("QBat - %1").arg(m_Message));
	}
	
	void CBatteryIcon::handleClicks(QSystemTrayIcon::ActivationReason reason) {
		switch (reason) {
			case Trigger:
				if (supportsMessages())
					showMessage(tr("QBat - Information"), m_Message);
				else
					QMessageBox::information(NULL, tr("QBat - Information"), m_Message);
				break;
			default:
				break;
		}
	}
}
