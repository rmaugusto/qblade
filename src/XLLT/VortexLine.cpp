#include "VortexLine.h"
#include <QtCore>


VortexLine::VortexLine()
{
    isTip = false;
    isHub = false;
    isTrailing = false;
    isShed = false;
    isConcentrated = false;
    oldLength = 0;
    Length = 0;
    age = 0;
    coreSize = 0;
    Gamma = 0.0;
    pT = NULL;
    pL = NULL;
    fromStation = -999;
    fromTimestep = -999;
    m_Lines = NULL;
    m_Nodes = NULL;
}

void VortexLine::Update(double dT){
   double a = 1.25643; // a factor from literature
   double strain;
   age += dT;
   oldLength = Length;
   Length = CVector(*pL-*pT).VAbs();
   if (oldLength == 0) strain = 0;
   else strain = (Length-oldLength / oldLength);
   coreSize = sqrt(4*a*m_TurbulentViscosity*m_KinViscosity*(age+m_VortexTimeOffset)/(1+strain));
//   qDebug() << "coresize" << coreSize << "age" << age << strain << m_TurbulentViscosity;
}

void VortexLine::SetGamma(double gamma){
    Gamma = gamma;
}

double VortexLine::GetGamma(){
    return Gamma;
}

double VortexLine::GetLength(){
    return Length;
}

void VortexLine::Delete(){

    pL->detachLine(this);
    pT->detachLine(this);

    if (!pL->hasLines()){
        for (int i=0;i<m_Nodes->size();i++){
            if (m_Nodes->at(i) == pL){
                m_Nodes->erase(m_Nodes->begin()+i);
                delete pL;
            }
        }
    }

    if (!pT->hasLines()){
        for (int i=0;i<m_Nodes->size();i++){
            if (m_Nodes->at(i) == pT){
                m_Nodes->erase(m_Nodes->begin()+i);
                delete pT;
            }
        }
    }

    for (int i=0;i<m_Lines->size();i++){
            if (m_Lines->at(i) == this){
                m_Lines->erase(m_Lines->begin()+i);
                delete this;
                break;
            }
    }
}


