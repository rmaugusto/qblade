#ifndef NOISEOPPOINT_H
#define NOISEOPPOINT_H

class OpPoint;


/* This class is a conveniance wrapper for OpPoint. Its purpose is to provide "fake" OpPoints with values only for
 * alpha and reynolds and to reduce the interface to some getter functions. If the m_opPoint pointer is set, this class
 * behaves like a wrapper. If it is not set, this class returns its own values for alpha and reynolds.
 * */

class NoiseOpPoint
{
public:
	NoiseOpPoint(OpPoint *opPoint);
	NoiseOpPoint(double reynolds, double alpha);
	
	double getReynolds();
	double getAlphaDegree();
	double getAlphaDegreeAbsolute();
	
	// the following are only available for true OpPoints and crash for fake OpPoints
	int getNSide1();
	int getNSide2();    
	double getXValue(int index, int topOrBot);
	double getDstrAt(int x, int y);
	
private:
	double m_reynolds, m_alpha;
	OpPoint *m_opPoint;
};

#endif // NOISEOPPOINT_H
