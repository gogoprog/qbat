//
// C++ Interface: qtimermessagebox
//
// Author: Oliver Groß <z.o.gross@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
#ifndef QTIMERMESSAGEBOX_H
#define QTIMERMESSAGEBOX_H

#include <QMessageBox>

/**
	@author Oliver Groß <z.o.gross@gmx.de>
*/
class QTimerMessageBox : public QMessageBox {
	Q_OBJECT
private:
	int m_TimerID;
	int m_Timeout;
	
	QString m_DefaultButtonText;
	
	void timerEvent(QTimerEvent * event);
	inline void markDefaultButton();
	
	inline static QTimerMessageBox::StandardButton messagebox(QWidget * parent, Icon icon, const QString & title, const QString & text, int timeout, StandardButtons buttons, StandardButton defaultButton);
public:
	QTimerMessageBox(QWidget * parent = 0);
	QTimerMessageBox(Icon icon, const QString & title, const QString & text, StandardButtons buttons = NoButton, QWidget * parent = 0, Qt::WindowFlags f = Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
	~QTimerMessageBox();
	
	int timeout() const { return m_Timeout; }
	inline void setTimeout(int timeout);
	
	static QMessageBox::StandardButton critical(QWidget * parent, const QString & title, const QString & text, int timeout = 10, StandardButtons buttons = Ok, StandardButton defaultButton = Ok);
	static QMessageBox::StandardButton information(QWidget * parent, const QString & title, const QString & text, int timeout = 10, StandardButtons buttons = Ok, StandardButton defaultButton = Ok);
	static QMessageBox::StandardButton question(QWidget * parent, const QString & title, const QString & text, int timeout = 10, StandardButtons buttons = Ok, StandardButton defaultButton = Ok);
	static QMessageBox::StandardButton warning(QWidget * parent, const QString & title, const QString & text, int timeout = 10, StandardButtons buttons = Ok, StandardButton defaultButton = Ok);
public slots:
	int exec(int timeout);
signals:
	void aboutToTriggerTimeout();
};

#endif
