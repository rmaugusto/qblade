#include "FASTResult.h"


FASTResult::FASTResult () {
	
}

FASTResult::FASTResult (QString name, QString unit, int rows) {
	defaultName = name;
	siUnit = unit;
	numberOfRows = rows;
	data = new float[rows];
}

FASTResult::~FASTResult() {
	delete [] data;
}
