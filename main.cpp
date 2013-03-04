#include <QtGui>
#include <QTranslator>
#include "powermanager.h"
#include "constants.h"

using namespace qbat;

int main(int argc, char * argv[])
{
	QApplication app(argc, argv);
	
	QString locale = QLocale::system().name();
	QTranslator translator;
	translator.load(QString(UI_PATH_TRANSLATIONS "qbat_") + locale);
	app.installTranslator(&translator);
	app.setQuitOnLastWindowClosed(false);
	
	{
		QDir workdir(UI_PATH_WORK);
		if (!workdir.exists()) {
			workdir.cdUp();
			workdir.mkdir(UI_DIR_WORK);
		}
	}
	
	CPowerManager mainobject;
	return app.exec();
}
