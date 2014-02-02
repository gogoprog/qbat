//
// C++ Implementation: qtimermessagebox
//
// Description: 
//
//
// Author: Oliver Groß <z.o.gross@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "qtimermessagebox.h"
#include <QTimerEvent>
#include <QPushButton>

QTimerMessageBox::QTimerMessageBox(QWidget * parent) :
	QMessageBox(parent), m_TimerID(-1), m_Timeout(0)
{
}

QTimerMessageBox::QTimerMessageBox(Icon icon, const QString & title, const QString & text, StandardButtons buttons, QWidget * parent, Qt::WindowFlags f) :
	QMessageBox(icon, title, text, buttons, parent, f), m_TimerID(-1), m_Timeout(0)
{
}

QTimerMessageBox::~QTimerMessageBox() {
	killTimer(m_TimerID);
}

void QTimerMessageBox::timerEvent(QTimerEvent * event) {
	if (event->timerId() == m_TimerID) {
		if (m_Timeout > 0) {
			markDefaultButton();
			m_Timeout--;
		}
		else {
			killTimer(m_TimerID);
			emit aboutToTriggerTimeout();
			defaultButton()->click();
		}
	}
}

inline void QTimerMessageBox::markDefaultButton() {
	defaultButton()->setText(m_DefaultButtonText + " (" + QString::number(m_Timeout) + ')');
}

inline void QTimerMessageBox::setTimeout(int timeout) {
	m_Timeout = timeout;
}

int QTimerMessageBox::exec(int timeout) {
	if (defaultButton() && timeout > 0) {
		setTimeout(timeout);
		m_DefaultButtonText = defaultButton()->text();
		
		m_TimerID = startTimer(1000);
		
		int result = QMessageBox::exec();
		
		killTimer(m_TimerID);
		return result;
	}
	else
		return QMessageBox::exec();
}

inline QMessageBox::StandardButton QTimerMessageBox::messagebox(QWidget * parent, Icon icon, const QString & title, const QString & text, int timeout, StandardButtons buttons, StandardButton defaultButton) {
	QTimerMessageBox messageBox(icon, title, text, buttons, parent);
	messageBox.setDefaultButton(defaultButton);
	return (QMessageBox::StandardButton)messageBox.exec(timeout);
}

QMessageBox::StandardButton QTimerMessageBox::critical(QWidget * parent, const QString & title, const QString & text, int timeout, StandardButtons buttons, StandardButton defaultButton) {
	return messagebox(parent, QMessageBox::Critical, title, text, timeout, buttons, defaultButton);
}

QMessageBox::StandardButton QTimerMessageBox::information(QWidget * parent, const QString & title, const QString & text, int timeout, StandardButtons buttons, StandardButton defaultButton) {
	return messagebox(parent, QMessageBox::Information, title, text, timeout, buttons, defaultButton);
}

QMessageBox::StandardButton QTimerMessageBox::question(QWidget * parent, const QString & title, const QString & text, int timeout, StandardButtons buttons, StandardButton defaultButton) {
	return messagebox(parent, QMessageBox::Question, title, text, timeout, buttons, defaultButton);
}

QMessageBox::StandardButton QTimerMessageBox::warning(QWidget * parent, const QString & title, const QString & text, int timeout, StandardButtons buttons, StandardButton defaultButton) {
	return messagebox(parent, QMessageBox::Warning, title, text, timeout, buttons, defaultButton);
}
