#include <QtWidgets>


struct Point {

	double x, y, z;

};
struct Triangle {

	int trianglePoints[3];


};


class Widget3D
{
private:

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

	void cubeCreator(double l);
	bool vtkSave(QString file);
	
};

