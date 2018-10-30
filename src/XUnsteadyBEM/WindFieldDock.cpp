/****************************************************************************

	WindFieldCreator Class
		Copyright (C) 2012 Nikolai Moesus

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*****************************************************************************/

#include "WindFieldDock.h"

#include "../Store.h"
#include "WindFieldCreatorDialog.h"
#include "../ParameterGrid.h"
#include "WindField.h"
#include "../MainFrame.h"

typedef Parameter::Windfield P;


WindFieldDock::WindFieldDock (const QString & title, QMainWindow * parent, Qt::WindowFlags flags, WindFieldModule *module)
    : CreatorDock<WindField> (title, parent, flags),
	  m_module(module)
{
    setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
    setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

	QGroupBox *groupBox = new QGroupBox ("Windfield Parameter");
	m_contentVBox->addWidget(groupBox);
		ParameterGrid<P> *grid = new ParameterGrid<P>(this);
		groupBox->setLayout(grid);
			grid->addLabel(P::Time, true, "Time [s]:");
			grid->addLabel(P::Timesteps, true, "Timesteps:");
			grid->addLabel(P::Points, true, "Points per Direction:");
	groupBox = new QGroupBox ("Simulation Parameter");
	m_contentVBox->addWidget(groupBox);
		grid = new ParameterGrid<P>(this);
		groupBox->setLayout(grid);
			grid->addLabel(P::RotorRadius, true, "Rotor Radius []:", LENGTH);
			grid->addLabel(P::HubHeight, true, "Hub Height []:", LENGTH);
			grid->addLabel(P::WindSpeed, true, "Mean Wind Speed []:", SPEED);
            grid->addLabel(P::MeasurementHeight, true, "Measurement Height []:", LENGTH);
			grid->addLabel(P::Turbulence, true, "Turbulence Intensity [%]:");
			grid->addLabel(P::ShearLayer, false, "Include Shear Layer:");
			grid->addLabel(P::RoughnessLength, true, "Roughness Length []:", LENGTH);
	m_contentVBox->addStretch();		
	
	connect(g_mainFrame, SIGNAL(unitsChanged()), this, SLOT(onUnitsChanged()));
	onUnitsChanged();
	
	addScrolledDock (Qt::LeftDockWidgetArea, parent, 300);
}

void WindFieldDock::setShownObject(WindField *newObject) {
	CreatorDock::setShownObject(newObject);
	
	loadObject(m_shownObject);
	
	if (m_shownObject) {
		const bool includeShear = m_shownObject->getIncludeShear();
		get<QLabel>(P::MeasurementHeight)->setEnabled(includeShear);
		get<QLabel>(P::RoughnessLength)->setEnabled(includeShear);
		if (!includeShear) {
			get<QLabel>(P::MeasurementHeight)->setText("--");
			get<QLabel>(P::RoughnessLength)->setText("--");
		}
	}
}

void WindFieldDock::onUnitsChanged() {
	setUnitContainingLabels();
	setShownObject(m_shownObject);  // reload object
}

void WindFieldDock::onEditCopyButtonClicked () {
	WindFieldCreatorDialog *dialog = new WindFieldCreatorDialog (m_shownObject, m_module);
	dialog->exec();
	delete dialog;
}

void WindFieldDock::onRenameButtonClicked () {
	g_windFieldStore.rename(m_shownObject);
}

void WindFieldDock::onDeleteButtonClicked () {
	g_windFieldStore.remove(m_shownObject);
}

void WindFieldDock::onNewButtonClicked () {
	WindFieldCreatorDialog *dialog = new WindFieldCreatorDialog (NULL, m_module);
	dialog->exec();
	delete dialog;
}
