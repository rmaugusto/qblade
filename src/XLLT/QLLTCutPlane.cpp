/****************************************************************************

    QLLTCutPlane Class
        Copyright (C) 2015 David Marten david.marten@tu-berlin.de

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

#include "QLLTCutPlane.h"
#include "../src/Params.h"
#include "../GlobalFunctions.h"
#include "../src/Serializer.h"
#include "QLLTModule.h"
#include "../MainFrame.h"

QLLTCutPlane::QLLTCutPlane(QString name, StorableObject *parent)
: StorableObject (name, parent)
{
    is_computed = false;
}

QLLTCutPlane* QLLTCutPlane::newBySerialize() {
    QLLTCutPlane* plane = new QLLTCutPlane ();
    plane->serialize();
    return plane;
}

void QLLTCutPlane::exportPlane(QString fileName) {

    QDate date = QDate::currentDate();
    QTime time = QTime::currentTime();
    QFile file (fileName);

    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream stream (&file);

        stream << "Export File Created with " << g_mainFrame->m_VersionName << " on "<<date.toString("dd.MM.yyyy") << " at " << time.toString("hh:mm:ss") << endl;
        stream << "Simulation Time " << m_time << " [s]" <<endl;
        stream << "Position Vector\t\tVelocity Vector " << endl;
        stream << "        X\t        Y\t        Z\t        X\t        Y\t        Z" << endl;

        for (int i=0;i<m_points.size();i++){
            for (int j=0;j<m_points.at(i).size();j++){
             stream << QString("%1\t%2\t%3\t%4\t%5\t%6").arg(m_points[i][j].x, 9).arg(m_points[i][j].y, 9).arg(m_points[i][j].z, 9).arg(m_velocities[i][j].x, 9).arg(m_velocities[i][j].y, 9).arg(m_velocities[i][j].z, 9)<<endl;
            }
        }
    }
    file.close();
}


void QLLTCutPlane::exportPlaneCSV(QString fileName) {

    QDate date = QDate::currentDate();
    QTime time = QTime::currentTime();
    QFile file (fileName);

    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream stream (&file);

        stream << "Export File Created with " << g_mainFrame->m_VersionName << " on "<<date.toString("dd.MM.yyyy") << " at " << time.toString("hh:mm:ss") << endl;
        stream << "Simulation Time " << m_time << " [s]" <<endl;
        stream << "Position Vector\t\tVelocity Vector " << endl;
        stream << "X;Y;Z;X;Y;Z" << endl;

        for (int i=0;i<m_points.size();i++){
            for (int j=0;j<m_points.at(i).size();j++){
             stream << QString("%1;%2;%3;%4;%5;%6").arg(m_points[i][j].x, 9).arg(m_points[i][j].y, 9).arg(m_points[i][j].z, 9).arg(m_velocities[i][j].x, 9).arg(m_velocities[i][j].y, 9).arg(m_velocities[i][j].z, 9)<<endl;
            }
        }
    }
    file.close();
}


void QLLTCutPlane::exportPlaneVTK(QString fileName) {

//    QDate date = QDate::currentDate();
//    QTime time = QTime::currentTime();
    QFile file (fileName);

    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream stream (&file);
//VTK LEGACY FORMAT
//        stream << "# vtk DataFile Version 2.0" << endl;
//        stream << "Export File Created with " << g_mainFrame->m_VersionName << " on "<<date.toString("dd.MM.yyyy") << " at " << time.toString("hh:mm:ss") << "\n";
//        stream << "ASCII" <<endl;
//        stream << "DATASET UNSTRUCTURED_GRID" << endl;
//        stream << "POINTS " <<QString().number(m_points.size()*m_points.at(0).size(),'f',0)<<" float"<< endl;

//        for (int i=0;i<m_points.size();i++){
//            for (int j=0;j<m_points.at(i).size();j++){
//             stream << QString("%1 %2 %3").arg(m_points[i][j].x, 4).arg(m_points[i][j].y, 4).arg(m_points[i][j].z, 4)<<endl;
//            }
//        }

//        stream << endl;
//        stream << "CELLS " <<QString().number((m_points.size()-1)*(m_points.at(0).size()-1),'f',0)<<" "<<QString().number((m_points.size()-1)*(m_points.at(0).size()-1)*5,'f',0)<< endl;

//        for (int i=0;i<m_points.size()-1;i++){
//            for (int j=0;j<m_points.at(i).size()-1;j++){
//             stream << "4 "<<QString().number(i+j*m_points.at(i).size(),'f',0)<<" "<<QString().number(i+j*m_points.at(i).size()+1,'f',0)<<" "<<QString().number(i+j*m_points.at(i).size()+m_points.size()+1,'f',0)<<" "<<QString().number(i+j*m_points.at(i).size()+m_points.size(),'f',0)<<endl;
//            }
//        }

//        stream << "CELL_TYPES " <<QString().number((m_points.size()-1)*(m_points.at(0).size()-1),'f',0)<< endl;
//        for (int i=0;i<m_points.size()-1;i++){
//            for (int j=0;j<m_points.at(i).size()-1;j++){
//             stream << "9"<<endl;
//            }
//        }

//        stream <<endl;
//        stream << "POINT_DATA " <<QString().number((m_points.size())*(m_points.at(0).size()),'f',0)<< endl;
//        stream << "VECTORS velocity float"<<endl;

//        for (int i=0;i<m_points.size();i++){
//            for (int j=0;j<m_points.at(i).size();j++){
//             stream << QString("%1 %2 %3").arg(m_velocities[i][j].x, 4).arg(m_velocities[i][j].y, 4).arg(m_velocities[i][j].z, 4)<<endl;
//            }
//        }


        stream << "<?xml version=\"1.0\"?>" <<endl;
        stream << "<VTKFile type=\"UnstructuredGrid\" version=\"0.1\" byte_order=\"LittleEndian\" compressor=\"vtkZLibDataCompressor\">" <<endl;
        stream << "<UnstructuredGrid>" <<endl;
        stream << "<Piece NumberOfPoints=\""<<QString().number(m_points.size()*m_points.at(0).size(),'f',0)<<"\" NumberOfCells=\"" << QString().number((m_points.size()-1)*(m_points.at(0).size()-1),'f',0)<<"\">" <<endl;
        stream << "<Points>" <<endl;
        stream << "<DataArray type=\"Float64\" NumberOfComponents=\"3\" format=\"ascii\">" <<endl;
        for (int i=0;i<m_points.size();i++){
            for (int j=0;j<m_points.at(i).size();j++){
             stream << QString("%1 %2 %3").arg(m_points[i][j].x, 4).arg(m_points[i][j].y, 4).arg(m_points[i][j].z, 4)<<endl;
            }
        }
        stream << "</DataArray>" <<endl;
        stream << "</Points>" <<endl;
        stream << "<Cells>" <<endl;
        stream << "<DataArray type=\"Int32\" Name=\"connectivity\" format=\"ascii\">" <<endl;
        for (int i=0;i<m_points.size()-1;i++){
            for (int j=0;j<m_points.at(i).size()-1;j++){
             stream <<QString().number(j+i*m_points.at(i).size(),'f',0)<<" "<<QString().number(j+i*m_points.at(i).size()+1,'f',0)<<" "<<QString().number(j+(i+1)*m_points.at(i).size()+1,'f',0)<<" "<<QString().number(j+(i+1)*m_points.at(i).size(),'f',0)<<endl;
            }
        }
        stream << "</DataArray>" <<endl;
        stream << "<DataArray type=\"Int32\" Name=\"offsets\" format=\"ascii\">" <<endl;
        int offset = 4;
        for (int i=0;i<m_points.size()-1;i++){
            for (int j=0;j<m_points.at(i).size()-1;j++){
             stream <<QString().number(offset,'f',0)<<endl;
             offset+=4;
            }
        }
        stream << "</DataArray>" <<endl;
        stream << "<DataArray type=\"UInt8\" Name=\"types\" format=\"ascii\">" <<endl;
        for (int i=0;i<m_points.size()-1;i++){
            for (int j=0;j<m_points.at(i).size()-1;j++){
             stream << "9"<<endl;
            }
        }
        stream << "</DataArray>" <<endl;
        stream << "</Cells>" <<endl;
        stream << "<PointData Vectors=\"velocity\">" <<endl;
        stream << "<DataArray type=\"Float64\" Name=\"velocity\" NumberOfComponents=\"3\" format=\"ascii\">" <<endl;
        for (int i=0;i<m_points.size();i++){
            for (int j=0;j<m_points.at(i).size();j++){
             stream << QString("%1 %2 %3").arg(m_velocities[i][j].x, 4).arg(m_velocities[i][j].y, 4).arg(m_velocities[i][j].z, 4)<<endl;
            }
        }
        stream << "</DataArray>" <<endl;
        stream << "</PointData>" <<endl;
        stream << "</Piece>" <<endl;
        stream << "</UnstructuredGrid>" <<endl;
        stream << "</VTKFile>" <<endl;

    }
    file.close();
}

void QLLTCutPlane::serialize() {
    StorableObject::serialize();

    g_serializer.readOrWriteDouble(&m_length);
    g_serializer.readOrWriteDouble(&m_width);
    g_serializer.readOrWriteDouble(&m_X);
    g_serializer.readOrWriteDouble(&m_Y);
    g_serializer.readOrWriteDouble(&m_Z);
    g_serializer.readOrWriteDouble(&m_X_rot);
    g_serializer.readOrWriteDouble(&m_Y_rot);
    g_serializer.readOrWriteDouble(&m_Z_rot);
    g_serializer.readOrWriteDouble(&m_time);
    g_serializer.readOrWriteDouble(&m_meanVelocity);



    m_curVelocity.serialize();
    m_Hub.serialize();
    m_Axis.serialize();

    g_serializer.readOrWriteInt(&m_X_res);
    g_serializer.readOrWriteInt(&m_Y_res);

    g_serializer.readOrWriteBool(&is_computed);

    if (g_serializer.isReadMode()) {
        int n = g_serializer.readInt();
        for (int i = 0; i < n; ++i) {
            QList<CVector> list;
            int m = g_serializer.readInt();
            for (int j = 0; j < m; ++j) {
                CVector vec;
                vec.serialize();
                list.append(vec);
            }
            m_velocities.append(list);
        }
    } else {
        g_serializer.writeInt(m_velocities.size());
        for (int i = 0; i < m_velocities.size(); ++i) {
            g_serializer.writeInt(m_velocities.at(i).size());
            for (int j = 0; j < m_velocities.at(i).size(); ++j) {
                m_velocities[i][j].serialize();
            }
        }
    }

    if (g_serializer.isReadMode()) {
        int n = g_serializer.readInt();
        for (int i = 0; i < n; ++i) {
            QList<CVector> list;
            int m = g_serializer.readInt();
            for (int j = 0; j < m; ++j) {
                CVector vec;
                vec.serialize();
                list.append(vec);
            }
            m_points.append(list);
        }
    } else {
        g_serializer.writeInt(m_points.size());
        for (int i = 0; i < m_points.size(); ++i) {
            g_serializer.writeInt(m_points.at(i).size());
            for (int j = 0; j < m_points.at(i).size(); ++j) {
                m_points[i][j].serialize();
            }
        }
    }

}

void QLLTCutPlane::Update(){

    for (int i=0;i<m_points.size();i++){
    m_points[i].clear();
    m_velocities[i].clear();
    }
    m_points.clear();
    m_velocities.clear();

    QList< CVector > line;

    for (int i=0;i<m_X_res;i++){
        line.clear();
        for (int j=0;j<m_Y_res;j++){
        CVector vec;
        vec.Set(m_X-m_length/2+m_length/(m_X_res-1)*i,m_Y-m_width/2+m_width/(m_Y_res-1)*j,m_Z);
        if (m_X_res == 1) vec.x = m_X;
        if (m_Y_res == 1) vec.y = m_Y;
        line.append(vec);
        }
        m_points.append(line);
        m_velocities.append(line);
    }

    CVector O;
    O.x = m_X;
    O.y = m_Y;
    O.z = m_Z;

    for (int i=0;i<m_points.size();i++){
        for (int j=0;j<m_points.at(i).size();j++){
            m_points[i][j].Rotate(O,CVector(1,0,0),m_X_rot);
            m_points[i][j].Rotate(O,CVector(0,1,0),m_Y_rot);
            m_points[i][j].Rotate(O,CVector(0,0,1),m_Z_rot);
        }
    }

}
void QLLTCutPlane::drawFrame(){

    glColor3d(0,0,0);
    glLineWidth(1);
    glBegin(GL_LINE_STRIP);
    {
        glVertex3d(m_points.at(0).at(0).x,m_points.at(0).at(0).y,m_points.at(0).at(0).z);
        glVertex3d(m_points.at(m_points.size()-1).at(0).x,m_points.at(m_points.size()-1).at(0).y,m_points.at(m_points.size()-1).at(0).z);
        glVertex3d(m_points.at(m_points.size()-1).at(m_points.at(0).size()-1).x,m_points.at(m_points.size()-1).at(m_points.at(0).size()-1).y,m_points.at(m_points.size()-1).at(m_points.at(0).size()-1).z);
        glVertex3d(m_points.at(0).at(m_points.at(0).size()-1).x,m_points.at(0).at(m_points.at(0).size()-1).y,m_points.at(0).at(m_points.at(0).size()-1).z);
        glVertex3d(m_points.at(0).at(0).x,m_points.at(0).at(0).y,m_points.at(0).at(0).z);
    }
    glEnd();

}

CVector QLLTCutPlane::CorrespondingAxisPoint(CVector Point, CVector Line1, CVector Line2)
{
     CVector v = Line1 - Line2;
     CVector w = Point - Line1;

     double c1 = w.dot(v);
     double c2 = v.dot(v);
     double b = c1 / c2;

     CVector Pb = Line1 + v * b;
     return Pb;
}

void QLLTCutPlane::render(bool redblue, bool vectors, int component, double meanFrac){

    hsv hs;
    hs.s = 1;
    hs.v = 1;

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0, 0);

     if (!is_computed){

    for (int i=0;i<m_points.size();i++){
        for (int j=0;j<m_points.at(i).size();j++){
    glBegin(GL_POINTS);

    glVertex3d(m_points.at(i).at(j).x, m_points.at(i).at(j).y, m_points.at(i).at(j).z);

    glEnd();
        }
    }
     }
    else{
         double  range = 2 * m_meanVelocity * meanFrac;

//         if (redblue) range = range/2;


         CVector PointOnAxis, RadialVector, TangentVector;

         double vel;
         double fac = 1 + 10*(1-meanFrac);
            for (int i=0;i<m_points.size()-1;i++){
                for (int j=0;j<m_points.at(i).size()-1;j++){
                    glBegin(GL_QUADS);

                    if (!redblue){
                        if (component == 0){
                            double vel = (m_velocities[i][j].VAbs()/m_curVelocity.VAbs());
                            if (vel > 1) hs.h = 112-(vel-1)*fac * 112;
                            else hs.h = 112 + (1-vel)*fac*112;
                            if (hs.h>225)hs.h = 225;
                            else if (hs.h < 0) hs.h = 0;
                            glColor4d(hsv2rgb(hs).r, hsv2rgb(hs).g, hsv2rgb(hs).b,0.7);
                        }
                        else if (component == 1){
                            hs.h = (m_velocities[i][j].x*(-1)+m_curVelocity.x)/range*225+112;
                            if (hs.h>225)hs.h = 225;
                            else if (hs.h < 0) hs.h = 0;
                            glColor4d(hsv2rgb(hs).r, hsv2rgb(hs).g, hsv2rgb(hs).b,0.7);
                        }
                        else if (component == 2){
                            hs.h = (m_velocities[i][j].y*(-1)+m_curVelocity.y)/range*225+112;
                            if (hs.h>225)hs.h = 225;
                            else if (hs.h < 0) hs.h = 0;
                            glColor4d(hsv2rgb(hs).r, hsv2rgb(hs).g, hsv2rgb(hs).b,0.7);
                        }
                        else if (component == 3){
                            hs.h = (m_velocities[i][j].z*(-1)+m_curVelocity.z)/range*225+112;
                            if (hs.h>225)hs.h = 225;
                            else if (hs.h < 0) hs.h = 0;
                            glColor4d(hsv2rgb(hs).r, hsv2rgb(hs).g, hsv2rgb(hs).b,0.7);
                        }
                        else if (component == 4){
                            hs.h = (m_curVelocity.dot(m_Axis)-m_velocities[i][j].dot(m_Axis))/range*225+112;
                            if (hs.h>225)hs.h = 225;
                            else if (hs.h < 0) hs.h = 0;
                            glColor4d(hsv2rgb(hs).r, hsv2rgb(hs).g, hsv2rgb(hs).b,0.7);
                        }
                        else if (component == 5){

                            PointOnAxis = CorrespondingAxisPoint(m_points[i][j],m_Hub,m_Hub+m_Axis);
                            RadialVector = CVector(m_points[i][j]-PointOnAxis);
                            TangentVector = m_Axis * RadialVector;
                            TangentVector.Normalize();

                            hs.h = (m_curVelocity.dot(TangentVector)-m_velocities[i][j].dot(TangentVector))/range*225+112;
                            if (hs.h>225)hs.h = 225;
                            else if (hs.h < 0) hs.h = 0;
                            glColor4d(hsv2rgb(hs).r, hsv2rgb(hs).g, hsv2rgb(hs).b,0.7);
                        }
                        else if (component == 6){
                            PointOnAxis = CorrespondingAxisPoint(m_points[i][j],m_Hub,m_Hub+m_Axis);
                            RadialVector = CVector(m_points[i][j]-PointOnAxis);
                            RadialVector.Normalize();

                            hs.h = (m_curVelocity.dot(RadialVector)-m_velocities[i][j].dot(RadialVector))/range*225+112;
                            if (hs.h>225)hs.h = 225;
                            else if (hs.h < 0) hs.h = 0;
                            glColor4d(hsv2rgb(hs).r, hsv2rgb(hs).g, hsv2rgb(hs).b,0.7);
                        }
                    }
                    else{
                        if (component == 0){
                            vel = m_velocities[i][j].VAbs()/m_curVelocity.VAbs();
                            if (vel > 1) glColor4d(1,0,0,(vel-1)*fac);
                            else glColor4d(0,0,1,(1-vel)*fac);
                        }
                        if (component == 1){
                            vel = (m_curVelocity.x*(-1)+m_velocities[i][j].x)/range;
                            if (vel > 0) glColor4d(1,0,0,vel);
                            else glColor4d(0,0,1,fabs(vel));
                        }
                        if (component == 2){
                            vel = (m_curVelocity.y*(-1)+m_velocities[i][j].y)/range;
                            if (vel > 0) glColor4d(1,0,0,vel);
                            else glColor4d(0,0,1,fabs(vel));
                        }
                        if (component == 3){
                            vel = (m_curVelocity.z*(-1)+m_velocities[i][j].z)/range;
                            if (vel > 0) glColor4d(1,0,0,vel);
                            else glColor4d(0,0,1,fabs(vel));
                        }
                        if (component == 4){
                            vel = (m_curVelocity.dot(m_Axis)*(-1)+m_velocities[i][j].dot(m_Axis))/range;
                            if (vel > 0) glColor4d(1,0,0,vel);
                            else glColor4d(0,0,1,fabs(vel));
                        }
                        if (component == 5){
                            PointOnAxis = CorrespondingAxisPoint(m_points[i][j],m_Hub,m_Hub+m_Axis);
                            RadialVector = CVector(m_points[i][j]-PointOnAxis);
                            TangentVector = m_Axis * RadialVector;
                            TangentVector.Normalize();

                            vel = (m_curVelocity.dot(TangentVector)*(-1)+m_velocities[i][j].dot(TangentVector))/range;
                            if (vel > 0) glColor4d(1,0,0,vel);
                            else glColor4d(0,0,1,fabs(vel));
                        }
                        if (component == 6){
                            PointOnAxis = CorrespondingAxisPoint(m_points[i][j],m_Hub,m_Hub+m_Axis);
                            RadialVector = CVector(m_points[i][j]-PointOnAxis);
                            RadialVector.Normalize();

                            vel = (m_curVelocity.dot(RadialVector)*(-1)+m_velocities[i][j].dot(RadialVector))/range;
                            if (vel > 0) glColor4d(1,0,0,vel);
                            else glColor4d(0,0,1,fabs(vel));
                        }
                    }
                    glVertex3d(m_points.at(i).at(j).x, m_points.at(i).at(j).y, m_points.at(i).at(j).z);


                    if (!redblue){
                        if (component == 0){
                            double vel = (m_velocities[i+1][j].VAbs()/m_curVelocity.VAbs());
                            if (vel > 1) hs.h = 112-(vel-1)*fac * 112;
                            else hs.h = 112 + (1-vel)*fac * 112;
                            if (hs.h>225)hs.h = 225;
                            else if (hs.h < 0) hs.h = 0;
                            glColor4d(hsv2rgb(hs).r, hsv2rgb(hs).g, hsv2rgb(hs).b,0.7);
                        }
                        else if (component == 1){
                            hs.h = (m_velocities[i+1][j].x*(-1)+m_curVelocity.x)/range*225+112;
                            if (hs.h>225)hs.h = 225;
                            else if (hs.h < 0) hs.h = 0;
                            glColor4d(hsv2rgb(hs).r, hsv2rgb(hs).g, hsv2rgb(hs).b,0.7);
                        }
                        else if (component == 2){
                            hs.h = (m_velocities[i+1][j].y*(-1)+m_curVelocity.y)/range*225+112;
                            if (hs.h>225)hs.h = 225;
                            else if (hs.h < 0) hs.h = 0;
                            glColor4d(hsv2rgb(hs).r, hsv2rgb(hs).g, hsv2rgb(hs).b,0.7);
                        }
                        else if (component == 3){
                            hs.h = (m_velocities[i+1][j].z*(-1)+m_curVelocity.z)/range*225+112;
                            if (hs.h>225)hs.h = 225;
                            else if (hs.h < 0) hs.h = 0;
                            glColor4d(hsv2rgb(hs).r, hsv2rgb(hs).g, hsv2rgb(hs).b,0.7);
                        }
                        else if (component == 4){
                            hs.h = (m_curVelocity.dot(m_Axis)-m_velocities[i+1][j].dot(m_Axis))/range*225+112;
                            if (hs.h>225)hs.h = 225;
                            else if (hs.h < 0) hs.h = 0;
                            glColor4d(hsv2rgb(hs).r, hsv2rgb(hs).g, hsv2rgb(hs).b,0.7);
                        }
                        else if (component == 5){

                            PointOnAxis = CorrespondingAxisPoint(m_points[i+1][j],m_Hub,m_Hub+m_Axis);
                            RadialVector = CVector(m_points[i+1][j]-PointOnAxis);
                            TangentVector = m_Axis * RadialVector;
                            TangentVector.Normalize();

                            hs.h = (m_curVelocity.dot(TangentVector)-m_velocities[i+1][j].dot(TangentVector))/range*225+112;
                            if (hs.h>225)hs.h = 225;
                            else if (hs.h < 0) hs.h = 0;
                            glColor4d(hsv2rgb(hs).r, hsv2rgb(hs).g, hsv2rgb(hs).b,0.7);
                        }
                        else if (component == 6){
                            PointOnAxis = CorrespondingAxisPoint(m_points[i+1][j],m_Hub,m_Hub+m_Axis);
                            RadialVector = CVector(m_points[i+1][j]-PointOnAxis);
                            RadialVector.Normalize();

                            hs.h = (m_curVelocity.dot(RadialVector)-m_velocities[i+1][j].dot(RadialVector))/range*225+112;
                            if (hs.h>225)hs.h = 225;
                            else if (hs.h < 0) hs.h = 0;
                            glColor4d(hsv2rgb(hs).r, hsv2rgb(hs).g, hsv2rgb(hs).b,0.7);
                        }
                    }
                    else{
                        if (component == 0){
                            vel = m_velocities[i+1][j].VAbs()/m_curVelocity.VAbs();
                            if (vel > 1) glColor4d(1,0,0,(vel-1)*fac);
                            else glColor4d(0,0,1,(1-vel)*fac);
                        }
                        if (component == 1){
                            vel = (m_curVelocity.x*(-1)+m_velocities[i+1][j].x)/range;
                            if (vel > 0) glColor4d(1,0,0,vel);
                            else glColor4d(0,0,1,fabs(vel));
                        }
                        if (component == 2){
                            vel = (m_curVelocity.y*(-1)+m_velocities[i+1][j].y)/range;
                            if (vel > 0) glColor4d(1,0,0,vel);
                            else glColor4d(0,0,1,fabs(vel));
                        }
                        if (component == 3){
                            vel = (m_curVelocity.z*(-1)+m_velocities[i+1][j].z)/range;
                            if (vel > 0) glColor4d(1,0,0,vel);
                            else glColor4d(0,0,1,fabs(vel));
                        }
                        if (component == 4){
                            vel = (m_curVelocity.dot(m_Axis)*(-1)+m_velocities[i+1][j].dot(m_Axis))/range;
                            if (vel > 0) glColor4d(1,0,0,vel);
                            else glColor4d(0,0,1,fabs(vel));
                        }
                        if (component == 5){
                            PointOnAxis = CorrespondingAxisPoint(m_points[i+1][j],m_Hub,m_Hub+m_Axis);
                            RadialVector = CVector(m_points[i+1][j]-PointOnAxis);
                            TangentVector = m_Axis * RadialVector;
                            TangentVector.Normalize();

                            vel = (m_curVelocity.dot(TangentVector)*(-1)+m_velocities[i+1][j].dot(TangentVector))/range;
                            if (vel > 0) glColor4d(1,0,0,vel);
                            else glColor4d(0,0,1,fabs(vel));
                        }
                        if (component == 6){
                            PointOnAxis = CorrespondingAxisPoint(m_points[i+1][j],m_Hub,m_Hub+m_Axis);
                            RadialVector = CVector(m_points[i+1][j]-PointOnAxis);
                            RadialVector.Normalize();

                            vel = (m_curVelocity.dot(RadialVector)*(-1)+m_velocities[i+1][j].dot(RadialVector))/range;
                            if (vel > 0) glColor4d(1,0,0,vel);
                            else glColor4d(0,0,1,fabs(vel));
                        }
                    }
                    glVertex3d(m_points.at(i+1).at(j).x, m_points.at(i+1).at(j).y, m_points.at(i+1).at(j).z);


                    if (!redblue){
                        if (component == 0){
                            double vel = (m_velocities[i+1][j+1].VAbs()/m_curVelocity.VAbs());
                            if (vel > 1) hs.h = 112-(vel-1)*fac * 112;
                            else hs.h = 112 + (1-vel)*fac * 112;
                            if (hs.h>225)hs.h = 225;
                            else if (hs.h < 0) hs.h = 0;
                            glColor4d(hsv2rgb(hs).r, hsv2rgb(hs).g, hsv2rgb(hs).b,0.7);
                        }
                        else if (component == 1){
                            hs.h = (m_velocities[i+1][j+1].x*(-1)+m_curVelocity.x)/range*225+112;
                            if (hs.h>225)hs.h = 225;
                            else if (hs.h < 0) hs.h = 0;
                            glColor4d(hsv2rgb(hs).r, hsv2rgb(hs).g, hsv2rgb(hs).b,0.7);
                        }
                        else if (component == 2){
                            hs.h = (m_velocities[i+1][j+1].y*(-1)+m_curVelocity.y)/range*225+112;
                            if (hs.h>225)hs.h = 225;
                            else if (hs.h < 0) hs.h = 0;
                            glColor4d(hsv2rgb(hs).r, hsv2rgb(hs).g, hsv2rgb(hs).b,0.7);
                        }
                        else if (component == 3){
                            hs.h = (m_velocities[i+1][j+1].z*(-1)+m_curVelocity.z)/range*225+112;
                            if (hs.h>225)hs.h = 225;
                            else if (hs.h < 0) hs.h = 0;
                            glColor4d(hsv2rgb(hs).r, hsv2rgb(hs).g, hsv2rgb(hs).b,0.7);
                        }
                        else if (component == 4){

                            hs.h = (m_curVelocity.dot(m_Axis)-m_velocities[i+1][j+1].dot(m_Axis))/range*225+112;
                            if (hs.h>225)hs.h = 225;
                            else if (hs.h < 0) hs.h = 0;
                            glColor4d(hsv2rgb(hs).r, hsv2rgb(hs).g, hsv2rgb(hs).b,0.7);
                        }
                        else if (component == 5){

                            PointOnAxis = CorrespondingAxisPoint(m_points[i+1][j+1],m_Hub,m_Hub+m_Axis);
                            RadialVector = CVector(m_points[i+1][j+1]-PointOnAxis);
                            TangentVector = m_Axis * RadialVector;
                            TangentVector.Normalize();

                            hs.h = (m_curVelocity.dot(TangentVector)-m_velocities[i+1][j+1].dot(TangentVector))/range*225+112;
                            if (hs.h>225)hs.h = 225;
                            else if (hs.h < 0) hs.h = 0;
                            glColor4d(hsv2rgb(hs).r, hsv2rgb(hs).g, hsv2rgb(hs).b,0.7);
                        }
                        else if (component == 6){
                            PointOnAxis = CorrespondingAxisPoint(m_points[i+1][j+1],m_Hub,m_Hub+m_Axis);
                            RadialVector = CVector(m_points[i+1][j+1]-PointOnAxis);
                            RadialVector.Normalize();

                            hs.h = (m_curVelocity.dot(RadialVector)-m_velocities[i+1][j+1].dot(RadialVector))/range*225+112;
                            if (hs.h>225)hs.h = 225;
                            else if (hs.h < 0) hs.h = 0;
                            glColor4d(hsv2rgb(hs).r, hsv2rgb(hs).g, hsv2rgb(hs).b,0.7);
                        }
                    }
                    else{
                        if (component == 0){
                            vel = m_velocities[i+1][j+1].VAbs()/m_curVelocity.VAbs();
                            if (vel > 1) glColor4d(1,0,0,(vel-1)*fac);
                            else glColor4d(0,0,1,(1-vel)*fac);
                        }
                        if (component == 1){
                            vel = (m_curVelocity.x*(-1)+m_velocities[i+1][j+1].x)/range;
                            if (vel > 0) glColor4d(1,0,0,vel);
                            else glColor4d(0,0,1,fabs(vel));
                        }
                        if (component == 2){
                            vel = (m_curVelocity.y*(-1)+m_velocities[i+1][j+1].y)/range;
                            if (vel > 0) glColor4d(1,0,0,vel);
                            else glColor4d(0,0,1,fabs(vel));
                        }
                        if (component == 3){
                            vel = (m_curVelocity.z*(-1)+m_velocities[i+1][j+1].z)/range;
                            if (vel > 0) glColor4d(1,0,0,vel);
                            else glColor4d(0,0,1,fabs(vel));
                        }
                        if (component == 4){
                            vel = (m_curVelocity.dot(m_Axis)*(-1)+m_velocities[i+1][j+1].dot(m_Axis))/range;
                            if (vel > 0) glColor4d(1,0,0,vel);
                            else glColor4d(0,0,1,fabs(vel));
                        }
                        if (component == 5){
                            PointOnAxis = CorrespondingAxisPoint(m_points[i+1][j+1],m_Hub,m_Hub+m_Axis);
                            RadialVector = CVector(m_points[i+1][j+1]-PointOnAxis);
                            TangentVector = m_Axis * RadialVector;
                            TangentVector.Normalize();

                            vel = (m_curVelocity.dot(TangentVector)*(-1)+m_velocities[i+1][j+1].dot(TangentVector))/range;
                            if (vel > 0) glColor4d(1,0,0,vel);
                            else glColor4d(0,0,1,fabs(vel));
                        }
                        if (component == 6){
                            PointOnAxis = CorrespondingAxisPoint(m_points[i+1][j+1],m_Hub,m_Hub+m_Axis);
                            RadialVector = CVector(m_points[i+1][j+1]-PointOnAxis);
                            RadialVector.Normalize();

                            vel = (m_curVelocity.dot(RadialVector)*(-1)+m_velocities[i+1][j+1].dot(RadialVector))/range;
                            if (vel > 0) glColor4d(1,0,0,vel);
                            else glColor4d(0,0,1,fabs(vel));
                        }
                    }
                    glVertex3d(m_points.at(i+1).at(j+1).x, m_points.at(i+1).at(j+1).y, m_points.at(i+1).at(j+1).z);


                    if (!redblue){
                        if (component == 0){
                            double vel = (m_velocities[i][j+1].VAbs()/m_curVelocity.VAbs());
                            if (vel > 1) hs.h = 112-(vel-1)*fac * 112;
                            else hs.h = 112 + (1-vel)*fac * 112;
                            if (hs.h>225)hs.h = 225;
                            else if (hs.h < 0) hs.h = 0;
                            glColor4d(hsv2rgb(hs).r, hsv2rgb(hs).g, hsv2rgb(hs).b,0.7);
                        }
                        else if (component == 1){
                            hs.h = (m_velocities[i][j+1].x*(-1)+m_curVelocity.x)/range*225+112;
                            if (hs.h>225)hs.h = 225;
                            else if (hs.h < 0) hs.h = 0;
                            glColor4d(hsv2rgb(hs).r, hsv2rgb(hs).g, hsv2rgb(hs).b,0.7);
                        }
                        else if (component == 2){
                            hs.h = (m_velocities[i][j+1].y*(-1)+m_curVelocity.y)/range*225+112;
                            if (hs.h>225)hs.h = 225;
                            else if (hs.h < 0) hs.h = 0;
                            glColor4d(hsv2rgb(hs).r, hsv2rgb(hs).g, hsv2rgb(hs).b,0.7);
                        }
                        else if (component == 3){
                            hs.h = (m_velocities[i][j+1].z*(-1)+m_curVelocity.z)/range*225+112;
                            if (hs.h>225)hs.h = 225;
                            else if (hs.h < 0) hs.h = 0;
                            glColor4d(hsv2rgb(hs).r, hsv2rgb(hs).g, hsv2rgb(hs).b,0.7);
                        }
                        else if (component == 4){

                            hs.h = (m_curVelocity.dot(m_Axis)-m_velocities[i][j+1].dot(m_Axis))/range*225+112;
                            if (hs.h>225)hs.h = 225;
                            else if (hs.h < 0) hs.h = 0;
                            glColor4d(hsv2rgb(hs).r, hsv2rgb(hs).g, hsv2rgb(hs).b,0.7);
                        }
                        else if (component == 5){

                            PointOnAxis = CorrespondingAxisPoint(m_points.at(i).at(j+1),m_Hub,m_Hub+m_Axis);
                            RadialVector = CVector(m_points[i][j+1]-PointOnAxis);
                            TangentVector = m_Axis * RadialVector;
                            TangentVector.Normalize();

                            hs.h = (m_curVelocity.dot(TangentVector)-m_velocities[i][j+1].dot(TangentVector))/range*225+112;
                            if (hs.h>225)hs.h = 225;
                            else if (hs.h < 0) hs.h = 0;
                            glColor4d(hsv2rgb(hs).r, hsv2rgb(hs).g, hsv2rgb(hs).b,0.7);
                        }
                        else if (component == 6){
                            PointOnAxis = CorrespondingAxisPoint(m_points.at(i).at(j+1),m_Hub,m_Hub+m_Axis);
                            RadialVector = CVector(m_points[i][j+1]-PointOnAxis);
                            RadialVector.Normalize();

                            hs.h = (m_curVelocity.dot(RadialVector)-m_velocities[i][j+1].dot(RadialVector))/range*225+112;
                            if (hs.h>225)hs.h = 225;
                            else if (hs.h < 0) hs.h = 0;
                            glColor4d(hsv2rgb(hs).r, hsv2rgb(hs).g, hsv2rgb(hs).b,0.7);
                        }
                    }
                    else{
                        if (component == 0){
                            vel = m_velocities[i][j+1].VAbs()/m_curVelocity.VAbs();
                            if (vel > 1) glColor4d(1,0,0,(vel-1)*fac);
                            else glColor4d(0,0,1,(1-vel)*fac);
                        }
                        if (component == 1){
                            vel = (m_curVelocity.x*(-1)+m_velocities[i][j+1].x)/range;
                            if (vel > 0) glColor4d(1,0,0,vel);
                            else glColor4d(0,0,1,fabs(vel));
                        }
                        if (component == 2){
                            vel = (m_curVelocity.y*(-1)+m_velocities[i][j+1].y)/range;
                            if (vel > 0) glColor4d(1,0,0,vel);
                            else glColor4d(0,0,1,fabs(vel));
                        }
                        if (component == 3){
                            vel = (m_curVelocity.z*(-1)+m_velocities[i][j+1].z)/range;
                            if (vel > 0) glColor4d(1,0,0,vel);
                            else glColor4d(0,0,1,fabs(vel));
                        }
                        if (component == 4){
                            vel = (m_curVelocity.dot(m_Axis)*(-1)+m_velocities[i][j+1].dot(m_Axis))/range;
                            if (vel > 0) glColor4d(1,0,0,vel);
                            else glColor4d(0,0,1,fabs(vel));
                        }
                        if (component == 5){
                            PointOnAxis = CorrespondingAxisPoint(m_points[i][j+1],m_Hub,m_Hub+m_Axis);
                            RadialVector = CVector(m_points[i][j+1]-PointOnAxis);
                            TangentVector = m_Axis * RadialVector;
                            TangentVector.Normalize();

                            vel = (m_curVelocity.dot(TangentVector)*(-1)+m_velocities[i][j+1].dot(TangentVector))/range;
                            if (vel > 0) glColor4d(1,0,0,vel);
                            else glColor4d(0,0,1,fabs(vel));
                        }
                        if (component == 6){
                            PointOnAxis = CorrespondingAxisPoint(m_points[i][j+1],m_Hub,m_Hub+m_Axis);
                            RadialVector = CVector(m_points[i][j+1]-PointOnAxis);
                            RadialVector.Normalize();

                            vel = (m_curVelocity.dot(RadialVector)*(-1)+m_velocities[i][j+1].dot(RadialVector))/range;
                            if (vel > 0) glColor4d(1,0,0,vel);
                            else glColor4d(0,0,1,fabs(vel));
                        }
                    }
                    glVertex3d(m_points.at(i).at(j+1).x, m_points.at(i).at(j+1).y, m_points.at(i).at(j+1).z);

                    glEnd();
                }
            }

            if (vectors){
                glDisable(GL_DEPTH_TEST);
                double norm;
                CVector V;
                glLineWidth(0.5);
                glPointSize(0.5);
                for (int i=0;i<m_points.size()-1;i++){
                    for (int j=0;j<m_points.at(i).size()-1;j++){
                        glBegin(GL_LINES);
                        glPolygonOffset(1.0, 1.0);

                        V = m_velocities.at(i).at(j);
                        norm = V.VAbs();
                        norm *= 1 / m_meanVelocity;
                        norm = pow(norm,2);
                        if (norm>2){
                            norm =2;
                        }
                        V.Normalize();
                        if (m_width/m_Y_res < m_length / m_X_res)  V *= m_width/m_Y_res;
                        else V *= m_length/m_X_res;
                        glColor4d(0,0,0,1);

                        glVertex3d(m_points.at(i).at(j).x, m_points.at(i).at(j).y, m_points.at(i).at(j).z);

                        glVertex3d(m_points.at(i).at(j).x+V.x*norm, m_points.at(i).at(j).y+V.y*norm, m_points.at(i).at(j).z+V.z*norm);


                        glEnd();


                    }
                }
                glEnable(GL_DEPTH_TEST);
            }
    }
}
