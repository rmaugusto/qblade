#include "mode.h"
#include <iostream>
Mode::Mode(): ModeType(UnSorted)
{
}

void Mode::PrintMode()
{
    std::cout<<"--------------------------------------------------------------------------------------"<<std::endl;
    switch(ModeType){
        case 0:
        {
            std::cout<<"Flap Mode"<<std::endl;
            break;
        }
        case 1:
        {
            std::cout<<"Edge Mode"<<std::endl;
            break;
        }
        case 2:
        {
            std::cout<<"Torsion Mode"<<std::endl;
            break;
        }
        case 3:
        {
            std::cout<<"Radial Mode"<<std::endl;
            break;
        }
        case 4:
        {
            std::cout<<"Unsorted Mode"<<std::endl;
            break;
        }
    }
    std::cout<<"Eigenfrequency: "<<Frequency<<"Rad/s,  "<<Frequency/(2.0*3.14)<<"Hz"<<std::endl;
    std::cout<<"Modal Translation at each Node(x,y,z)"<<std::endl;
    for(int i = 0; i < (int)ModeShape.size(); i++)
    {
        DeformationVector TempNode = ModeShape.at(i);

       TempNode.PrintCoords();
    }

    std::cout<<"Modal Rotation at each Node"<<std::endl;
    for(int i = 0; i < (int)ModeShape.size(); i++)
    {
        DeformationVector TempNode = ModeShape.at(i);

        std::cout<<TempNode.zAxisAngle<<std::endl;
    }
    std::cout<<"FAST Polynomial"<<std::endl;
    if(Polynomial.rows()!=0)
    {
        std::cout<<Polynomial(0,0);
        for(int k=1; k<Polynomial.rows();k++)
        {
            std::cout<<"+("<<Polynomial(k,0)<<"*x^"<<k<<")";
        }
        std::cout<<std::endl;
    }
    std::cout<<"--------------------------------------------------------------------------------------"<<std::endl;
}
