#ifndef NOISEEXCEPTION_H
#define NOISEEXCEPTION_H

#include <QException>
#include <QString>

class NoiseException : public QException
{
public:
    NoiseException(int ec, QString em);
    NoiseException() throw();
    ~NoiseException() throw();
    void raise() const { throw *this; }
    NoiseException *clone() const { return new NoiseException(*this); }

    QString getErrorMessage() const;

    int getErrorCode() const;

private:

    int errorCode;
    QString errorMessage;

};

#endif // NOISEEXCEPTION_H
