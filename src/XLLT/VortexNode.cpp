#include "VortexNode.h"

VortexNode::VortexNode(){
    x=0.0;
    y=0.0;
    z=0.0;

    fromTimestep = -1;
    fromStation = -1;
    wasConvected = false;
    V_induced.Set(0,0,0);
    attachedLines.clear();
}

VortexNode::VortexNode(double const &xi, double const &yi, double const &zi)
    : CVector (xi, yi, zi) {

    fromTimestep = -1;
    fromStation = -1;
    wasConvected = false;
    V_induced.Set(0,0,0);
    attachedLines.clear();
}

void VortexNode::attachLine(void *line){
    for (int i = 0; i< attachedLines.size();i++){
        if (attachedLines.at(i) == line) return;
    }
    attachedLines.push_back(line);
}

void VortexNode::detachLine(void *line){
    for (int i=0;i<attachedLines.size();i++){
        if (attachedLines.at(i) == line){
            attachedLines.erase(attachedLines.begin()+i);
        }
    }
}

bool VortexNode::hasLines(){
    if (attachedLines.size()) return true;
    else return false;
}
