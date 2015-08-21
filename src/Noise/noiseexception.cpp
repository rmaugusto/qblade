#include "noiseexception.h"

NoiseException::NoiseException(int ec, QString em)
{
    errorCode = ec;
    errorMessage = em;
}

NoiseException::NoiseException() throw()
{

}

NoiseException::~NoiseException() throw()
{

}
QString NoiseException::getErrorMessage() const
{
    return errorMessage;
}
int NoiseException::getErrorCode() const
{
    return errorCode;
}



