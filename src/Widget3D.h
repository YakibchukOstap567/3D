#pragma once
#include <QtWidgets>


struct Point {

	double x, y, z;

};
struct Triangle {
	int trianglePoints[3] = {};


};


class Widget3D
{
private:
	int azimut = 0, zenit = 0, range = 0;
	bool projectionType = 0, cubeOrSphere = 0, wireframeStatus = 0; //cube = o,sphere =1
	QVector<Point> points;
	QVector<Triangle> triangles;

	void addTriangle(int p1, int p2, int p3) {
		Triangle triag;
		triag.trianglePoints[0] = p1;
		triag.trianglePoints[1] = p2;
		triag.trianglePoints[2] = p3;
		triangles.push_back(triag);
	}

public:

	void setPoints(QVector<Point> p) { points = p; };
	void setTriangles(QVector<Triangle> t) { triangles = t; };

	QVector<Point> getPoints() { return points; };
	QVector<Triangle> getTriangles() { return triangles; };

	void setAzimut(int a) { azimut = a; };
	void setZenit(int z) { zenit = z; };
	void setProjectionType(bool pt) { projectionType = pt; };
	void setRange(int r) { range = r; };
	void setCubeOrSphere(bool cors) { cubeOrSphere = cors; };
	void setWireframeStatus(bool w) { wireframeStatus = w; };

	int getAzimut() { return azimut; };
	int getZenit() { return zenit; };
	bool getProjectionType() { return projectionType; };
	int getRange() { return range; };
	bool getCubeOrSphere() { return cubeOrSphere; };
	bool getWireframeStatus() { return wireframeStatus; };

	void cubeCreator(double l);
	void sphereCreator(double r, int p, int m);

	bool vtkSave(QString file);
	bool vtkLoad(QString file);
	
	QVector<Point> cameraManager();
};

