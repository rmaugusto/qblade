#include "TwoDGraphMenu.h"

#include <QDebug>
#include <QMainWindow>

#include "Module.h"


TwoDGraphMenu::TwoDGraphMenu(QMainWindow *parent, TwoDModule *module)
	: QMenu(parent)
{
	m_module = module;
	
	setTitle ("Graph");
	connect (this, SIGNAL(aboutToShow()), SLOT(onAboutToShow()));
	
	m_singleGraphAction = new QAction("Single Graph", this);
	m_singleGraphAction->setCheckable(true);
	connect(m_singleGraphAction, SIGNAL(triggered()), this, SLOT(onGraphArrangementChanged()));
	addAction(m_singleGraphAction);
	m_twoHorizontalGraphsAction = new QAction("Two Graphs Horizontal", this);
	m_twoHorizontalGraphsAction->setCheckable(true);
	connect(m_twoHorizontalGraphsAction, SIGNAL(triggered()), this, SLOT(onGraphArrangementChanged()));
	addAction(m_twoHorizontalGraphsAction);
	m_twoVerticalGraphsAction = new QAction("Two Graphs Vertical", this);
	m_twoVerticalGraphsAction->setCheckable(true);
	connect(m_twoVerticalGraphsAction, SIGNAL(triggered()), this, SLOT(onGraphArrangementChanged()));
	addAction(m_twoVerticalGraphsAction);
    m_threeGraphsAction = new QAction("Three Graphs Vertical", this);
	m_threeGraphsAction->setCheckable(true);
	connect(m_threeGraphsAction, SIGNAL(triggered()), this, SLOT(onGraphArrangementChanged()));
	addAction(m_threeGraphsAction);
	m_fourGraphsAction = new QAction("Four Graphs", this);
	m_fourGraphsAction->setCheckable(true);
	connect(m_fourGraphsAction, SIGNAL(triggered()), this, SLOT(onGraphArrangementChanged()));
	addAction(m_fourGraphsAction);
    m_fourGraphsVerticalAction = new QAction("Four Graphs Vertical", this);
    m_fourGraphsVerticalAction->setCheckable(true);
    connect(m_fourGraphsVerticalAction, SIGNAL(triggered()), this, SLOT(onGraphArrangementChanged()));
    addAction(m_fourGraphsVerticalAction);
}

void TwoDGraphMenu::onAboutToShow() {
	m_singleGraphAction->setChecked(m_module->getGraphArrangement() == TwoDWidgetInterface::Single);
	m_twoHorizontalGraphsAction->setChecked(m_module->getGraphArrangement() == TwoDWidgetInterface::Horizontal);
	m_twoVerticalGraphsAction->setChecked(m_module->getGraphArrangement() == TwoDWidgetInterface::Vertical);
	m_threeGraphsAction->setChecked(m_module->getGraphArrangement() == TwoDWidgetInterface::Vertical3);
	m_fourGraphsAction->setChecked(m_module->getGraphArrangement() == TwoDWidgetInterface::Quad);
    m_fourGraphsVerticalAction->setChecked(m_module->getGraphArrangement() == TwoDWidgetInterface::QuadVertical);
}

void TwoDGraphMenu::onGraphArrangementChanged() {
	if (QObject::sender() == m_singleGraphAction) {
		m_module->setGraphArrangement(TwoDWidgetInterface::Single);
	} else if (QObject::sender() == m_twoHorizontalGraphsAction) {
		m_module->setGraphArrangement(TwoDWidgetInterface::Horizontal);
	} else if (QObject::sender() == m_twoVerticalGraphsAction) {
		m_module->setGraphArrangement(TwoDWidgetInterface::Vertical);
	} else if (QObject::sender() == m_threeGraphsAction) {
		m_module->setGraphArrangement(TwoDWidgetInterface::Vertical3);
	} else if (QObject::sender() == m_fourGraphsAction) {
		m_module->setGraphArrangement(TwoDWidgetInterface::Quad);
    } else if (QObject::sender() == m_fourGraphsVerticalAction) {
        m_module->setGraphArrangement(TwoDWidgetInterface::QuadVertical);
    }
}
