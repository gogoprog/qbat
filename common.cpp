//
// C++ Implementation: common
//
// Author: Oliver Groß <z.o.gross@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
#include <cstdlib>
#include <cstdio>
#include "common.h"

namespace qbat {
	using namespace std;
	
	bool readStringFromFile(char * buffer, const char * fileName) {
		FILE *f = NULL;
		f = fopen(fileName, "r");
		if (f) {
			fgets(buffer, BUF_SIZE, f);
			fclose(f);
			
			bool fill = false;
			for (int i = 0; i < BUF_SIZE; i++) {
				if (buffer[i] == '\n')
					fill = true;
				if (fill)
					buffer[i] = '\0';
			}
			
			return true;
		}
		else
			return false;
	}
	
	int readIntSysFile(const char * fileName) {
		char buffer[BUF_SIZE];
		readStringFromFile(buffer, fileName);
		return atoi(buffer);
	}
	
	int toStatusInt(const char * status) {
		if (strcmp(status, "Discharging") == 0)
			return UI_BATTERY_DISCHARGING;
		else if (strcmp(status, "Charging") == 0)
			return UI_BATTERY_CHARGING;
		else if (strcmp(status, "Full") == 0)
			return UI_BATTERY_FULL;
		else
			return 0;
	}
}
