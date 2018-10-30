#ifndef NOISEEXCEPTION_H
#define NOISEEXCEPTION_H

#include <QException>
#include <QString>


class NoiseException : public QException
{
public:
	enum Error {EXPT_DSTAR_NOT_FOUND};
	
	NoiseException(Error ec, QString em) : errorCode(ec), errorMessage(em) { }
	NoiseException()  throw() { }
	~NoiseException()  throw() { }
	
	NoiseException* clone() { return new NoiseException(*this); }
	void raise() { throw *this; }
	
	QString getErrorMessage() { return errorMessage; }
	int getErrorCode() { return errorCode; }
	
private:
	Error errorCode;
	QString errorMessage;
};

#endif // NOISEEXCEPTION_H
