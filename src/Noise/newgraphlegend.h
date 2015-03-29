#ifndef NEWGRAPHLEGEND_H
#define NEWGRAPHLEGEND_H

#include <QPainter>
#include "../Graph/NewGraph.h"

class NewGraphLegend : public NewGraph
{
public:    
    NewGraphLegend(QString nameInSettings, GraphType graphType, TwoDWidgetInterface *twoDInterface);

    ~NewGraphLegend();

    void drawGraph(QPainter &painter);

};

#endif // NEWGRAPHLEGEND_H
