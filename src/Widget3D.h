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
	bool projectionType = 0,  wireframeStatus = 0, typeOfShading = 0; //cube = o,sphere =1  cubeOrSphere = 0,
	QVector<Point> points;
	QVector<Triangle> triangles;
	QVector<QVector<int>> rgbi;
	QVector<QVector<double>> rgbd;
	Point ls;
	void addTriangle(int p1, int p2, int p3) {
		Triangle triag;
		triag.trianglePoints[0] = p1;
		triag.trianglePoints[1] = p2;
		triag.trianglePoints[2] = p3;
		triangles.push_back(triag);
	}
	double h;
	QVector<Point> cameraPoints;
public:
	Widget3D() {

		rgbi.resize(2);
		for (int i = 0; i < 2; i++)
			rgbi[i].resize(3);

		rgbd.resize(3);
		for (int i = 0; i < 3; i++)
			rgbd[i].resize(3);
	}
	
	void setPoints(QVector<Point> p) { points = p; };
	void setTriangles(QVector<Triangle> t) { triangles = t; };
	void setCameraPoints(QVector<Point> p) { cameraPoints = p; };

	QVector<Point> getPoints() { return points; };
	QVector<Triangle> getTriangles() { return triangles; };
	QVector<Point> getCameraPoints() { return cameraPoints; };

	void setAzimut(int a) { azimut = a; };
	void setZenit(int z) { zenit = z; };
	void setProjectionType(bool pt) { projectionType = pt; };
	void setRange(int r) { range = r; };
	//void setCubeOrSphere(bool cors) { cubeOrSphere = cors; };
	void setWireframeStatus(bool w) { wireframeStatus = w; };

	void setRGBI(int lscR, int lscG, int lscB, int alcR, int alcG, int alcB) { rgbi[0][0] = lscR; rgbi[0][1] = lscG; rgbi[0][2] = lscB; rgbi[1][0] = alcR; rgbi[1][1] = alcG; rgbi[1][2] = alcB;};
	void setRGBD(double dR, double dG, double dB, double rR, double rG, double rB, double aR, double aG, double aB){ rgbd[0][0] = dR; rgbd[0][1] = dG; rgbd[0][2] = dB; rgbd[1][0] = rR; rgbd[1][1] = rG; rgbd[1][2] = rB; rgbd[2][0] = aR; rgbd[2][1] = aG; rgbd[2][2] = aB;};
	void setLS(int x, int y, int z) { ls.x = x, ls.y = y, ls.z = z; };
	void setTypeOfShading(bool s) { typeOfShading = s; };
	void setH(double h1) { h = h1; };

	int getAzimut() { return azimut; };
	int getZenit() { return zenit; };
	bool getProjectionType() { return projectionType; };
	int getRange() { return range; };
	//bool getCubeOrSphere() { return cubeOrSphere; };
	bool getWireframeStatus() { return wireframeStatus; };
	double getH() { return h; };

	QVector<QVector<int>> getRGBI() { return rgbi; };
	QVector<QVector<double>> getRGBD() { return rgbd; };
	Point getLS() { return ls; };
	bool getTypeOfShading() { return typeOfShading; };

	void cubeCreator(double l);
	void sphereCreator(double r, int p, int m);

	bool vtkSave(QString file);
	bool vtkLoad(QString file);
	
	QVector<Point> cameraManager();
	QVector<Point> projectionManager(QVector<Point> cameraPoints);
};

