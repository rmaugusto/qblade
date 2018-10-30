#include "ParameterObject.h"


template <class ParameterGroup>
void ParameterObject<ParameterGroup>::setParameter(Key key, QVariant value) {
	accessParameter(key, value);
}

template <class ParameterGroup>
QVariant ParameterObject<ParameterGroup>::getParameter(Key key) {
	return accessParameter(key);
}


template class ParameterObject<Parameter::Windfield>;
template class ParameterObject<Parameter::TData>;
template class ParameterObject<Parameter::TDMSData>;
template class ParameterObject<Parameter::CDMSData>;
template class ParameterObject<Parameter::NoiseSimulation>;
template class ParameterObject<Parameter::DMSData>;
