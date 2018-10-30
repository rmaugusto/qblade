#ifndef C360POLAR_H
#define C360POLAR_H

#include <QString>
#include <QList>
#include <QColor>
#include <QTextStream>

#include "../StorableObject.h"
class CFoil;


class C360Polar : public StorableObject
{
public:
	static C360Polar* newBySerialize ();
	C360Polar (QString name = "< no name >", StorableObject *parent = NULL);

	void serialize ();  // override from StorableObject
	void restorePointers();  // override from StorableObject
	
	static QStringList prepareMissingObjectMessage();
	
    void ExportPolarNREL(QTextStream &out);
    void getCdMinimum(double &cdMin, double &cdMinAngle);
    void getClMaximum(double &clMax, double &clMaxAngle);
    double getClMaximum();
    double getClMinimum();
    double getCdAtAlphaZero();
    void GetLinearizedCn(double &Alpha0, double &slope);
    double GetZeroLiftAngle();
    void GetCnAtStallAngles(double &cnPosStallAlpha, double &cnNegStallAlpha);
    QList<double> GetPropertiesAt(double AoA);


	// TODO probably the most uses of m_airfoil are bad style and should be changed
	CFoil *m_airfoil;  // the airfoil this polar is based on
	
    double alpha_zero;
    double slope;
    double CD90;
    double reynolds;
    double posalphamax;
	
	QList <double> m_Alpha;
	QList <double> m_Cl;
	QList <double> m_Cd;
	QList <double> m_Cm;
    QList <double> m_Cl_att;
    QList <double> m_Cl_sep;
    QList <double> m_fst;
    bool m_bisDecomposed;
    double CLzero;
	
	int m_Style;
	int m_Width;
	bool m_bIsVisible;
	bool m_bShowPoints;
	QColor m_Color;
};

#endif // C360POLAR_H
