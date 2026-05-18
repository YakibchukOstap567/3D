#include "Widget3D.h"
#include "string"
#include "fstream"
#include "sstream"

using namespace std;

void Widget3D::cubeCreator(double l)
{
	points.clear();
	triangles.clear();

	points.push_back({ 0,0,0 });
	points.push_back({ l,0,0 });
	points.push_back({ l,l,0 });
	points.push_back({ 0,l,0 });
	points.push_back({ 0,0,l });
	points.push_back({ l,0,l });
	points.push_back({ l,l,l });
	points.push_back({ 0,l,l });

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

	double polar = M_PI / m;
	double azimut = 2 * M_PI / p;

	for (double i = 0; i <= M_PI; i += polar) {
		for (double j = 0; j <= 2 * M_PI; j += azimut) {

			points.push_back({ r * sin(i) * cos(j),r * cos(i) ,r * sin(i) * sin(j) });

		}

	}
	for (double i = 0; i <= m; i++) {
		for (double j = 0; j <= p; j++) {

			addTriangle(i * m + j, i * m + j + 1, (i + 1) * m + j + 1);
			addTriangle(i * m + j, (i + 1) * m + j + 1, (i + 1) * m + j);



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
