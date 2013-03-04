//
// C++ Implementation: powermanager
//
// Author: Oliver Groß <z.o.gross@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
#include <QtGui>
#include "powermanager.h"
#include "common.h"
#include "settings.h"
#include "batteryicon.h"
#include "qtimermessagebox.h"

namespace qbat {
	using namespace std;
	
	CPowerManager::CPowerManager(QObject * parent) :
		QObject(parent),
		m_Timer(-1),
		m_DataTimer(-1),
// 		m_RelativeCapacity(100),
		m_ACPlug(false),
		m_CriticalHandled(false),
		m_SettingsFile(UI_FILE_CONFIG, QSettings::IniFormat, this)
	{
		readSettings();
		m_ContextMenu.addAction(tr("&Settings"), this, SLOT(showSettings()))->setEnabled(CBatteryIcon::sysfsDir.exists());
		m_ContextMenu.addAction(tr("&About"), this, SLOT(showAbout()));
		m_ContextMenu.addSeparator();
		m_ContextMenu.addAction(tr("&Quit"), qApp, SLOT(quit()));
		
		m_DefaultIcon = new CBatteryIcon(&m_Settings, tr("merged"), this);
		m_DefaultIcon->setContextMenu(&m_ContextMenu);
		
		if (CBatteryIcon::sysfsDir.exists()) {
			m_DataTimer = startTimer(15000);
			m_Timer = startTimer(m_Settings.pollingRate);
		}
		updateSupplies();
	}
	
	CPowerManager::~CPowerManager() {
		killTimer(m_Timer);
		killTimer(m_DataTimer);
	}
	
	inline void CPowerManager::readSettings() {
		m_SettingsFile.beginGroup("Main");
		m_Settings.pollingRate = m_SettingsFile.value("pollingRate", 3000).toUInt();
		m_Settings.showBalloon = m_SettingsFile.value("showBalloon", false).toBool();
		m_SettingsFile.endGroup();
		
		m_SettingsFile.beginGroup("TrayIcons");
		m_Settings.mergeBatteries = m_SettingsFile.value("mergeBatteries", false).toBool();
		m_Settings.colors[UI_COLOR_PEN_CHARGE] = m_SettingsFile.value("textChargingColor", 0).toUInt();
		m_Settings.colors[UI_COLOR_PEN_DISCHARGE] = m_SettingsFile.value("textDischargingColor", 0).toUInt();
		m_Settings.colors[UI_COLOR_PEN_FULL] = m_SettingsFile.value("textFullColor", 0).toUInt();
		m_Settings.colors[UI_COLOR_BRUSH_CHARGED] = m_SettingsFile.value("mainChargedColor", qRgb( 0, 255, 0)).toUInt();
		m_Settings.colors[UI_COLOR_BRUSH_EMPTY] = m_SettingsFile.value("mainEmptyColor", qRgb(255, 255, 255)).toUInt();
		m_Settings.colors[UI_COLOR_BRUSH_FULL] = m_SettingsFile.value("mainFullColor", m_Settings.colors[UI_COLOR_BRUSH_CHARGED]).toUInt();
		m_Settings.colors[UI_COLOR_BRUSH_POLE_CHARGE] = m_SettingsFile.value("poleChargingColor", qRgb(255, 255, 0)).toUInt();
		m_Settings.colors[UI_COLOR_BRUSH_POLE_DISCHARGE] = m_SettingsFile.value("poleDischargingColor", qRgb(255, 255, 0)).toUInt();
		m_Settings.colors[UI_COLOR_BRUSH_POLE_FULL] = m_SettingsFile.value("poleFullColor", m_Settings.colors[UI_COLOR_BRUSH_POLE_CHARGE]).toUInt();
		m_SettingsFile.endGroup();
		
		m_SettingsFile.beginGroup("CriticalEvent");
		m_Settings.handleCritical = m_SettingsFile.value("handle", false).toBool();
		m_Settings.criticalCapacity = m_SettingsFile.value("criticalCapacity", 7).toUInt();
		m_Settings.executeCommand = m_SettingsFile.value("executeCommand", false).toBool();
		m_Settings.criticalCommand = m_SettingsFile.value("command", "").toString();
		m_Settings.confirmCommand = m_SettingsFile.value("confirmCommand", false).toBool();
		m_Settings.confirmWithTimeout = m_SettingsFile.value("confirmWithTimeout", false).toBool();
		m_Settings.timeoutValue = m_SettingsFile.value("timeoutValue", 5).toUInt();
		m_SettingsFile.endGroup();
	}
	
	inline void CPowerManager::writeSettings() {
		m_SettingsFile.beginGroup("Main");
		m_SettingsFile.setValue("pollingRate", m_Settings.pollingRate);
		m_SettingsFile.setValue("showBalloon", m_Settings.showBalloon);
		m_SettingsFile.endGroup();
		
		m_SettingsFile.beginGroup("TrayIcons");
		m_SettingsFile.setValue("mergeBatteries", m_Settings.mergeBatteries);
		m_SettingsFile.setValue("textChargingColor", m_Settings.colors[UI_COLOR_PEN_CHARGE]);
		m_SettingsFile.setValue("textDischargingColor", m_Settings.colors[UI_COLOR_PEN_DISCHARGE]);
		m_SettingsFile.setValue("textFullColor", m_Settings.colors[UI_COLOR_PEN_FULL]);
		m_SettingsFile.setValue("mainChargedColor", m_Settings.colors[UI_COLOR_BRUSH_CHARGED]);
		m_SettingsFile.setValue("mainEmptyColor", m_Settings.colors[UI_COLOR_BRUSH_EMPTY]);
		m_SettingsFile.setValue("mainFullColor", m_Settings.colors[UI_COLOR_BRUSH_FULL]);
		m_SettingsFile.setValue("poleChargingColor", m_Settings.colors[UI_COLOR_BRUSH_POLE_CHARGE]);
		m_SettingsFile.setValue("poleDischargingColor", m_Settings.colors[UI_COLOR_BRUSH_POLE_DISCHARGE]);
		m_SettingsFile.setValue("poleFullColor", m_Settings.colors[UI_COLOR_BRUSH_POLE_FULL]);
		m_SettingsFile.endGroup();
		
		m_SettingsFile.beginGroup("CriticalEvent");
		m_SettingsFile.setValue("handle", m_Settings.handleCritical);
		m_SettingsFile.setValue("criticalCapacity", m_Settings.criticalCapacity);
		m_SettingsFile.setValue("executeCommand", m_Settings.executeCommand);
		m_SettingsFile.setValue("command", m_Settings.criticalCommand);
		m_SettingsFile.setValue("confirmCommand", m_Settings.confirmCommand);
		m_SettingsFile.setValue("confirmWithTimeout", m_Settings.confirmWithTimeout);
		m_SettingsFile.setValue("timeoutValue", m_Settings.timeoutValue);
		m_SettingsFile.endGroup();
	}
	
	void CPowerManager::timerEvent(QTimerEvent * event) {
		if (event->timerId() == m_Timer)
			updateSupplies();
		else if (event->timerId() == m_DataTimer)
			updateBatteryData();
	}
	
	void CPowerManager::updateSupplies() {
		if (CBatteryIcon::sysfsDir.exists()) {
			bool oldAC = m_ACPlug;
			m_ACPlug = false;
			
			QStringList powerSupplies = CBatteryIcon::sysfsDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
			
			QList<CBatteryIcon *> newBatteryIcons;
			CBatteryIcon * currentBatteryIcon;
			
			foreach(QString i, powerSupplies) {
				// this is possibly not very reliable
				if (CBatteryIcon::sysfsDir.exists(i + "/online")) {
					if (readIntSysFile(CBatteryIcon::sysfsDir.filePath(i + "/online").toAscii().constData()) == 1) {
						m_ACPlug = true;
						m_CriticalHandled = false;
					}
				}
				else {
					if (m_BatteryIcons.contains(i)) {
						currentBatteryIcon = m_BatteryIcons.take(i);
					}
					else {
// 						qDebug("new battery: %s", i.toAscii().constData());
						currentBatteryIcon = new CBatteryIcon(&m_Settings, i, this);
						currentBatteryIcon->setContextMenu(&m_ContextMenu);
						currentBatteryIcon->updateData();
						currentBatteryIcon->updateIcon();
						currentBatteryIcon->updateToolTip();
						
						if (!m_Settings.mergeBatteries)
							currentBatteryIcon->show();
						
						m_CriticalHandled = false;
					}
					newBatteryIcons << currentBatteryIcon;
				}
			}
			
			foreach(CBatteryIcon * i, m_BatteryIcons)
				delete i;
			
			m_BatteryIcons.clear();
			
			foreach(CBatteryIcon * i, newBatteryIcons)
				m_BatteryIcons.insert(i->data().name, i);
			
			if (oldAC != m_ACPlug)
				updateBatteryData();
			else
				updateMergedData();
			
			if (m_BatteryIcons.isEmpty()) {
				if (!m_DefaultIcon->isVisible()) {
					m_DefaultIcon->setVisible(true);
					m_DefaultIcon->setIcon(QIcon(UI_ICON_QBAT_SMALL));
					
					if (m_ACPlug)
						m_DefaultIcon->setMessage(tr("AC adapter plugged in"));
					else
						m_DefaultIcon->setMessage(tr("AC adapter unplugged"));
				}
			}
			else {
				if (!m_DefaultIcon->isVisible()) {
					m_DefaultIcon->updateData();
					m_DefaultIcon->updateIcon();
				}
				m_DefaultIcon->setVisible(m_Settings.mergeBatteries);
			}
			
			checkCritical();
		}
		else {
			m_DefaultIcon->setMessage(tr("no information available"));
			m_DefaultIcon->setVisible(true);
		}
		
	}
	
	void CPowerManager::updateMergedData() {
		if (!m_BatteryIcons.isEmpty()) {
			int fullCount = 0;
			int rateCount = 0;
			int voltageCount = 0;
			
			int fullCapacity    = 0;
			int currentCapacity = 0;
			int rate            = 0;
			int voltage         = 0;
			int status          = 0;
			
			foreach (CBatteryIcon * battery, m_BatteryIcons) {
				if (battery->data().voltage) {
					voltage += battery->data().voltage;
					voltageCount++;
				}
				
				if (!battery->data().energyUnits) {
					currentCapacity += battery->data().currentCapacity;
					
					if (battery->data().fullCapacity) {
						fullCapacity += battery->data().fullCapacity;
						fullCount++;
					}
					
					if (battery->data().rate) {
						rate += battery->data().rate;
						rateCount++;
					}
				}
				else if (battery->data().voltage) {
					double voltageNorm  = battery->data().voltage / 100.0;
					currentCapacity += qRound(battery->data().currentCapacity / voltageNorm);
					
					if (battery->data().fullCapacity) {
						fullCapacity += qRound(battery->data().fullCapacity / voltageNorm);
						fullCount++;
					}
					
					if (battery->data().rate) {
						rate += qRound(battery->data().rate / voltageNorm);
						rateCount++;
					}
				}
				
				if ((!status) || (battery->data().status != UI_BATTERY_FULL))
					status = battery->data().status;
			}
			
			if (fullCount)
				fullCapacity /= fullCount;
			
			if (rateCount)
				rate /= rateCount;
			
			if (voltageCount)
				voltage /= voltageCount;
			
			currentCapacity /= m_BatteryIcons.count();
			
			m_DefaultIcon->updateData(currentCapacity, fullCapacity, 0, rate, voltage, status, false);
		}
	}
	
	void CPowerManager::checkCritical() {
		if (m_ACPlug || m_CriticalHandled ||
			m_DefaultIcon->data().relativeCharge == -1 || !m_Settings.handleCritical ||
			m_DefaultIcon->data().relativeCharge > m_Settings.criticalCapacity)
			return;
		
		QString msgTitle = (m_Settings.executeCommand && m_Settings.confirmWithTimeout) ?
			tr("QBat - critical battery capacity (will automatically choose ok on timeout)"):
			tr("QBat - critical battery capacity");
		QString msgText = (m_Settings.executeCommand && m_Settings.confirmCommand) ?
			tr("WARNING: The attached battery(s) reached the critical mark.\n"
			"Click cancel and please make sure to save and shut down soon or provide another source of power\n"
			"or:\n"
			"Click ok to execute:\n%1").arg(m_Settings.criticalCommand) :
			tr("WARNING: The attached battery(s) reached the critical mark.\n"
			"Please make sure to save and shut down soon or provide another source of power.");
		if (m_Settings.executeCommand && (!m_Settings.confirmCommand || (QTimerMessageBox::warning(NULL, msgTitle, msgText,
			(m_Settings.confirmWithTimeout) ? m_Settings.timeoutValue : -1, QMessageBox::Ok | QMessageBox::Cancel))))
			QProcess::startDetached(m_Settings.criticalCommand);
		else {
			if (m_Settings.showBalloon)
				m_BatteryIcons.begin().value()->showMessage(msgTitle, msgText, QSystemTrayIcon::Warning, 7000);
			else
				QMessageBox::warning(NULL, msgTitle, msgText);
		}
		m_CriticalHandled = true;
	}
	
	void CPowerManager::updateBatteryData() {
		foreach (CBatteryIcon * battery, m_BatteryIcons)
			battery->updateData();
		
		updateMergedData();
		checkCritical();
	}
	
	void CPowerManager::restartTimer() {
		killTimer(m_Timer);
		m_Timer = startTimer(m_Settings.pollingRate);
		killTimer(m_DataTimer);
		m_DataTimer = startTimer(15000);
	}
	
	void CPowerManager::showSettings() {
		CSettings dialog;
		
		if (dialog.execute(&m_Settings)) {
			foreach (CBatteryIcon * i, m_BatteryIcons) {
				delete i;
			}
			
			m_BatteryIcons.clear();
			m_DefaultIcon->updateData(0, 0, 0, 0, 0, 0, false);
			
			updateSupplies();
			restartTimer();
			writeSettings();
		}
	}
	
	void CPowerManager::showAbout() {
		if (m_Settings.showBalloon) {
			if (m_DefaultIcon->isVisible())
				m_DefaultIcon->showMessage(tr("About QBat"), UI_NAME + "\nv" + QString(UI_VERSION),
					QSystemTrayIcon::Information, 4000);
			else
				m_BatteryIcons.begin().value()->showMessage(tr("About QBat"), UI_NAME + "\nv" + QString(UI_VERSION),
					QSystemTrayIcon::Information, 4000);
		}
		else {
			QMessageBox aboutBox;
			
			aboutBox.setWindowIcon(QIcon(UI_ICON_QBAT_SMALL));
			aboutBox.setIconPixmap(QPixmap(UI_ICON_QBAT_LARGE));
			aboutBox.setWindowTitle(tr("About QBat"));
			aboutBox.setText(UI_NAME + "\nv" + QString(UI_VERSION));
			aboutBox.setStandardButtons(QMessageBox::Ok);
			
			aboutBox.exec();
		}
	}
}
