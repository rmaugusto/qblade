#ifndef FASTRESULT_H
#define FASTRESULT_H

#include <QString>


class FASTResult
{
public:
	FASTResult ();
	FASTResult (QString name, QString unit, int rows);
	~FASTResult ();
	
	QString defaultName;
	QString siUnit;
	int numberOfRows;
	float *data;
};

#endif // FASTRESULT_H
