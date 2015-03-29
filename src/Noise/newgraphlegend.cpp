#include "newgraphlegend.h"


NewGraphLegend::NewGraphLegend(QString nameInSettings, NewGraph::GraphType graphType, TwoDWidgetInterface *twoDInterface)
    : NewGraph(nameInSettings, graphType, twoDInterface)
{

}

NewGraphLegend::~NewGraphLegend()
{

}

void NewGraphLegend::drawGraph(QPainter &painter)
{
    NewGraph::drawGraph(painter);


}

