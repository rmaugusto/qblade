#include "forcingterm.h"
#include<iostream>

void ForcingTerm::Print()
{
    std::cout<<"Forcing Term on DOF: "<<DOF<<" with a value of: "<<Force<<"N"<<std::endl;
}
