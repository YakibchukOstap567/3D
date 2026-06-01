#pragma once
#include <QtWidgets>
#include "Widget3D.h"
struct Vertex {
    QPoint pos;
    QColor color;
    double z;
};

class ViewerWidget :public QWidget {
    Q_OBJECT
private:
    QSize areaSize = QSize(0, 0);
    QImage* img = nullptr;
    uchar* data = nullptr;
    bool drawLineActivated = false;
    QPoint drawLineBegin = QPoint(0, 0);

    QVector<QPoint> polygonPoints;
   
    bool polygonFinished = false;

    QPoint lineStart;
    QPoint lineEnd;
    bool lineDefined = false;
    bool draggingPolygon = false;
    QPoint lastMousePos = QPoint(0, 0);

    Vertex base_t0, base_t1, base_t2;
    bool fillEnabled = false;
    int currentFillType = 0;
    
    QVector<QPoint> curveEvaluated;

    Widget3D w3D;
    QVector<QVector<double>> Zbuffer;
    QColor cubeColor[6] = {QColor::fromHsv(0, 255, 255),QColor::fromHsv(60, 255, 255),QColor::fromHsv(120, 255, 255),QColor::fromHsv(180, 255, 255),QColor::fromHsv(240, 255, 255),QColor::fromHsv(300, 255, 255)};
public:
    ViewerWidget(QSize imgSize, QWidget* parent = Q_NULLPTR);
    ~ViewerWidget();
    void resizeWidget(QSize size);

   
    bool setImage(const QImage& inputImg);
    QImage* getImage() { return img; };
    bool isEmpty();
    bool changeSize(int width, int height);

    void setPixel(int x, int y, int r, int g, int b, int a = 255);
    void setPixel(int x, int y, double valR, double valG, double valB, double valA = 1.);
    void setPixel(int x, int y, const QColor& color);
    bool isInside(int x, int y);

  
    void drawLine(QPoint start, QPoint end, QColor color, int algType = 0);
    void setDrawLineBegin(QPoint begin) { drawLineBegin = begin; }
    QPoint getDrawLineBegin() { return drawLineBegin; }
    void setDrawLineActivated(bool state) { drawLineActivated = state; }
    bool getDrawLineActivated() { return drawLineActivated; }

    
    uchar* getData() { return data; }
    void setDataPtr() { data = img ? img->bits() : nullptr; }

    int getImgWidth() { return img ? img->width() : 0; };
    int getImgHeight() { return img ? img->height() : 0; };

    void clear();

    
    void drawLineDDA(QPoint start, QPoint end, QColor color, double z);
    void drawLineBresenham(QPoint start, QPoint end, QColor color);
    void drawCircle(QPoint start, QPoint end, QColor color);
    void drawPolygon(QColor color, int algType, bool closed = true);

    QVector<QPoint>& getPolygonPoints() { return polygonPoints; }
    void clearPolygon() { polygonPoints.clear(); polygonFinished = false; }
    bool isPolygonFinished() { return polygonFinished; }
    void setPolygonFinished(bool s) { polygonFinished = s; }

    bool isDraggingPolygon() const { return draggingPolygon; }
    void setDraggingPolygon(bool state) { draggingPolygon = state; }

    QPoint getLastMousePos() const { return lastMousePos; }
    void setLastMousePos(QPoint p) { lastMousePos = p; }

    void movePolygon(int dx, int dy);
    

    void rotateObject(double deg);

    void Scale(double x, double y);
    void Shear(double pS, int algType);
    void OsSum();
    QVector<QPoint> lineClip(QPoint P1, QPoint P2);
    QVector<QPoint> clipPolygon(const QVector<QPoint>& sourcePoints);

    void Scan_line(QVector<QPoint> points, const QColor& color, double z);
    void setScan(bool state) { fillEnabled = state; }
    bool getScan() { return fillEnabled; }

    void setFillEnabled(bool enabled) { fillEnabled = enabled; }
    void setFillType(int type) { currentFillType = type; }

    void fillTriangle(Vertex t0, Vertex t1, Vertex t2, int fillType);
    void fillTrianglePart(int y1, int y2, double x1, double x2, double w1, double w2, int fillType);
    void fillBottomTriangle(Vertex t0, Vertex t1, Vertex t2, int fillType);
    void fillTopTriangle(Vertex t0, Vertex t1, Vertex t2, int fillType);

    QColor getNearestColor(int x, int y, Vertex t0, Vertex t1, Vertex t2);
    QColor getBarycentricColor(int x, int y, Vertex t0, Vertex t1, Vertex t2);
    QColor getColor(int x, int y, int fillType);

    void updateTriangleLogic();

    void setTriangleVertixes(Vertex t0, Vertex t1, Vertex t2) {
        base_t0 = t0;
        base_t1 = t1;
        base_t2 = t2;
    }

    void finishCurve(int curveType, const QColor& color);
    void Hermite();
    void Bezier();
    void BSpline();
    void drawCurve(const QColor& color);
    void draw3D(Widget3D widget3D);
    void  setZPixel(int x, int y, double z, QColor c);
    double interpolateZ(int x, int y, Vertex t0, Vertex t1, Vertex t2);
public slots:
    void paintEvent(QPaintEvent* event) Q_DECL_OVERRIDE;

};
