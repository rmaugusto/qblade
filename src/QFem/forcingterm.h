#ifndef FORCINGTERM_H
#define FORCINGTERM_H
/*!
 * Simply contains the degree of freedom and a force for a static analysis.
 *
 */
class ForcingTerm
{
public:

    int DOF;
    double Force;

    ForcingTerm(int _DOF, double _Force)
    {
        DOF = _DOF;
        Force= _Force;
    }
    void Print();
};

#endif // FORCINGTERM_H
