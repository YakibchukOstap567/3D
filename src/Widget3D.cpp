#include "Widget3D.h"
#include "string"
#include "fstream"
#include "sstream"

using namespace std;

void Widget3D::cubeCreator(double l)
{
	points.clear();
	triangles.clear();
	//-l/2 l/2
	points.push_back({ -l / 2,-l / 2,-l / 2 });
	points.push_back({ l / 2,-l / 2,-l / 2 });
	points.push_back({ l / 2,l / 2,-l / 2 });
	points.push_back({ -l / 2,l / 2,-l / 2 });
	points.push_back({ -l / 2,-l / 2,l / 2 });
	points.push_back({ l / 2,-l / 2,l / 2 });
	points.push_back({ l / 2,l / 2,l / 2 });
	points.push_back({ -l / 2,l / 2,l / 2 });

	addTriangle(0,1,4);
	addTriangle(1, 5, 4);
	addTriangle(1, 2, 6);
	addTriangle(1, 6, 5);
	addTriangle(2, 6, 7);
	addTriangle(2, 3, 7);
	addTriangle(0, 3, 7);
	addTriangle(0, 7, 4);
	addTriangle(4, 5, 6);
	addTriangle(4, 6, 7);
	//zabudol som este jednu stranu kocky
	addTriangle(0, 1, 2);
	addTriangle(0, 3, 2);


}

void Widget3D::sphereCreator(double r, int p, int m)
{
	points.clear();
	triangles.clear();

	/*double polar = M_PI / m;
	double azimut = 2 * M_PI / p;*/

	for ( int i = 0; i <= m; i ++) {
		double polar = M_PI*i / m;
		for (int j = 0; j <= p; j ++) {
			double azimut = 2 * M_PI *j / p;
			points.push_back({ r * sin(polar) * cos(azimut),r * cos(polar) ,r * sin(polar) * sin(azimut) });

		}

	}
	for (int i = 0; i < m; i++) {
		for (int j = 0; j < p; j++) {
			// (p+1)
			addTriangle(i * (p + 1) + j, i * (p + 1) + j + 1, (i + 1) * (p + 1) + j + 1);
			addTriangle(i * (p + 1) + j, (i + 1) * (p + 1) + j + 1, (i + 1) * (p + 1) + j);



		}

	}




}

bool Widget3D::vtkSave(QString filename)
{
	string path = filename.toStdString();
	ofstream file(path);

	if (!file.is_open())
		return false;

	file << "# vtk DataFile Version 3.0\nvtk output\nASCII\nDATASET POLYDATA\nPOINTS " << points.size() << " " << " double" << endl;

	for (const Point& p : points) {
		file << p.x << " " << p.y << " " << p.z << endl;
	}

	//Ne fungoval mi vtk subor v paraview lebo zabudol som hlavicku POLYGons
	file << "POLYGONS " << triangles.size() << " " << triangles.size() * 4 << endl;

	for (const Triangle& t : triangles) {
		file << "3 " << t.trianglePoints[0] << " " << t.trianglePoints[1] << " " << t.trianglePoints[2] << endl;
	}
	file.close();
	return true;

}

bool Widget3D::vtkLoad(QString file)
{
	points.clear();
	triangles.clear();

	string path = file.toStdString();
	ifstream file1(path);
	string line;
	string head;

	int pointsCount=0,trianglesCount=0,trash,p1,p2,p3;
	string pointsType;

	int objectNum, totalNum;

	if (!file1.is_open())
		return false;

	while (getline(file1, line)) {
		stringstream ss(line);
		ss >> head;
		if (head == "POINTS") {
			ss >> pointsCount;
			ss >> pointsType;
			break;
		}
	}
	points.resize(pointsCount);
	for (int i = 0; i < pointsCount; i++) {
		getline(file1, line);
		stringstream ss(line);
		ss >> points[i].x; 
		ss >> points[i].y;
		ss >> points[i].z;
	}
	
	getline(file1, line);
	stringstream ss(line);
	ss >> head;
	if (head == "POLYGONS") {
		ss >> trianglesCount;
		ss >> pointsCount;
	}

	for (int i = 0; i < trianglesCount; i++) {
		getline(file1, line);
		stringstream ss(line);
		ss >> trash;
		ss >> p1;
		ss >> p2;
		ss >> p3;
		addTriangle(p1, p2, p3);
	}
	
	file1.close();
	return true;
}

QVector<Point> Widget3D::cameraManager()
{
	double Ux, Uy, Uz, Nx, Ny, Nz, Vx, Vy, Vz;
	QVector<Point> Vi;
	/*QVector<Point> projectedPoints;*/
	double z = zenit * M_PI / 180;
	double a = azimut * M_PI / 180;
	Ux = sin(z+ M_PI / 2 ) * sin(a);
	Uy = sin(z + M_PI / 2) * cos(a);
	Uz = cos(z + M_PI / 2);

	Nx = sin(z) * sin(a);
	Ny = sin(z) * cos(a);
	Nz = cos(z);

	Vx = Uy * Nz - Uz * Ny;
	Vy = Uz * Nx - Ux * Nz;
	Vz = Ux * Ny - Uy * Nx;

	/*Vx = sin(z + M_PI / 2) * cos(a)*cos(z)-cos(z+M_PI/2)*sin(z)*cos(a);
	Vy = cos(z + M_PI / 2) * sin(a) * sin(z) - sin(z + M_PI / 2) * sin(a) * cos(z);
	Vz = sin(z + M_PI / 2) * cos(a) * cos(z)*sin(a) - sin(z + M_PI / 2) * sin(z) * cos(a)*sin(a);*/

	for (auto& p : points) {
		Vi.push_back({ p.x * Vx + p.y * Vy + p.z * Vz ,p.x * Ux + p.y * Uy + p.z * Uz, p.x * Nx + p.y * Ny + p.z * Nz });
	}
	
	/*if (projectionType == 0) {
		for (auto& p : points) {
			projectedPoints.push_back({ p.x * Vx + p.y * Vy + p.z * Vz ,p.x * Ux + p.y * Uy + p.z * Uz, p.x * Nx + p.y * Ny + p.z * Nz });
		}

	}
	else {
		for (auto& p : points) {
			projectedPoints.push_back({ range * (p.x * Vx + p.y * Vy + p.z * Vz) /(range- (p.x * Nx + p.y * Ny + p.z * Nz)),range * (p.x * Ux + p.y * Uy + p.z * Uz) / (range -( p.x * Nx + p.y * Ny + p.z * Nz)), range * (p.x * Nx + p.y * Ny + p.z * Nz) / (range - (p.x * Nx + p.y * Ny + p.z * Nz)) });
		}

	}
	return projectedPoints;*/
	return Vi;
}

QVector<Point> Widget3D::projectionManager(QVector<Point> cameraPoints)
{
	QVector<Point> projectedPoints;
	if (projectionType == 0) {
		for (auto& p : cameraPoints) {
			projectedPoints.push_back({ p.x ,p.y, p.z});
		}

	}
	else {
		for (auto& p : cameraPoints) {
			projectedPoints.push_back({ range * (p.x) / (range - (p.z)),range * (p.y) / (range - (p.z)), range * (p.z) / (range - (p.z)) });
		}

	}
	return projectedPoints;
}

Point Widget3D::lsManager()
{
	double Ux, Uy, Uz, Nx, Ny, Nz, Vx, Vy, Vz;
	Point lsi;
	/*QVector<Point> projectedPoints;*/
	double z = zenit * M_PI / 180;
	double a = azimut * M_PI / 180;
	Ux = sin(z + M_PI / 2) * sin(a);
	Uy = sin(z + M_PI / 2) * cos(a);
	Uz = cos(z + M_PI / 2);

	Nx = sin(z) * sin(a);
	Ny = sin(z) * cos(a);
	Nz = cos(z);

	Vx = Uy * Nz - Uz * Ny;
	Vy = Uz * Nx - Ux * Nz;
	Vz = Ux * Ny - Uy * Nx;

	lsi.x = ls.x * Vx + ls.y * Vy + ls.z * Vz;
	lsi.y = ls.x * Ux + ls.y * Uy + ls.z * Uz;
	lsi.z = ls.x * Nx + ls.y * Ny + ls.z * Nz;
	return lsi;
}
