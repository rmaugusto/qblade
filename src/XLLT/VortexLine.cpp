#include "VortexLine.h"
#include <QtCore>


VortexLine::VortexLine(){
    isTip = false;
    isHub = false;
    deleteLater = false;
    isTrailing = false;
    isShed = false;
    isConcentrated = false;
    oldLength = 0;
    stretchFactor = 1;
    Length = 0;
    oldLengthPredCorr = 0;
    age = 0;
    coreSizeSquared = 0;
    Gamma = 0.0;
    VizGamma = 0.0;
    pT = NULL;
    pL = NULL;
    fromStation = -1;
    fromTimestep = -1;
    fromTime = -1;
    fromRevolution = -1;
    m_Lines = NULL;
    m_Nodes = NULL;
    includeStrain = true;
    leftPanel = NULL;
    rightPanel = NULL;
    fromLine = NULL;
    isStrut = false;
}

void VortexLine::InitVortexSize(){
    double a = 1.25643;
    coreSizeSquared = 4*a*m_TurbulentViscosity*m_KinViscosity*(m_VortexTimeOffset);
}

void VortexLine::InitLength(){
    oldLengthPredCorr = Length;
    Length = CVector(*pL-*pT).VAbs();
}

void VortexLine::Update(double dT){
   double a = 1.25643; // a factor from literature
   double strain;
   Length = CVector(*pL-*pT).VAbs();
   if (oldLength == 0 || !includeStrain) strain = 0;
   else strain = (Length-oldLength) / oldLength;
   stretchFactor *= (strain+1);
   coreSizeSquared += 4*a*m_TurbulentViscosity*m_KinViscosity*dT;
   coreSizeSquared *= 1/(1+strain);
   age += dT;
   oldLength = Length;
}

void VortexLine::SetGamma(double gamma){
    Gamma = gamma;
}

double VortexLine::GetGamma(){
    return Gamma;
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


