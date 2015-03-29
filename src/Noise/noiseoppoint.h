#ifndef NOISEOPPOINT_H
#define NOISEOPPOINT_H

/**
 * @brief The NoiseOpPoint class
 * This class is a simplified representation of OpPoint
 */
class NoiseOpPoint
{
public:
    NoiseOpPoint(double reynolds,double alpha) : m_Reynolds(reynolds),m_Alpha(alpha) {}
    NoiseOpPoint();
    ~NoiseOpPoint();

    double Reynolds() const;
    void setReynolds(double Reynolds);

    double Alpha() const;
    void setAlpha(double Alpha);

//    double DStar() const;
//    void setDStar(double DStar);

    double AlphaDeg();

private:
    double m_Reynolds;
    //Alpha in radians
    double m_Alpha;
//    double m_DStar;

};

#endif // NOISEOPPOINT_H
