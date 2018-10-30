#include "360Polar.h"

#include <QDebug>

#include "../Globals.h"
#include "../Serializer.h"
#include "../Store.h"
#include "../Objects/Foil.h"
#include "../Objects/Polar.h"
#include <QDate>
#include <QTime>
#include "../XBEM/BEM.h"


C360Polar* C360Polar::newBySerialize() {
	C360Polar* polar360 = new C360Polar ();
	polar360->serialize();
	return polar360;
}

C360Polar::C360Polar(QString name, StorableObject *parent)
    : StorableObject (name, parent)
{
    alpha_zero = 0;
    slope = 0;
    CD90 = 0;
    reynolds = 0;
	m_Style = 0;
	m_Width = 1;
	m_bIsVisible = true;
	m_bShowPoints = false;
    m_bisDecomposed = false;
//	m_Color = ;	 // NM not initialised in CPolar constructor too
}

void C360Polar::serialize() {
	StorableObject::serialize();
	
	g_serializer.readOrWriteStorableObject (&m_airfoil);
	
	g_serializer.readOrWriteDouble (&alpha_zero);
	g_serializer.readOrWriteDouble (&slope);
	g_serializer.readOrWriteDouble (&CD90);
	g_serializer.readOrWriteDouble (&reynolds);	
	
	g_serializer.readOrWriteDoubleList1D (&m_Alpha);
	g_serializer.readOrWriteDoubleList1D (&m_Cl);
	g_serializer.readOrWriteDoubleList1D (&m_Cd);
	g_serializer.readOrWriteDoubleList1D (&m_Cm);
	
	g_serializer.readOrWriteInt (&m_Style);
	g_serializer.readOrWriteInt (&m_Width);
	g_serializer.readOrWriteBool (&m_bIsVisible);
	g_serializer.readOrWriteBool (&m_bShowPoints);
	g_serializer.readOrWriteColor (&m_Color);

    if (g_serializer.getArchiveFormat() > 100032){
        g_serializer.readOrWriteDoubleList1D (&m_Cl_att);
        g_serializer.readOrWriteDoubleList1D (&m_Cl_sep);
        g_serializer.readOrWriteDoubleList1D (&m_fst);
        g_serializer.readOrWriteBool(&m_bisDecomposed);
        g_serializer.readOrWriteDouble(&CLzero);
    }
    else if (g_serializer.isReadMode() && g_serializer.getArchiveFormat() < 100033){
        for (int i=0; i<m_Alpha.size();i++){
            m_Cl_att.append(0);
            m_Cl_sep.append(0);
            m_fst.append(0);
            m_bisDecomposed = false;
            CLzero = 0;
        }
    }
}

void C360Polar::restorePointers() {
	StorableObject::restorePointers();
	
	g_serializer.restorePointer (reinterpret_cast<StorableObject**> (&m_airfoil));
}

QStringList C360Polar::prepareMissingObjectMessage() {
	if (g_360PolarStore.isEmpty() && g_qbem->m_pCur360Polar == NULL) {
		QStringList message = CPolar::prepareMissingObjectMessage();
		if (message.isEmpty()) {
			message = QStringList(">>> Create a new 360 Polar in the 360 Polar Extrapolation Module");
		}
		message.prepend("- No 360 Polar in Database");
		return message;
	} else {
		return QStringList();
	}
}

QList<double> C360Polar::GetPropertiesAt(double AoA) {
    QList<double> results;
    double clMax, CdZero, clMin;
    clMax = getClMaximum();
    clMin = getClMinimum();
    CdZero = getCdAtAlphaZero();

    for (int i=0;i<m_Alpha.size()-1;i++){
        if (AoA >= m_Alpha.at(i) && AoA <= m_Alpha.at(i+1)){
            results.append(m_Cl.at(i)+(AoA-m_Alpha.at(i))/(m_Alpha.at(i+1)-m_Alpha.at(i))*(m_Cl.at(i+1)-m_Cl.at(i)));
            results.append(m_Cd.at(i)+(AoA-m_Alpha.at(i))/(m_Alpha.at(i+1)-m_Alpha.at(i))*(m_Cd.at(i+1)-m_Cd.at(i)));
            results.append(m_Cl_att.at(i)+(AoA-m_Alpha.at(i))/(m_Alpha.at(i+1)-m_Alpha.at(i))*(m_Cl_att.at(i+1)-m_Cl_att.at(i)));
            results.append(m_Cl_sep.at(i)+(AoA-m_Alpha.at(i))/(m_Alpha.at(i+1)-m_Alpha.at(i))*(m_Cl_sep.at(i+1)-m_Cl_sep.at(i)));
            results.append(m_fst.at(i)+(AoA-m_Alpha.at(i))/(m_Alpha.at(i+1)-m_Alpha.at(i))*(m_fst.at(i+1)-m_fst.at(i)));
            results.append(clMax);
            results.append(CdZero);
            results.append(clMin);
            break;
        }
    }
    if (AoA < m_Alpha.at(0) || AoA > m_Alpha.at(m_Alpha.size()-1)){
        results.append(m_Cl.at(0));
        results.append(m_Cd.at(0));
        results.append(m_Cl.at(0));
        results.append(m_Cl_sep.at(0));
        results.append(m_fst.at(0));
        results.append(clMax);
        results.append(CdZero);
        results.append(clMin);
    }

    return results;
}

void C360Polar::ExportPolarNREL(QTextStream &out) {
	/* export the polar in NREL format compatibel with AeroDyn 13 and WT_perf 3 */
    QDate date = QDate::currentDate();
    QTime time = QTime::currentTime();
	double clMaxAngle, cdMin, cdMinAngle;
	double cnSlopeZeroLift, cnStallPositiveAngle, cnStallNegativeAngle;
	double dummy; // used for not needed values
	getClMaximum (dummy, clMaxAngle);
	getCdMinimum (cdMin, cdMinAngle);
	GetLinearizedCn (dummy, cnSlopeZeroLift);
	GetCnAtStallAngles (cnStallPositiveAngle, cnStallNegativeAngle);
	
	/* version for WT_perf. Not compatibel with AeroDyn 13, althoug it says so... */
//	out <<  "AeroDyn airfoil file.  Compatible with AeroDyn v13.0." << endl <<
//			"Polar \"" << m_PlrName << "\" on Foil \"" << m_FoilName << "\"" << endl <<
//			"Generated by QBlade" << endl <<
//			QString("%1").arg(1, -15) <<
//			"Number of airfoil tables in this file" << endl <<
//			QString("%1").arg(m_Reynolds, -15, 'e', 3) <<
//			"Reynolds number in millions.  For efficiency, make very large if only one table." << endl <<
//			QString("%1").arg(0, -15, 'f', 1) <<
//			"Control setting" << endl <<  // NM: whatever it means...
//			QString("%1").arg(clMaxAngle, -15, 'f', 2) <<
//			"Stall angle (deg)" << endl <<
//			QString("%1").arg(GetZeroLiftAngle(), -15, 'f', 2) <<
//			"Zero lift angle of attack (deg)" << endl <<
//			QString("%1").arg(cnSlopeZeroLift, -15, 'f', 5) <<
//			"Cn slope for zero lift (dimensionless)" << endl <<
//			QString("%1").arg(cnStallPositiveAngle, -15, 'f', 4) <<
//			"Cn at stall value for positive angle of attack" << endl <<
//			QString("%1").arg(cnStallNegativeAngle, -15, 'f', 4) <<
//			"Cn at stall value for negative angle of attack" << endl <<
//			QString("%1").arg(cdMinAngle, -15, 'f', 2) <<
//			"Angle of attack for minimum CD (deg)" << endl <<
//			QString("%1").arg(cdMin, -15, 'f', 4) <<
//			"Minimum CD value" << endl;

	/* Version that really is compatibel with AeroDyn 13 */
    out <<  "AeroDyn airfoil file Created with " << g_mainFrame->m_VersionName << " on "<<date.toString("dd.MM.yyyy") << " at " << time.toString("hh:mm:ss") << " Compatible with AeroDyn v13.0." << endl <<
			"Polar \"" << getName() << "\" on Foil \"" << getParent()->getName() << "\" :: generated by QBlade"<< endl <<
			QString("%1").arg(1, -15) <<
			"Number of airfoil tables in this file" << endl <<
			QString("%1").arg(0, -15) <<
			"Table ID parameter" << endl <<
			QString("%1").arg(clMaxAngle, -15, 'f', 2) <<
			"Stall angle (deg)" << endl <<
			"0              No longer used, enter zero" << endl <<
			"0              No longer used, enter zero" << endl <<
			"0              No longer used, enter zero" << endl <<
			QString("%1").arg(GetZeroLiftAngle(), -15, 'f', 2) <<
			"Angle of attack for zero Cn for linear Cn curve (deg)" << endl <<
			QString("%1").arg(cnSlopeZeroLift, -15, 'f', 5) <<
			"Cn slope for zero lift for linear Cn curve (1/rad)" << endl <<
			QString("%1").arg(cnStallPositiveAngle, -15, 'f', 4) <<
			"Cn at stall value for positive angle of attack for linear Cn curve" << endl <<
			QString("%1").arg(cnStallNegativeAngle, -15, 'f', 4) <<
			"Cn at stall value for negative angle of attack for linear Cn curve" << endl <<
			QString("%1").arg(cdMinAngle, -15, 'f', 2) <<
			"Angle of attack for minimum CD (deg)" << endl <<
			QString("%1").arg(cdMin, -14, 'f', 4) <<
			" Minimum CD value" << endl;
	
	for (int i = 0; i < m_Alpha.size(); ++i) {
		out << QString("%1").arg(m_Alpha[i], 10, 'f', 2) <<
			   QString("%1").arg(m_Cl[i], 10, 'f', 4) <<
			   QString("%1").arg(m_Cd[i], 10, 'f', 4) <</*
			   QString("%1").arg(m_Cm[i], 10, 'f', 4) << */endl;  // m_Cm is not filled with values
	}
    if (m_Alpha.size()){
	/* add the last line for +180 degree that is not contained in the arrays */
	out << QString("%1").arg(180.0, 10, 'f', 2) <<
		   QString("%1").arg(m_Cl[0], 10, 'f', 4) <<
		   QString("%1").arg(m_Cd[0], 10, 'f', 4) <</*
		   QString("%1").arg(m_Cm[0], 10, 'f', 4) <<*/ endl;  /* <<
		   "EOT" << endl;  // only for WT_Perf*/
    }
    else{
    out << QString(tr("- exported polar did not contain any data -"));
    }
}

void C360Polar::GetCnAtStallAngles(double &cnPosStallAlpha, double &cnNegStallAlpha)
{
    //the stall angles are best seen in the peaks of the Cl/Cd over Alpha curve.
    //Only the part between -50 and +50 deg is considered
    QList <double> ClCd;
    QList <double> Alpha;
    QList <double> Cn;
    cnNegStallAlpha = 0;
    cnPosStallAlpha = 0;

    if (m_Cl.size() != m_Cd.size()) return;

    for (int i = 0; i < m_Cl.size(); ++i)
    {
        if ((m_Alpha[i] > -50) && (m_Alpha[i] < 50) && (m_Cd[i] != 0.0) )
        {
            Alpha.append(m_Alpha[i]);
            ClCd.append(m_Cl[i] / m_Cd[i]);
            Cn.append( m_Cl[i]*cos(m_Alpha[i]*PI/180) + m_Cd[i]*sin(m_Alpha[i]*PI/180));
        }

    }

    bool bNegStallFound = 0;
    bool bPosStallFound = 0;

    //get 2 Inflection points
    for (int i = 0; i < ClCd.size()-1; ++i)
    {
       if (!bNegStallFound)
        if (ClCd[i+1] > ClCd[i])
         {
             //double negStallAlpha = Alpha[i];
             bNegStallFound = 1;
             cnNegStallAlpha = Cn[i];
         }


       if (!bPosStallFound && bNegStallFound)
        if (ClCd[i+1] < ClCd[i])
         {
             //double posStallAlpha = Alpha[i];
             bPosStallFound = 1;
             cnPosStallAlpha = Cn[i];
         }

    }
}

double C360Polar::GetZeroLiftAngle()
{
    /////////////// new code NM ///////////////
    // only consider reasonable aoa range
    QList<double> clRange;
    QList<double> alphaRange;

    for (int i = 0; i < m_Alpha.size(); ++i)
    {
        if (m_Alpha[i] > -10 && m_Alpha[i] < 10)
        {
            clRange.append(m_Cl[i]);
            alphaRange.append(m_Alpha[i]);
        }

    }
    /////////// end new code NM ///////////////


    double Clmin =  1000.0;
    double Clmax = -1000.0;
    for (int i=0; i<clRange.size(); i++)
    {
        Clmin = qMin(Clmin, clRange[i]);
        Clmax = qMax(Clmax, clRange[i]);
    }
    if(!(Clmin<0.0) || !(Clmax>0.0))
        return 0.0;

    int k=0;
//	double rr  = clRange[k];
//	double rr1 = clRange[k+1];

    while (clRange[k+1]<0.0)
    {
//		rr  = clRange[k];
//		rr1 = clRange[k+1];
        k++;
    }
    if(k+1>=alphaRange.size()) return 0.0;
    double Alpha0 = alphaRange[k] + (alphaRange[k+1]-alphaRange[k])*(0.0-clRange[k])/(clRange[k+1]-clRange[k]);
    return Alpha0;

}

void C360Polar::GetLinearizedCn(double &Alpha0, double &slope)
{
    //linearize Cn vs. Alpha set of points by least square method
    QList <double> Cn;
    QList <double> Alpha;

    int n;
    double alpha0L = GetZeroLiftAngle();

    //calculate Cn
    //all points in the range -3/+5 deg around zero lift angle is taken into account
    for (int i = 0; i < m_Cl.size(); ++i)
    {
        if ((m_Alpha[i] > (alpha0L - 3)) && (m_Alpha[i] < (alpha0L + 3)))
        {
            Cn.append( m_Cl[i]*cos(m_Alpha[i]*PI/180) + m_Cd[i]*sin(m_Alpha[i]*PI/180));
            Alpha.append(m_Alpha[i]);
        }

    }

    n = (int)Cn.size();

    if(n<=1)
    {
        Alpha0 = 0.0;
        slope = 2.0*PI*PI/180.0;
        return;
    }


    double fn = (double)n;
    double sum1 = 0.0;
    double sum2 = 0.0;
    double sum3 = 0.0;
    double sum4 = 0.0;
    double b1, b2;

    for (int k=0; k<n; k++)
    {
        sum1 += Cn[k] * Alpha[k];
        sum2 += Alpha[k];
        sum3 += Cn[k];
        sum4 += Alpha[k] * Alpha[k];
    }


    if(fn*sum4 == sum2*sum2 || fn*sum1 == sum2 * sum3) {//very improbable...
        Alpha0 = 0.0;
        slope = 2.0*PI*PI/180.0;
        return;
    }


    b1 = (fn*sum1 - sum2 * sum3)/(fn*sum4 - sum2*sum2);
    b2 = (sum3 - b1 * sum2)/fn;

    slope  = b1*180/PI; //in cn/alpha[rad]
    Alpha0 = -b2/b1;
}

void C360Polar::getCdMinimum(double &cdMin, double &cdMinAngle) {
    if (m_Cd.empty()) {
        cdMin = 0.0;
        cdMinAngle = 0.0;
    } else {
        int minIndex = m_Cd.size() / 2;  // guess some value in the middle
        for (int i = 0; i < m_Cd.size(); ++i) {
            // search Cd minimum between -20 and +20 degree
            if (m_Cd[i] < m_Cd[minIndex] && m_Alpha[i] > -20 && m_Alpha[i] < +20) {
                minIndex = i;
            }
        }
        cdMin = m_Cd[minIndex];
        cdMinAngle = m_Alpha[minIndex];
    }
}

void C360Polar::getClMaximum(double &clMax, double &clMaxAngle) {
    if (m_Cl.empty()) {
        clMax = 0.0;
        clMaxAngle = 0.0;
    } else {
        int maxIndex = 0;
        for (int i = 0; i < m_Cl.size(); ++i) {
            if (m_Cl[i] > m_Cl[maxIndex]) {
                maxIndex = i;
            }
        }
        clMax = m_Cl[maxIndex];
        clMaxAngle = m_Alpha[maxIndex];
    }
}

double C360Polar::getClMaximum() {
    if (m_Cl.empty()) {
        return 0;
    } else {
        int maxIndex = 0;
        for (int i = 0; i < m_Cl.size(); ++i) {
            if (m_Cl[i] > m_Cl[maxIndex]) {
                maxIndex = i;
            }
        }
        return  m_Cl[maxIndex];
    }
}

double C360Polar::getClMinimum() {
    if (m_Cl.empty()) {
        return 0;
    } else {
        int minIndex = 0;
        for (int i = 0; i < m_Cl.size(); ++i) {
            if (m_Cl[i] < m_Cl[minIndex]) {
                minIndex = i;
            }
        }
        return  m_Cl[minIndex];
    }
}

double C360Polar::getCdAtAlphaZero(){
    for (int j=0;j<m_Alpha.size()-1;j++){
        if (alpha_zero >= m_Alpha.at(j) && alpha_zero <= m_Alpha.at(j+1)){
            return m_Cd.at(j)+(alpha_zero-m_Alpha.at(j))/(m_Alpha.at(j+1)-m_Alpha.at(j))*(m_Cd.at(j+1)-m_Cd.at(j));
            break;
        }
    }
    return -10;
}
