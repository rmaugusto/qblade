#include "GlobalFunctions.h"

#include <QRegExp>
#include <QDir>
#include <QDebug>

#include "Store.h"
#include "Params.h"
#include "MainFrame.h"


extern bool ObjectIsEdited;





hsv rgb2hsv(rgb in)
{
hsv         out;
double      min, max, delta;

min = in.r < in.g ? in.r : in.g;
min = min  < in.b ? min  : in.b;

max = in.r > in.g ? in.r : in.g;
max = max  > in.b ? max  : in.b;

out.v = max;                                // v
delta = max - min;
if( max > 0.0 ) {
    out.s = (delta / max);                  // s
} else {
    // r = g = b = 0                        // s = 0, v is undefined
    out.s = 0.0;
    out.h = NAN;                            // its now undefined
    return out;
}
if( in.r >= max )                           // > is bogus, just keeps compilor happy
    out.h = ( in.g - in.b ) / delta;        // between yellow & magenta
else
if( in.g >= max )
    out.h = 2.0 + ( in.b - in.r ) / delta;  // between cyan & yellow
else
    out.h = 4.0 + ( in.r - in.g ) / delta;  // between magenta & cyan

out.h *= 60.0;                              // degrees

if( out.h < 0.0 )
    out.h += 360.0;

return out;
}


rgb hsv2rgb(hsv in)
{
double      hh, p, q, t, ff;
long        i;
rgb         out;

if(in.s <= 0.0) {       // < is bogus, just shuts up warnings
    out.r = in.v;
    out.g = in.v;
    out.b = in.v;
    return out;
}
hh = in.h;
if(hh >= 360.0) hh = 0.0;
hh /= 60.0;
i = (long)hh;
ff = hh - i;
p = in.v * (1.0 - in.s);
q = in.v * (1.0 - (in.s * ff));
t = in.v * (1.0 - (in.s * (1.0 - ff)));

switch(i) {
case 0:
    out.r = in.v;
    out.g = t;
    out.b = p;
    break;
case 1:
    out.r = q;
    out.g = in.v;
    out.b = p;
    break;
case 2:
    out.r = p;
    out.g = in.v;
    out.b = t;
    break;

case 3:
    out.r = p;
    out.g = q;
    out.b = in.v;
    break;
case 4:
    out.r = t;
    out.g = p;
    out.b = in.v;
    break;
case 5:
default:
    out.r = in.v;
    out.g = p;
    out.b = q;
    break;
}
return out;
}

QString makeNameWithHigherNumber (QString oldName) {
	int number = 2;
	int position = oldName.lastIndexOf(QRegExp(" [(][0-9]+[)]$"));
	if (position >= 0) {  // read and truncate old number if existing
		number = oldName.mid(position+2, oldName.length()-position-3).toInt() + 1;
		oldName.truncate(position);
	}
	
	return QString(oldName + " (%1)").arg(number);
}

bool GLscreenMessage(int iApp, int iView, GLWidget *glWidget){

    if (ObjectIsEdited) return false;

    glPushAttrib (GL_ALL_ATTRIB_BITS);  // save the openGL state
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_LIGHT0);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0, 0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    if (!glWidget) return true;

//    if (glWidget){
//    const double width = glWidget->width();
//    const double height = glWidget->height();
//    }

    glColor3d(0.0,0.0,0.0);

    bool isFoil = g_foilStore.size();
    bool isPolar = g_polarStore.size();
    bool is360Polar = g_360PolarStore.size();
    bool isBlade = g_rotorStore.size();
    bool isVBlade = g_verticalRotorStore.size();
    bool isStruct = g_bladeStructureStore.size();
    bool isLoad = g_bladestructureloadingStore.size();
    bool isWind = g_windFieldStore.size();  
    bool isLLT = g_QLLTHAWTSimulationStore.size();

    int why1 = 0.02*glWidget->rect().width();
    int why2 = 0.015*glWidget->rect().width();
    QFont font(g_mainFrame->m_TextFont.family(),why1 );
    QFont font2(g_mainFrame->m_TextFont.family(),why2 );



    int dist = 0.08*glWidget->rect().height();
    int xdist = 0.15*glWidget->rect().height();
    int ydist = 0.15*glWidget->rect().height();

    bool giveMsg = false;
    int i = 0;
    QString tip;


    QString strong = "Missing Objects:";
    bool missing = false;



    if (iApp == QFEMMODULE){
        if (iView == QFEMLOADINGVIEW){
            if (!isLoad){
                if (!missing) glWidget->renderText(xdist,ydist+i,strong,font);
                if (!missing) i+= dist;
                missing = true;
                glWidget->renderText(xdist,ydist+i,"- No Static Blade Loading in Database",font);
                i+=dist;
                if (isStruct){
                    giveMsg = true;
                    tip = "Click 'New' to create a new Structural Loading...";
                }
            }
        }
        if (!isStruct){
            if (!missing) glWidget->renderText(xdist,ydist+i,strong,font);
            if (!missing) i+= dist;
            missing = true;
            glWidget->renderText(xdist,ydist+i,"- No Blade Structure in Database",font);
            i+=dist;
            if (isBlade && iView == QFEMSTRUCTVIEW){
                giveMsg = true;
                tip = "Click 'New' to create a new Blade Structure...";
            }
            if (isBlade && iView == QFEMLOADINGVIEW){
                giveMsg = true;
                tip = "Create a new Blade Structure in the 'Structural Blade Design' Tab...";
            }
        }
    }
    if (iApp == WINDFIELDMODULE){
        if (!isWind){
            if (!missing) glWidget->renderText(xdist,ydist+i,strong,font);
            if (!missing) i+= dist;
            missing = true;
            glWidget->renderText(xdist,ydist+i,"- No Windfield in Database",font);
            i+=dist;
            giveMsg = true;
            tip = "Click 'New' to create a new Windfield";
        }
    }
    if (iApp == QLLTMODULE){
        if (!isLLT){
            if (!missing) glWidget->renderText(xdist,ydist+i,strong,font);
            if (!missing) i+= dist;
            missing = true;
            glWidget->renderText(xdist,ydist+i,"- No LLT Simulation in Database",font);
            i+=dist;
            giveMsg = true;
            tip = "Click 'New' to create a new LLT Simulation";
        }
    }
    if (iApp == NOISE_MODULE){

            if (!missing) glWidget->renderText(xdist,ydist+i,strong,font);
            if (!missing) i+= dist;
            missing = true;
            glWidget->renderText(xdist,ydist+i,"- No NOISE Simulation in Database",font);
            i+=dist;
            giveMsg = true;
            tip = "Click 'NOISE' to create a new LLT Simulation";

    }
    if (iApp == BEM || iApp == QFEMMODULE || iApp == QLLTMODULE){
        if (iView == BLADEVIEW || iView == QFEMLOADINGVIEW || iView == QFEMSTRUCTVIEW || iView == QLLTGLVIEW){
            if (!isBlade){
                if (!missing) glWidget->renderText(xdist,ydist+i,strong,font);
                if (!missing) i+= dist;
                missing = true;
                glWidget->renderText(xdist,ydist+i,"- No Blade in Database",font);
                i+=dist;
                giveMsg = true;
                tip = "Click 'New' to create a new HAWT Blade";
                }
        }
    }
    if (iApp == DMS){
        if (iView == BLADEVIEW){
            if (!isVBlade){
                if (!missing) glWidget->renderText(xdist,ydist+i,strong,font);
                if (!missing) i+= dist;
                missing = true;
                glWidget->renderText(xdist,ydist+i,"- No Blade in Database",font);
                i+=dist;
                giveMsg = true;
                tip = "Click 'New' to create a new VAWT Blade";
            }
        }
    }

    if (iApp != DIRECTDESIGN &&  iApp != INVERSEDESIGN && iApp != XFOILANALYSIS && iApp != WINDFIELDMODULE){
            if (!is360Polar){
                glWidget->renderText(xdist,ydist+i,"- No 360 Polar in Database",font);
                i+=dist;
                giveMsg = true;
                tip = "Create a new 360 Polar in the 360 Polar Extrapolation Module";
            }
    }
    if (iApp != DIRECTDESIGN && iApp != INVERSEDESIGN && iApp != WINDFIELDMODULE){
            if (!isPolar){
                glWidget->renderText(xdist,ydist+i,"- No Polar in Database",font);
                i+=dist;
                giveMsg = true;
                tip = "Create a new Polar in the XFOIL Direct Analysis Module";
            }
            if (!isFoil){
                glWidget->renderText(xdist,ydist+i,"- No Airfoil in Database",font);
                i+=dist;
                giveMsg = true;
                tip = "Create a new Airfoil in the Airfoil Design Module";
            }
    }

    if (giveMsg) glWidget->renderText(xdist,ydist+i,">>> "+tip+"...",font2);

    glMatrixMode(GL_MODELVIEW);  // restore state
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glPopAttrib ();

    return giveMsg;

}

QString UnifyString(QString strong){
    strong.replace(" e","_e");
    strong.replace(" E","_E");
    strong.replace("e ","e_");
    strong.replace("E ","E_");
    strong.replace("\\- ","\\-_");
    strong.replace(" \\-","_\\-");
    strong.replace("\\+ ","\\+_");
    strong.replace(" \\+","_\\+");
    strong.replace("_","");
    strong.replace(",",".");

    return strong;
}

bool TwoDscreenMessage(int iApp, int iView, QPainter &painter, TwoDWidget *twodwidget){

    if (ObjectIsEdited) return false;

    bool isFoil = g_foilStore.size();
    bool isPolar = g_polarStore.size();
    bool is360Polar = g_360PolarStore.size();
    bool isBlade = g_rotorStore.size();
    bool isVBlade = g_verticalRotorStore.size();
    bool isBEM = g_bemdataStore.size();
    bool isTData = g_tdataStore.size();
    bool isTBEM = g_tbemdataStore.size();
    bool isCBEM = g_cbemdataStore.size();
    bool isDMS = g_dmsdataStore.size();
    bool isTDMS = g_tdmsdataStore.size();
    bool isCDMS = g_cdmsdataStore.size();
    bool isVTData = g_verticalRotorStore.size();
    bool isStruct = g_bladeStructureStore.size();
    bool isFAST = g_FASTSimulationStore.size();
    bool isWind = g_windFieldStore.size();
    bool isLLT = g_QLLTHAWTSimulationStore.size();

    bool giveMsg = false;

    QFont font(g_mainFrame->m_TextFont.family(),0.02*twodwidget->rect().width());
    QFont font2(g_mainFrame->m_TextFont.family(),0.015*twodwidget->rect().width());

    painter.setFont(font);

    int i = 0;
    int dist = 0.08*twodwidget->rect().height();
    int xdist = 0.15*twodwidget->rect().height();
    int ydist = 0.15*twodwidget->rect().height();
    QString tip;
    bool missing = false;

    painter.fillRect(twodwidget->rect(), QColor(255,255,255));

    QString strong = "Missing Objects:";




    if (iApp == FASTMODULE){
        if (!isFAST){
            if (!missing) painter.drawText(xdist, ydist+i,strong);
            if (!missing) i+=dist;
            missing = true;
            painter.drawText(xdist, ydist+i,"- No FAST Simulation in Database");
            i+=dist;
            giveMsg = true;
            tip = "Click 'New' to create a new FAST Simulation";
        }
        if (!isWind){
            if (!missing) painter.drawText(xdist, ydist+i,strong);
            if (!missing) i+=dist;
            missing = true;
            painter.drawText(xdist, ydist+i,"- No Windfield in Database");
            i+=dist;
            giveMsg = true;
            tip = "Create a new Windfield in the Windfield Module";
        }
    }
    if (iApp == QLLTMODULE && iView == QLLTGRAPHVIEW){
        if (!isLLT){
            if (!missing) painter.drawText(xdist, ydist+i,strong);
            if (!missing) i+=dist;
            missing = true;
            painter.drawText(xdist, ydist+i,"- No LLT Simulation in Database");
            i+=dist;
            giveMsg = true;
            tip = "Click 'New' to create a new LLT Simulation";
        }
        if (!isWind){
            if (!missing) painter.drawText(xdist, ydist+i,strong);
            if (!missing) i+=dist;
            missing = true;
            painter.drawText(xdist, ydist+i,"- No Windfield in Database");
            i+=dist;
            giveMsg = true;
            tip = "Create a new Windfield in the Windfield Module";
        }
    }
    if (iApp == NOISE_MODULE && iView == NOISE_BPMTE_VIEW){
    }
    if ((iApp == QFEMMODULE && iView == QFEMGRAPHVIEW) || iApp == FASTMODULE){
        if (!isStruct){
            if (!missing) painter.drawText(xdist, ydist+i,strong);
            if (!missing) i+=dist;
            missing = true;
            painter.drawText(xdist, ydist+i,"- No Blade Structure in Database");
            i+=dist;
            giveMsg = true;
            tip = "Create a new Blade Structure in the QFEM Module";
            if (iApp == QFEMMODULE && iView == QFEMGRAPHVIEW) tip = "Click 'New' to create a new Blade Structure";
        }
    }
    if (iApp == BEM){
        if (iView == BEMSIMVIEW){
            if (!isBEM){
                if (!missing) painter.drawText(xdist, ydist+i,strong);
                if (!missing) i+=dist;
                missing = true;
                painter.drawText(xdist, ydist+i,"- No BEM Simulation in Database");
                i+=dist;
                giveMsg = true;
                tip = "Click 'Define Simulation' to create a new BEM Simulation";
                }
        }
        if (iView == CHARSIMVIEW){
            if (!isCBEM){
                if (!missing) painter.drawText(xdist, ydist+i,strong);
                if (!missing) i+=dist;
                missing = true;
                painter.drawText(xdist, ydist+i,"- No Multi Parameter BEM Simulation in Database");
                i+=dist;
                giveMsg = true;
                tip = "Click 'Define Simulation' to create a new Multi Parameter Simulation";
            }
        }
        if (iView == TURBINEVIEW){
            if (!isTBEM){
                if (!missing) painter.drawText(xdist, ydist+i,strong);
                if (!missing) i+=dist;
                missing = true;
                painter.drawText(xdist, ydist+i,"- No Turbine Simulation in Database");
                i+=dist;
                giveMsg = true;
                tip = "Click 'Define Simulation' to create a new Turbine Simulation";
            }
            if (!isTData){
                if (!missing) painter.drawText(xdist, ydist+i,strong);
                if (!missing) i+=dist;
                missing = true;
                painter.drawText(xdist, ydist+i,"- No Turbine Definition in Database");
                i+=dist;
                tip = "Click 'New' to define a new Turbine";
            }
        }
    }
    if (iApp == DMS){
        if (iView == BEMSIMVIEW){
            if (!isDMS){
                if (!missing) painter.drawText(xdist, ydist+i,strong);
                if (!missing) i+=dist;
                missing = true;
                painter.drawText(xdist, ydist+i,"- No DMS Simulation in Database");
                i+=dist;
                giveMsg = true;
                tip = "Click 'Define Simulation' to create a new DMS Simulation";
            }
        }
        if (iView == CHARSIMVIEW){
            if (!isCDMS){
                if (!missing) painter.drawText(xdist, ydist+i,strong);
                if (!missing) i+=dist;
                missing = true;
                painter.drawText(xdist, ydist+i,"- No Multi Parameter DMS Simulation in Database");
                i+=dist;
                tip = "Click 'Define Simulation' to create a new Multi Parameter DMS Simulation";
            }
        }
        if (iView == TURBINEVIEW){
            if (!isTDMS){
                if (!missing) painter.drawText(xdist, ydist+i,strong);
                if (!missing) i+=dist;
                missing = true;
                painter.drawText(xdist, ydist+i,"- No Turbine Simulation in Database");
                i+=dist;
                giveMsg = true;
                tip = "Click 'Define Simulation' to create a new Turbine Simulation";
            }
            if (!isVTData){
                if (!missing) painter.drawText(xdist, ydist+i,strong);
                if (!missing) i+=dist;
                missing = true;
                painter.drawText(xdist, ydist+i,"- No Turbine Definition in Database");
                i+=dist;
                giveMsg = true;
                tip = "Click 'New' to define a new Turbine";
                }
        }
    }
    if (iApp != DIRECTDESIGN &&  iApp != INVERSEDESIGN && iApp != XFOILANALYSIS && iView != POLARVIEW){
            if (!isBlade && (iApp == BEM || iApp == FASTMODULE || iApp == QFEMMODULE || iApp == QLLTMODULE)){
                if (!missing) painter.drawText(xdist, ydist+i,strong);
                if (!missing) i+=dist;
                missing = true;
                painter.drawText(xdist, ydist+i,"- No Blade in Database");
                i+=dist;
                giveMsg = true;
                tip = "Create a new Blade in the HAWT Blade Design Module";
            }
            if (!isVBlade && iApp == DMS){
                if (!missing) painter.drawText(xdist, ydist+i,strong);
                if (!missing) i+=dist;
                missing = true;
                painter.drawText(xdist, ydist+i,"- No Blade in Database");
                i+=dist;
                giveMsg = true;
                tip = "Create a new Blade in the VAWT Blade Design Module";
            }
    }
    if (iApp != DIRECTDESIGN &&  iApp != INVERSEDESIGN && iApp != XFOILANALYSIS && iApp != NOISE_MODULE){
            if (!is360Polar){
                if (!missing) painter.drawText(xdist, ydist+i,strong);
                if (!missing) i+=dist;
                missing = true;
                painter.drawText(xdist, ydist+i,"- No 360 Polar in Database");
                i+=dist;
                giveMsg = true;
                tip = "Create a new 360 Polar in the 360 Polar Extrapolation Module";
                if (iView == POLARVIEW){
                    giveMsg = false;
                    tip = "Click 'New' to create a new 360 Polar";
                }
            }
    }
    if (iApp != DIRECTDESIGN && iApp != INVERSEDESIGN){
            if (!isPolar){
                if (!missing) painter.drawText(xdist, ydist+i,strong);
                if (!missing) i+=dist;
                missing = true;
                painter.drawText(xdist, ydist+i,"- No Polar in Database");
                i+=dist;
                giveMsg = true;
                tip = "Create a new Polar in the XFOIL Direct Analysis Module";
                if (iApp == XFOILANALYSIS) tip = "Click 'Define XFOIL Polar' to create a new 360 Polar";
            }
            if (!isFoil){
                if (!missing) painter.drawText(xdist, ydist+i,strong);
                if (!missing) i+=dist;
                missing = true;
                painter.drawText(xdist, ydist+i,"- No Airfoil in Database");
                i+=dist;
                giveMsg = true;
                tip = "Create a new Airfoil in the Airfoil Design Module";
            }
    }

    painter.setFont(font2);
    if (giveMsg) painter.drawText(xdist, ydist+i,">>> "+tip+"...");

    return giveMsg;
}


bool removeDir(const QString & dirName)
{
    bool result = true;
    QDir dir(dirName);

    if (dir.exists(dirName)) {
        Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs |
													QDir::Files, QDir::DirsFirst)) {
            if (info.isDir()) {
                result = removeDir(info.absoluteFilePath());
            }
            else {
                result = QFile::remove(info.absoluteFilePath());
            }

            if (!result) {
                return result;
            }
        }
        result = dir.rmdir(dirName);
    }
    return result;
}

void printStoreState () {
	g_windFieldStore.printState();
	g_FASTSimulationStore.printState();
	g_foilStore.printState();
	g_polarStore.printState();
	g_360PolarStore.printState();
	g_rotorStore.printState();
	g_verticalRotorStore.printState();
	g_bladeStructureStore.printState();
	g_tdataStore.printState();
	g_verttdataStore.printState();
	g_bemdataStore.printState();
	g_tbemdataStore.printState();
	g_cbemdataStore.printState();
	g_dmsdataStore.printState();
	g_tdmsdataStore.printState();
	g_cdmsdataStore.printState();
	
	qDebug() << "-";
	qDebug() << "-";
	qDebug() << "-";	
}
