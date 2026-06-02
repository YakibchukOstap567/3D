#include   "ViewerWidget.h"
#include   "math.h"
#include "algorithm"
#include <climits>
ViewerWidget::ViewerWidget(QSize imgSize, QWidget* parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_StaticContents);
    setMouseTracking(true);
    if (imgSize != QSize(0, 0)) {
        img = new QImage(imgSize, QImage::Format_ARGB32);
        img->fill(Qt::white);
        resizeWidget(img->size());
        setDataPtr();
    }
}
ViewerWidget::~ViewerWidget()
{
    delete img;
    img = nullptr;
    data = nullptr;
}
void ViewerWidget::resizeWidget(QSize size)
{
    this->resize(size);
    this->setMinimumSize(size);
    this->setMaximumSize(size);
}


bool ViewerWidget::setImage(const QImage& inputImg)
{
    if (img) {
        delete img;
        img = nullptr;
        data = nullptr;
    }
    img = new QImage(inputImg.convertToFormat(QImage::Format_ARGB32));
    if (!img || img->isNull()) {
        return false;
    }
    resizeWidget(img->size());
    setDataPtr();
    update();

    return true;
}
bool ViewerWidget::isEmpty()
{
    if (img == nullptr) {
        return true;
    }

    if (img->size() == QSize(0, 0)) {
        return true;
    }
    return false;
}

bool ViewerWidget::changeSize(int width, int height)
{
    QSize newSize(width, height);

    if (newSize != QSize(0, 0)) {
        if (img != nullptr) {
            delete img;
        }

        img = new QImage(newSize, QImage::Format_ARGB32);
        if (!img || img->isNull()) {
            return false;
        }
        img->fill(Qt::white);
        resizeWidget(img->size());
        setDataPtr();
        update();
    }

    return true;
}

void ViewerWidget::setPixel(int x, int y, int r, int g, int b, int a)
{
    if (!img || !data) return;
    if (!isInside(x, y)) return;

    r = r > 255 ? 255 : (r < 0 ? 0 : r);
    g = g > 255 ? 255 : (g < 0 ? 0 : g);
    b = b > 255 ? 255 : (b < 0 ? 0 : b);
    a = a > 255 ? 255 : (a < 0 ? 0 : a);

    size_t startbyte = y * img->bytesPerLine() + x * 4;
    data[startbyte] = static_cast<uchar>(b);
    data[startbyte + 1] = static_cast<uchar>(g);
    data[startbyte + 2] = static_cast<uchar>(r);
    data[startbyte + 3] = static_cast<uchar>(a);
}
void ViewerWidget::setPixel(int x, int y, double valR, double valG, double valB, double valA)
{
    valR = valR > 1 ? 1 : (valR < 0 ? 0 : valR);
    valG = valG > 1 ? 1 : (valG < 0 ? 0 : valG);
    valB = valB > 1 ? 1 : (valB < 0 ? 0 : valB);
    valA = valA > 1 ? 1 : (valA < 0 ? 0 : valA);

    setPixel(x, y, static_cast<int>(255 * valR + 0.5), static_cast<int>(255 * valG + 0.5), static_cast<int>(255 * valB + 0.5), static_cast<int>(255 * valA + 0.5));
}
void ViewerWidget::setPixel(int x, int y, const QColor& color)
{
    if (color.isValid()) {
        setPixel(x, y, color.red(), color.green(), color.blue(), color.alpha());
    }
}

bool ViewerWidget::isInside(int x, int y)
{
    return img && x >= 0 && y >= 0 && x < img->width() && y < img->height();
}


void ViewerWidget::drawLine(QPoint start, QPoint end, QColor color, int algType)
{
    if (!img || !data) return;

    if (algType == 0) {
        drawLineDDA(start, end,color, 0);
    }
    else if (algType == 1) {
        drawLineBresenham(start, end, color);
    }
    
    update();


}

void ViewerWidget::clear()
{
    if (!img) return;
    img->fill(Qt::white);
    polygonPoints.clear();
    polygonFinished = false;
    drawLineActivated = false;
    update();
}

void ViewerWidget::drawLineDDA(QPoint start, QPoint end ,QColor color, double z = 0)
{
    if (!img || !data) return;

    int x1 = start.x(), x2 = end.x(), y2 = end.y(), y1 = start.y();
    int delx = x2 - x1, dely = y2 - y1;
    double m = (double)(dely) / (double)(delx);
    if (qAbs(m) <= 1.0) {

        int xStep = (x2 >= x1) ? 1 : -1;
        double x = x1;
        double y = y1;

        while (true) {
            int xi = (int)(x + 0.5);
            int yi = (int)(y + 0.5);
            setZPixel(xi, yi,z, color);

            if (xi == x2) {
                break;
            }
            x += xStep;
            y += m * xStep;
        }

    }
    else {

        int yStep = (y2 >= y1) ? 1 : -1;
        double x = x1;
        double y = y1;

        double invM = double(delx) / double(dely);

        while (true) {
            int xi = (int)(x + 0.5);
            int yi = (int)(y + 0.5);
            setZPixel(xi, yi,z, color);

            if (yi == y2) {
                break;
            }

            y += yStep;
            x += invM * yStep;
        }

    }


}

void ViewerWidget::drawLineBresenham(QPoint start, QPoint end, QColor color)
{
    int x1 = start.x(), x2 = end.x(), y2 = end.y(), y1 = start.y();
    int delx = x2 - x1, dely = y2 - y1;

    int xStep = (x2 >= x1) ? 1 : -1;
    int yStep = (y2 >= y1) ? 1 : -1;

    int adx = abs(delx);
    int ady = abs(dely);

    if (adx >= ady) {
        int p = 2 * ady - adx;
        int k1 = 2 * ady;
        int k2 = 2 * (ady - adx);

        for (int i = 0; i <= adx; i++) {
            setPixel(x1, y1, color);
            x1 += xStep;
            if (p > 0) {
                y1 += yStep;
                p += k2;
            }
            else {
                p += k1;
            }
        }
    }
    else {
        int p = 2 * adx - ady;
        int k1 = 2 * adx;
        int k2 = 2 * (adx - ady);

        for (int i = 0; i <= ady; i++) {
            setPixel(x1, y1, color);
            y1 += yStep;
            if (p > 0) {
                x1 += xStep;
                p += k2;
            }
            else {
                p += k1;
            }
        }
    }
}

void ViewerWidget::drawCircle(QPoint centre, QPoint radius, QColor color)
{
    int xc = centre.x(), xr = radius.x(), yr = radius.y(), yc = centre.y();
    int r = sqrt(pow(xr - xc, 2) + pow(yr - yc, 2)) + 0.5;
    int p = 1 - r;
    int x = 0;
    int y = r;
    int dvaX = 3;
    int dvaY = 2 * r - 2;
    while (x <= y) {
        
        setPixel(xc + x, yc + y, color);
        setPixel(xc + y, yc + x, color);
        setPixel(xc - x, yc - y, color);
        setPixel(xc - y, yc - x, color);
        setPixel(xc - x, yc + y, color);
        setPixel(xc - y, yc + x, color);
        setPixel(xc + x, yc - y, color);
        setPixel(xc + y, yc - x, color);
        if (p > 0)

        {
            p = p - dvaY;
            y -= 1;
            dvaY = dvaY - 2;
        }
        p += dvaX;
        dvaX += 2;
        x += 1;
    }
}

void ViewerWidget::drawPolygon(QColor color, int algType, bool closed)
{
    if (!img || !data) return;

    img->fill(Qt::white);

    if (polygonPoints.size() < 2) return;

    if (fillEnabled) {

        if (polygonPoints.size() == 3 && currentFillType != 2) {
            updateTriangleLogic();
            fillTriangle(base_t0, base_t1, base_t2, currentFillType);
        }
        else {

            Scan_line(polygonPoints,color,0);
        }
    }
    QVector<QPoint> clippedPoints;
    
    if (polygonPoints.size() == 2)
    {
        img->fill(Qt::white);

        clippedPoints = lineClip(polygonPoints[0], polygonPoints[1]);

        if (clippedPoints.size() == 2)
            drawLine(clippedPoints[0], clippedPoints[1],color, algType);

        update();
        return;
    }
    else {
        clippedPoints = clipPolygon(polygonPoints);

        if (clippedPoints.isEmpty()) return;

        QPoint start = clippedPoints[0];

        for (int i = 1; i < clippedPoints.size(); i++) {
            QPoint end = clippedPoints[i];

            if (algType == 0) drawLineDDA(start, end, color);
            else if (algType == 1) drawLineBresenham(start, end, color);

            start = end;
        }

        if (closed && clippedPoints.size() >= 3) {
            if (algType == 0) drawLineDDA(start, clippedPoints[0], color);
            else if (algType == 1) drawLineBresenham(start, clippedPoints[0], color);
        }
    }
    

    update();

}

void ViewerWidget::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    if (!img || img->isNull()) return;

    QRect area = event->rect();
    painter.drawImage(area, *img, area);
}



void ViewerWidget::movePolygon(int dx, int dy)
{
    for (QPoint& p : polygonPoints) {
        p += QPoint(dx, dy);
    }
    
}



void ViewerWidget::rotateObject(double deg)
{
    if (polygonPoints.size() < 2)
    {
        return;
    }
    double rad = deg * M_PI / 180.0;
    QPoint center = polygonPoints[0];
    for (QPoint& p : polygonPoints) {
        double x = p.x() - center.x();
        double y = p.y() - center.y();

        double xr = x * cos(rad) - y * sin(rad) + center.x();
        double yr = x * sin(rad) + y * cos(rad) + center.y();
        p.setX(xr);
        p.setY(yr);
    }
    
}

void ViewerWidget::Scale(double x, double y)
{
    if (polygonPoints.isEmpty() || !img) return;

    double cx = 0.0;
    double cy = 0.0;

    for (const QPoint& p : polygonPoints) {
        cx += p.x();
        cy += p.y();
    }

    cx /= polygonPoints.size();
    cy /= polygonPoints.size();

    for (QPoint& p : polygonPoints) {
        if (x != 0) 
        { 
            double nx = cx + (p.x() - cx) * x;
            p.setX(nx); 
        }
        if (y != 0) 
        {
            double ny = cy + (p.y() - cy) * y;
            p.setY(ny);
        }
    }

    
}




void ViewerWidget::Shear(double pS, int algType)
{
    if (polygonPoints.size() < 2 || !img) return;
    //QVector<QPoint> newPoint;

    if (algType == 0) {
        for (int i = 0; i < polygonPoints.size(); i++) {
            double nx = polygonPoints[i].x() + polygonPoints[i].y() * pS; 
            polygonPoints[i].setX(nx);
        }
    }
    if (algType == 1) {
        for (int i = 0; i < polygonPoints.size(); i++) {
            double ny = polygonPoints[i].y() + polygonPoints[i].x() * pS; 
            polygonPoints[i].setY(ny);
        }
    }
    
}

void ViewerWidget::OsSum() {
    if (polygonPoints.size() < 2 || !img) return;
   
    int x1 = polygonPoints[0].x(), x2 = polygonPoints[1].x();
    int y1 = polygonPoints[0].y(), y2 = polygonPoints[1].y();
    int Vx = x2 - x1, Vy = y2 - y1;
    double a = Vy, b = -Vx, c = -a * x1 - b * y1;

    if (polygonPoints.size() > 2) {
        for (int i = 1; i < polygonPoints.size(); i++) {
            double xN = polygonPoints[i].x() - 2 * a * ((a * polygonPoints[i].x() + b * polygonPoints[i].y() + c) / (a * a + b * b));
            double yN = polygonPoints[i].y() - 2 * b * ((a * polygonPoints[i].x() + b * polygonPoints[i].y() + c) / (a * a + b * b));
            polygonPoints[i].setX(xN), polygonPoints[i].setY(yN);
        }
    }
    else if (polygonPoints.size() == 2) {
        polygonPoints[1].setX(x2);
        polygonPoints[1].setY(2 * y1 - y2);
    }
   
}

QVector<QPoint> ViewerWidget::lineClip(QPoint P1, QPoint P2)
{
    QVector<QPoint> result;
    QVector2D d(P2 - P1);

    double tE = 0.0;   
    double tL = 1.0;   

    struct Edge {
        QVector2D n;   
        QPoint E;      
    };

    QVector<Edge> edges = {
        { QVector2D(1,  0), QPoint(0, 0) },   
        { QVector2D(-1,  0), QPoint(img->width() - 1, 0) },
        { QVector2D(0,  1), QPoint(0, 0) },  
        { QVector2D(0, -1), QPoint(0, img->height() - 1) }
    };

    for (auto& edge : edges)
    {
        QVector2D w = QVector2D(P1 - edge.E);

        double dn = QVector2D::dotProduct(d, edge.n);
        double wn = QVector2D::dotProduct(w, edge.n);

        if (dn == 0)
        {
            if (wn < 0)  
                return {};
            continue;
        }

        double t = -wn / dn;

        if (dn > 0)         
        {
            if (t > tE) tE = t;
            if (tE > tL) return {};
        }
        else                
        {
            if (t < tL) tL = t;
            if (tL < tE) return {};
        }
    }

    QPoint P1c = QPoint(
        qRound(P1.x() + d.x() * tE),
        qRound(P1.y() + d.y() * tE)
    );

    QPoint P2c = QPoint(
        qRound(P1.x() + d.x() * tL),
        qRound(P1.y() + d.y() * tL)
    );

    result.push_back(P1c);
    result.push_back(P2c);
    return result;
}

QVector<QPoint> ViewerWidget::clipPolygon(const QVector<QPoint>& sourcePoints) {
    if (sourcePoints.size() < 3) return sourcePoints;

    QVector<QPoint> V = sourcePoints;
    QVector<QPoint> W;
    int xmax = img->width() - 1;
    int ymax = img->height() - 1;

    for (int border = 0; border < 4; border++) {
        if (V.isEmpty()) return {};
        W.clear();
        QPoint S = V.last();
        for (int i = 0; i < V.size(); i++) {
            QPoint P = V[i];
            bool Sin, Pin;
            if (border == 0) { Sin = (S.x() >= 0); Pin = (P.x() >= 0); }
            else if (border == 1) { Sin = (S.x() <= xmax); Pin = (P.x() <= xmax); }
            else if (border == 2) { Sin = (S.y() >= 0); Pin = (P.y() >= 0); }
            else { Sin = (S.y() <= ymax); Pin = (P.y() <= ymax); }

            if (Pin) {
                if (!Sin) { 
                    double t;
                    if (border < 2) t = (double)((border == 0 ? 0 : xmax) - S.x()) / (P.x() - S.x());
                    else t = (double)((border == 2 ? 0 : ymax) - S.y()) / (P.y() - S.y());
                    W.append(QPoint(qRound(S.x() + t * (P.x() - S.x())), qRound(S.y() + t * (P.y() - S.y()))));
                }
                W.append(P);
            }
            else if (Sin) { 
                double t;
                if (border < 2) t = (double)((border == 0 ? 0 : xmax) - S.x()) / (P.x() - S.x());
                else t = (double)((border == 2 ? 0 : ymax) - S.y()) / (P.y() - S.y());
                W.append(QPoint(qRound(S.x() + t * (P.x() - S.x())), qRound(S.y() + t * (P.y() - S.y()))));
            }
            S = P;
        }
        V = W;
    }
    return V;
}

void ViewerWidget::Scan_line(QVector<QPoint> points,const QColor& color, double z = 0)
{

    if (!img || points.size() < 3)
        return;

    int ymin = points[0].y();
    int ymax = points[0].y();

    
    for (const QPoint& p : points) {
        if (p.y() < ymin) ymin = p.y();
        if (p.y() > ymax) ymax = p.y();
    }

    
    ymin = std::max(0, ymin);
    ymax = std::min(img->height() - 1, ymax);

    
    for (int y = ymin; y <= ymax; y++) {
        QVector<int> xYes;

       
        for (int i = 0; i < points.size(); i++) {
            QPoint p1 = points[i];
            QPoint p2 = points[(i + 1) % points.size()];

            
            if (p1.y() == p2.y())
                continue;

            
            if (p1.y() > p2.y())
                std::swap(p1, p2);

            
            if (y >= p1.y() && y < p2.y()) {
                double x = p1.x() + (double)(y - p1.y()) * (p2.x() - p1.x()) / (double)(p2.y() - p1.y());

                xYes.push_back(qRound(x));
            }
        }

       
        std::sort(xYes.begin(), xYes.end());

        
        for (int i = 0; i + 1 < xYes.size(); i += 2) {
            int xStart = xYes[i];
            int xEnd = xYes[i + 1];

            if (xStart > xEnd)
                std::swap(xStart, xEnd);

            xStart = std::max(0, xStart);
            xEnd = std::min(img->width() - 1, xEnd);

            for (int x = xStart; x <= xEnd; x++) {
                
                    setZPixel(x, y, z,color);
                
            }
        }
    }
}

void ViewerWidget::fillTriangle(Vertex t0, Vertex t1, Vertex t2, int fillType)
{
    base_t0 = t0;
    base_t1 = t1;
    base_t2 = t2;

    std::vector<Vertex> points = { t0, t1, t2 };

    std::sort(points.begin(), points.end(), [](const Vertex& a, const Vertex& b) {
        if (a.pos.y() != b.pos.y()) { 
            return a.pos.y() < b.pos.y(); 
        }
        else { 
            return a.pos.x() < b.pos.x(); 
        }
        });

    t0 = points[0];
    t1 = points[1];
    t2 = points[2];

    if (t0.pos.y() == t1.pos.y()) {
       
        fillBottomTriangle(t0, t1, t2, fillType);
    }
    else if (t1.pos.y() == t2.pos.y()) {
        fillTopTriangle(t0, t1, t2, fillType);
    }
    else {
        QPoint pos_p;
        pos_p = QPoint((t0.pos.x() + (t1.pos.y() - t0.pos.y()) * ((double)(t2.pos.x() - t0.pos.x()) / (t2.pos.y() - t0.pos.y()))), t1.pos.y());
        Vertex p = { pos_p, t1.color };

        if (t1.pos.x() < p.pos.x()) {
            fillTopTriangle(t0, t1, p, fillType);
            fillBottomTriangle(t1, p, t2, fillType);
        }
        else {
            fillTopTriangle(t0, p, t1, fillType);
            fillBottomTriangle(p, t1, t2, fillType);
        }
    }

}

void ViewerWidget::fillTrianglePart(int y1, int y2, double x1, double x2, double w1, double w2, int fillType)
{
    for (int y = y1; y <= y2; y++) {

        int startX = (int)std::ceil(std::min(x1, x2));
        int endX = (int)std::floor(std::max(x1, x2));

        for (int x = startX; x <= endX; x++) {
            double z = interpolateZ(x, y, base_t0, base_t1, base_t2);
            setZPixel(x, y,z,getColor(x, y, fillType));
        }

        x1 += w1;
        x2 += w2;
    }
}

void ViewerWidget::fillBottomTriangle(Vertex t0, Vertex t1, Vertex t2, int fillType)
{
    double w1 = (double)(t2.pos.x() - t0.pos.x()) / (t2.pos.y() - t0.pos.y());
    double w2 = (double)(t2.pos.x() - t1.pos.x()) / (t2.pos.y() - t1.pos.y());

    double x1 = t0.pos.x();
    double x2 = t1.pos.x();

    int y1 = t0.pos.y();
    int y2 = t2.pos.y();

    fillTrianglePart(y1, y2, x1, x2, w1, w2, fillType);
}

void ViewerWidget::fillTopTriangle(Vertex t0, Vertex t1, Vertex t2, int fillType)
{
    
    double w1 = (double)(t1.pos.x() - t0.pos.x()) / (t1.pos.y() - t0.pos.y());
    double w2 = (double)(t2.pos.x() - t0.pos.x()) / (t2.pos.y() - t0.pos.y());

    
    double x1 = t0.pos.x();
    double x2 = t0.pos.x();

    int y1 = t0.pos.y();
    int y2 = t1.pos.y();

    fillTrianglePart(y1, y2, x1, x2, w1, w2, fillType);
}

QColor ViewerWidget::getNearestColor(int x, int y, Vertex t0, Vertex t1, Vertex t2)
{
    
    int d0 = (x - t0.pos.x()) * (x - t0.pos.x()) +
        (y - t0.pos.y()) * (y - t0.pos.y());

    int d1 = (x - t1.pos.x()) * (x - t1.pos.x()) +
        (y - t1.pos.y()) * (y - t1.pos.y());

    int d2 = (x - t2.pos.x()) * (x - t2.pos.x()) +
        (y - t2.pos.y()) * (y - t2.pos.y());

    
    if (d0 <= d1 && d0 <= d2) {
        return t0.color;
    }
    else if (d1 <= d0 && d1 <= d2) {
        return t1.color;
    }
    else {
        return t2.color;
    }
}

QColor ViewerWidget::getBarycentricColor(int x, int y, Vertex t0, Vertex t1, Vertex t2)
{
    
    double A = abs((t1.pos.x() - t0.pos.x()) * (t2.pos.y() - t0.pos.y()) -
        (t1.pos.y() - t0.pos.y()) * (t2.pos.x() - t0.pos.x())) / 2;


    double A0 = abs((t1.pos.x() - x) * (t2.pos.y() - y) -
        (t1.pos.y() - y) * (t2.pos.x() - x)) / 2;

    double A1 = abs((t0.pos.x() - x) * (t2.pos.y() - y) -
        (t0.pos.y() - y) * (t2.pos.x() - x)) / 2;

    double A2 = A - A0 - A1;

    
    double l0 = A0 / A;
    double l1 = A1 / A;
    double l2 = A2 / A;

  
    int r = (int)(l0 * t0.color.red() + l1 * t1.color.red() + l2 * t2.color.red());
    int g = (int)(l0 * t0.color.green() + l1 * t1.color.green() + l2 * t2.color.green());
    int b = (int)(l0 * t0.color.blue() + l1 * t1.color.blue() + l2 * t2.color.blue());

  
    r = qBound(0, r, 255);
    g = qBound(0, g, 255);
    b = qBound(0, b, 255);

    return QColor(r, g, b);
}

QColor ViewerWidget::getColor(int x, int y, int fillType) {
    QColor color;
    if (fillType == 0) {
        color = getNearestColor(x, y, base_t0, base_t1, base_t2);
    }
    else if (fillType == 1) {
        color = getBarycentricColor(x, y, base_t0, base_t1, base_t2);
    }
    
    return color;

}

void ViewerWidget::updateTriangleLogic() {
    if (polygonPoints.size() == 3) {
        
        
        base_t0.pos = polygonPoints[0];
        base_t1.pos = polygonPoints[1];
        base_t2.pos = polygonPoints[2];
    }
}


void ViewerWidget::finishCurve(int curveType, const QColor& color)
{
    if (polygonPoints.size() < 2) return;

    if (curveType == 0)
        Hermite();
    else if (curveType == 1)
        Bezier();
    else if (curveType == 2)
        BSpline();

    drawCurve(color);
    polygonFinished = true;
}

void ViewerWidget::Hermite()
{
   
}

void ViewerWidget::Bezier()
{
    curveEvaluated.clear();
    QVector<QPoint>& P = polygonPoints;

    for (double t = 0; t <= 1; t += 0.002)
    {
        QVector<QPointF> Q(P.begin(), P.end());
        int n = P.size() - 1;

        for (int k = 1; k <= n; k++)
            for (int i = 0; i <= n - k; i++)
                Q[i] = Q[i] * (1 - t) + Q[i + 1] * t;

        curveEvaluated.push_back(Q[0].toPoint());
    }
}

void ViewerWidget::BSpline()
{
    curveEvaluated.clear();
    QVector<QPoint>& P = polygonPoints;

    if (P.size() < 4) return;

    for (int i = 0; i < P.size() - 3; i++)
    {
        for (double t = 0; t < 1; t += 0.01)
        {
            double b0 = (-t * t * t + 3 * t * t - 3 * t + 1) / 6;
            double b1 = (3 * t * t * t - 6 * t * t + 4) / 6;
            double b2 = (-3 * t * t * t + 3 * t * t + 3 * t + 1) / 6;
            double b3 = t * t * t / 6;

            double x = b0 * P[i].x() + b1 * P[i + 1].x() + b2 * P[i + 2].x() + b3 * P[i + 3].x();
            double y = b0 * P[i].y() + b1 * P[i + 1].y() + b2 * P[i + 2].y() + b3 * P[i + 3].y();

            curveEvaluated.push_back(QPoint(x, y));
        }
    }
}

void ViewerWidget::drawCurve(const QColor& color)
{
    img->fill(Qt::white);

    
    for (auto p : polygonPoints)
        setPixel(p.x(), p.y(), color);

    
    for (int i = 1; i < curveEvaluated.size(); i++)
        drawLineBresenham(curveEvaluated[i - 1], curveEvaluated[i], color);

    update();
}

void ViewerWidget::draw3D(Widget3D widget3D)
{
   Zbuffer.clear();
   Zbuffer.resize(img->width());
   for (int x = 0; x < img->width(); x++) {
       Zbuffer[x].resize(img->height());

       for (int y = 0; y < img->height(); y++) {
           Zbuffer[x][y] = -1000000000;
       }
   }
   img->fill(Qt::white);

   QVector<Point> cameraPoints = widget3D.cameraManager();
   QVector<Point> projectedPoints = widget3D.projectionManager(cameraPoints);
   QVector<Triangle> triangles = widget3D.getTriangles();
   QVector<Point> points = widget3D.getPoints();

   QVector<QVector<int>> rgbi = widget3D.getRGBI();
   QVector<QVector<double>> rgbd = widget3D.getRGBD();
   Point ls = widget3D.lsManager();
   int range = widget3D.getRange();
   double h = widget3D.getH();

   QColor rgb3[3];
   //QPoint pos[3];
   //int i = 0, j = 0;
   if (widget3D.getWireframeStatus() == 0) {
       



           for (auto& t : triangles) {

               int t1 = t.trianglePoints[0];
               int t2 = t.trianglePoints[1];
               int t3 = t.trianglePoints[2];

               

               double x = (cameraPoints[t1].x + cameraPoints[t2].x + cameraPoints[t3].x) / 3;
               double y = (cameraPoints[t1].y + cameraPoints[t2].y + cameraPoints[t3].y) / 3;
               double z = (cameraPoints[t1].z + cameraPoints[t2].z + cameraPoints[t3].z) / 3;

               QVector<QPoint> triagPoints;
               triagPoints.push_back(QPoint(projectedPoints[t1].x + width() / 2, projectedPoints[t1].y + width() / 2));
               triagPoints.push_back(QPoint(projectedPoints[t2].x + width() / 2, projectedPoints[t2].y + width() / 2));
               triagPoints.push_back(QPoint(projectedPoints[t3].x + width() / 2, projectedPoints[t3].y + width() / 2));




               /*double x = cameraPoints[t1].x;
               double y = cameraPoints[t1].y;
               double z = cameraPoints[t1].z;*/

               /*double x = (projectedPoints[t1].x + projectedPoints[t2].x + projectedPoints[t3].x) / 3;
               double y = (projectedPoints[t1].y + projectedPoints[t2].y + projectedPoints[t3].y) / 3;
               double z = (projectedPoints[t1].z + projectedPoints[t2].z + projectedPoints[t3].z) / 3;*/

               /*drawLineDDA(QPoint(projectedPoints[t1].x + width() / 2, projectedPoints[t1].y + width() / 2), QPoint(projectedPoints[t2].x + width() / 2, projectedPoints[t2].y + width() / 2), Qt::blue, z);
               drawLineDDA(QPoint(projectedPoints[t2].x + width() / 2, projectedPoints[t2].y + width() / 2), QPoint(projectedPoints[t3].x + width() / 2, projectedPoints[t3].y + width() / 2), Qt::blue, z);
               drawLineDDA(QPoint(projectedPoints[t3].x + width() / 2, projectedPoints[t3].y + width() / 2), QPoint(projectedPoints[t1].x + width() / 2, projectedPoints[t1].y + width() / 2), Qt::blue, z);*/

               

               if (widget3D.getTypeOfShading() == 0) {

                   /*int t1 = t.trianglePoints[0];
                   int t2 = t.trianglePoints[1];
                   int t3 = t.trianglePoints[2];



                   double x = (cameraPoints[t1].x + cameraPoints[t2].x + cameraPoints[t3].x) / 3;
                   double y = (cameraPoints[t1].y + cameraPoints[t2].y + cameraPoints[t3].y) / 3;
                   double z = (cameraPoints[t1].z + cameraPoints[t2].z + cameraPoints[t3].z) / 3;

                   QVector<QPoint> triagPoints;
                   triagPoints.push_back(QPoint(projectedPoints[t1].x + width() / 2, projectedPoints[t1].y + width() / 2));
                   triagPoints.push_back(QPoint(projectedPoints[t2].x + width() / 2, projectedPoints[t2].y + width() / 2));
                   triagPoints.push_back(QPoint(projectedPoints[t3].x + width() / 2, projectedPoints[t3].y + width() / 2));*/


                   // projectedPoints[t2].x - projectedPoints[t1].x , projectedPoints[t2].y - projectedPoints[t1].y, projectedPoints[t2].z - projectedPoints[t1].z    
                   // projectedPoints[t3].x - projectedPoints[t1].x , projectedPoints[t3].y - projectedPoints[t1].y, projectedPoints[t3].z - projectedPoints[t1].z
                   double Nx = (cameraPoints[t2].y - cameraPoints[t1].y) * (cameraPoints[t3].z - cameraPoints[t1].z) - (cameraPoints[t2].z - cameraPoints[t1].z) * (cameraPoints[t3].y - cameraPoints[t1].y);
                   double Ny = (cameraPoints[t2].z - cameraPoints[t1].z) * (cameraPoints[t3].x - cameraPoints[t1].x) - (cameraPoints[t2].x - cameraPoints[t1].x) * (cameraPoints[t3].z - cameraPoints[t1].z);
                   double Nz = (cameraPoints[t2].x - cameraPoints[t1].x) * (cameraPoints[t3].y - cameraPoints[t1].y) - (cameraPoints[t2].y - cameraPoints[t1].y) * (cameraPoints[t3].x - cameraPoints[t1].x);

                   /*double Nx = (projectedPoints[t2].y - projectedPoints[t1].y) * (projectedPoints[t3].z - projectedPoints[t1].z) - (projectedPoints[t2].z - projectedPoints[t1].z) * (projectedPoints[t3].y - projectedPoints[t1].y);
                   double Ny = (projectedPoints[t2].z - projectedPoints[t1].z) * (projectedPoints[t3].x - projectedPoints[t1].x) - (projectedPoints[t2].x - projectedPoints[t1].x) * (projectedPoints[t3].z - projectedPoints[t1].z);
                   double Nz = (projectedPoints[t2].x - projectedPoints[t1].x) * (projectedPoints[t3].y - projectedPoints[t1].y) - (projectedPoints[t2].y - projectedPoints[t1].y) * (projectedPoints[t3].x - projectedPoints[t1].x);*/

                   double lengthN = sqrt(Nx * Nx + Ny * Ny + Nz * Nz);

                   Nx = Nx / lengthN;
                   Ny = Ny / lengthN;
                   Nz = Nz / lengthN;

                   /*double test = Nx * x + Ny * y + Nz * z;

                   if (test < 0) {
                       Nx = -Nx;
                       Ny = -Ny;
                       Nz = -Nz;
                   }*/

                   double Lx = ls.x - x;
                   double Ly = ls.y - y;
                   double Lz = ls.z - z;

                   double lengthL = sqrt(Lx * Lx + Ly * Ly + Lz * Lz);

                   Lx = Lx / lengthL;
                   Ly = Ly / lengthL;
                   Lz = Lz / lengthL;

                   double Rx = 2 * (Lx * Nx + Ly * Ny + Lz * Nz) * Nx - Lx;
                   double Ry = 2 * (Lx * Nx + Ly * Ny + Lz * Nz) * Ny - Ly;
                   double Rz = 2 * (Lx * Nx + Ly * Ny + Lz * Nz) * Nz - Lz;

                   double lengthR = sqrt(Rx * Rx + Ry * Ry + Rz * Rz);

                   Rx = Rx / lengthR;
                   Ry = Ry / lengthR;
                   Rz = Rz / lengthR;

                   double Vx = -x;
                   double Vy = -y;
                   double Vz = range - z;

                   double lengthV = sqrt(Vx * Vx + Vy * Vy + Vz * Vz);

                   Vx = Vx / lengthV;
                   Vy = Vy / lengthV;
                   Vz = Vz / lengthV;


                  /* double VR = Vx * Rx + Vy * Ry + Vz * Rz;
                   VR = std::max(0.0, VR);
                   VR = std::min(1.0, VR);

                   double aboba = pow(std::max(0.0, (VR)), h);

                   double IsR = rgbi[0][0] * rgbd[1][0] * aboba;
                   double IsG = rgbi[0][1] * rgbd[1][1] * aboba;
                   double IsB = rgbi[0][2] * rgbd[1][2] * aboba;*/

                   double IsR = rgbi[0][0] * rgbd[1][0] * pow(std::max(0.0, (Vx * Rx + Vy * Ry + Vz * Rz)), h);
                   double IsG = rgbi[0][1] * rgbd[1][1] * pow(std::max(0.0, (Vx * Rx + Vy * Ry + Vz * Rz)), h);
                   double IsB = rgbi[0][2] * rgbd[1][2] * pow(std::max(0.0, (Vx * Rx + Vy * Ry + Vz * Rz)), h);

                   double IdR = rgbi[0][0] * rgbd[0][0] * std::max(0.0, (Lx * Nx + Ly * Ny + Lz * Nz));
                   double IdG = rgbi[0][1] * rgbd[0][1] * std::max(0.0, (Lx * Nx + Ly * Ny + Lz * Nz));
                   double IdB = rgbi[0][2] * rgbd[0][2] * std::max(0.0, (Lx * Nx + Ly * Ny + Lz * Nz));

                   double IaR = rgbi[1][0] * rgbd[2][0];
                   double IaG = rgbi[1][1] * rgbd[2][1];
                   double IaB = rgbi[1][2] * rgbd[2][2];

                   double IR = IsR + IdR + IaR;
                   double IG = IsG + IdG + IaG;
                   double IB = IsB + IdB + IaB;

                   /*int r = qBound(0, static_cast<int>(rgbi[0][0] * rgbd[1][0] * aboba + rgbi[0][0] * rgbd[0][0] * std::max(0.0, (Lx * Nx + Ly * Ny + Lz * Nz)) + rgbi[1][0] * rgbd[2][0]), 255);
                   int g = qBound(0, static_cast<int>(rgbi[0][1] * rgbd[1][1] * aboba + rgbi[0][1] * rgbd[0][1] * std::max(0.0, (Lx * Nx + Ly * Ny + Lz * Nz)) + rgbi[1][1] * rgbd[2][1]), 255);
                   int b = qBound(0, static_cast<int>(IB), 255);*/

                   int r = qBound(0, (int)IR, 255);
                   int g = qBound(0, (int)IG, 255);
                   int b = qBound(0, (int)IB, 255);

                   QColor rgb = QColor::fromRgb(r, g, b);
                   Scan_line(triagPoints, rgb, z);
               }
               else if (widget3D.getTypeOfShading() == 1) {
                   for (int i = 0; i < 3; i++) {

                     /*  int t1 = t.trianglePoints[0];
                       int t2 = t.trianglePoints[1];
                       int t3 = t.trianglePoints[2];*/
                       int p = t.trianglePoints[i];


                       double x = cameraPoints[p].x;
                       double y = cameraPoints[p].y;
                       double z = cameraPoints[p].z;

                       /*QVector<QPoint> triagPoints;
                       triagPoints.push_back(QPoint(projectedPoints[p].x + width() / 2, projectedPoints[p].y + width() / 2));*/
                       /*triagPoints.push_back(QPoint(projectedPoints[t2].x + width() / 2, projectedPoints[t2].y + width() / 2));
                       triagPoints.push_back(QPoint(projectedPoints[t3].x + width() / 2, projectedPoints[t3].y + width() / 2));*/


                       // projectedPoints[t2].x - projectedPoints[t1].x , projectedPoints[t2].y - projectedPoints[t1].y, projectedPoints[t2].z - projectedPoints[t1].z    
                       // projectedPoints[t3].x - projectedPoints[t1].x , projectedPoints[t3].y - projectedPoints[t1].y, projectedPoints[t3].z - projectedPoints[t1].z
                       /*double Nx = (cameraPoints[t2].y - cameraPoints[t1].y) * (cameraPoints[t3].z - cameraPoints[t1].z) - (cameraPoints[t2].z - cameraPoints[t1].z) * (cameraPoints[t3].y - cameraPoints[t1].y);
                       double Ny = (cameraPoints[t2].z - cameraPoints[t1].z) * (cameraPoints[t3].x - cameraPoints[t1].x) - (cameraPoints[t2].x - cameraPoints[t1].x) * (cameraPoints[t3].z - cameraPoints[t1].z);
                       double Nz = (cameraPoints[t2].x - cameraPoints[t1].x) * (cameraPoints[t3].y - cameraPoints[t1].y) - (cameraPoints[t2].y - cameraPoints[t1].y) * (cameraPoints[t3].x - cameraPoints[t1].x);*/

                       /*double Nx = (projectedPoints[t2].y - projectedPoints[t1].y) * (projectedPoints[t3].z - projectedPoints[t1].z) - (projectedPoints[t2].z - projectedPoints[t1].z) * (projectedPoints[t3].y - projectedPoints[t1].y);
                       double Ny = (projectedPoints[t2].z - projectedPoints[t1].z) * (projectedPoints[t3].x - projectedPoints[t1].x) - (projectedPoints[t2].x - projectedPoints[t1].x) * (projectedPoints[t3].z - projectedPoints[t1].z);
                       double Nz = (projectedPoints[t2].x - projectedPoints[t1].x) * (projectedPoints[t3].y - projectedPoints[t1].y) - (projectedPoints[t2].y - projectedPoints[t1].y) * (projectedPoints[t3].x - projectedPoints[t1].x);*/
                       double Nx = x;
                       double Ny = y;
                       double Nz = z;

                       double lengthN = sqrt(Nx * Nx + Ny * Ny + Nz * Nz);

                       Nx = Nx / lengthN;
                       Ny = Ny / lengthN;
                       Nz = Nz / lengthN;

                       /*double test = Nx * x + Ny * y + Nz * z;

                       if (test < 0) {
                           Nx = -Nx;
                           Ny = -Ny;
                           Nz = -Nz;
                       }*/

                       double Lx = ls.x - x;
                       double Ly = ls.y - y;
                       double Lz = ls.z - z;

                       double lengthL = sqrt(Lx * Lx + Ly * Ly + Lz * Lz);

                       Lx = Lx / lengthL;
                       Ly = Ly / lengthL;
                       Lz = Lz / lengthL;

                       double Rx = 2 * (Lx * Nx + Ly * Ny + Lz * Nz) * Nx - Lx;
                       double Ry = 2 * (Lx * Nx + Ly * Ny + Lz * Nz) * Ny - Ly;
                       double Rz = 2 * (Lx * Nx + Ly * Ny + Lz * Nz) * Nz - Lz;

                       double lengthR = sqrt(Rx * Rx + Ry * Ry + Rz * Rz);

                       Rx = Rx / lengthR;
                       Ry = Ry / lengthR;
                       Rz = Rz / lengthR;

                       double Vx = -x;
                       double Vy = -y;
                       double Vz = range - z;

                       double lengthV = sqrt(Vx * Vx + Vy * Vy + Vz * Vz);

                       Vx = Vx / lengthV;
                       Vy = Vy / lengthV;
                       Vz = Vz / lengthV;


                       double VR = Vx * Rx + Vy * Ry + Vz * Rz;
                       VR = std::max(0.0, VR);
                       VR = std::min(1.0, VR);

                       double aboba = pow(std::max(0.0, (VR)), h);

                       double IsR = rgbi[0][0] * rgbd[1][0] * aboba;
                       double IsG = rgbi[0][1] * rgbd[1][1] * aboba;
                       double IsB = rgbi[0][2] * rgbd[1][2] * aboba;

                       double IdR = rgbi[0][0] * rgbd[0][0] * std::max(0.0, (Lx * Nx + Ly * Ny + Lz * Nz));
                       double IdG = rgbi[0][1] * rgbd[0][1] * std::max(0.0, (Lx * Nx + Ly * Ny + Lz * Nz));
                       double IdB = rgbi[0][2] * rgbd[0][2] * std::max(0.0, (Lx * Nx + Ly * Ny + Lz * Nz));

                       double IaR = rgbi[1][0] * rgbd[2][0];
                       double IaG = rgbi[1][1] * rgbd[2][1];
                       double IaB = rgbi[1][2] * rgbd[2][2];

                       double IR = IsR + IdR + IaR;
                       double IG = IsG + IdG + IaG;
                       double IB = IsB + IdB + IaB;

                       /*int r = qBound(0, static_cast<int>(rgbi[0][0] * rgbd[1][0] * aboba + rgbi[0][0] * rgbd[0][0] * std::max(0.0, (Lx * Nx + Ly * Ny + Lz * Nz)) + rgbi[1][0] * rgbd[2][0]), 255);
                       int g = qBound(0, static_cast<int>(rgbi[0][1] * rgbd[1][1] * aboba + rgbi[0][1] * rgbd[0][1] * std::max(0.0, (Lx * Nx + Ly * Ny + Lz * Nz)) + rgbi[1][1] * rgbd[2][1]), 255);
                       int b = qBound(0, static_cast<int>(IB), 255);*/

                       int r = qBound(0, (int)IR, 255);
                       int g = qBound(0, (int)IG, 255);
                       int b = qBound(0, (int)IB, 255);

                       rgb3[i] = QColor::fromRgb(r, g, b);
                       //Scan_line(triagPoints, rgb, z);
                       //pos[i] = QPoint(x + width() / 2,y + width() / 2);


                   }
                   //Qpoint(projectedPoints[t1].x + width() / 2, projectedPoints[t1].y + width() / 2)
                   fillTriangle({ QPoint(projectedPoints[t1].x + width() / 2, projectedPoints[t1].y + width() / 2) ,rgb3[0], cameraPoints[t1].z }, { QPoint(projectedPoints[t2].x + width() / 2, projectedPoints[t2].y + width() / 2),rgb3[1],cameraPoints[t2].z }, { QPoint(projectedPoints[t3].x + width() / 2, projectedPoints[t3].y + width() / 2),rgb3[2],cameraPoints[t3].z }, widget3D.getTypeOfFill());



               }


               
           }
        
      
       
   }
   else {
       for (auto& t : triangles) {



           int t1 = t.trianglePoints[0];
           int t2 = t.trianglePoints[1];
           int t3 = t.trianglePoints[2];

           drawLineDDA(QPoint(projectedPoints[t1].x + width() / 2, projectedPoints[t1].y + width() / 2), QPoint(projectedPoints[t2].x + width() / 2, projectedPoints[t2].y + width() / 2), Qt::blue);
           drawLineDDA(QPoint(projectedPoints[t2].x + width() / 2, projectedPoints[t2].y + width() / 2), QPoint(projectedPoints[t3].x + width() / 2, projectedPoints[t3].y + width() / 2), Qt::blue);
           drawLineDDA(QPoint(projectedPoints[t3].x + width() / 2, projectedPoints[t3].y + width() / 2), QPoint(projectedPoints[t1].x + width() / 2, projectedPoints[t1].y + width() / 2), Qt::blue);
       }
      



   }
   update();
}

void ViewerWidget::setZPixel(int x, int y, double z, QColor c)
{
    if (x < 0 || x >= img->width() || y < 0 || y >= img->height()) {
        return;
    }
    if (Zbuffer.isEmpty()) {
        setPixel(x, y, c); return;
    }
    if (z > Zbuffer[x][y]) {
        Zbuffer[x][y] = z;
        setPixel(x,y,c);
    }

}

double ViewerWidget::interpolateZ(int x, int y, Vertex t0, Vertex t1, Vertex t2)
{

    double A = static_cast<double>((t1.pos.y() -t2.pos.y()) * (t0.pos.x() - t2.pos.x()) + (t2.pos.x() - t1.pos.x()) * (t0.pos.y() - t2.pos.y()));

    double A0 = ((t1.pos.y() - t2.pos.y()) * (x - t2.pos.x()) + (t2.pos.x() - t1.pos.x()) * (y - t2.pos.y()));
    double A1 = ((t2.pos.y() - t0.pos.y()) * (x - t2.pos.x()) +(t0.pos.x() - t2.pos.x()) * (y - t2.pos.y()));
    double A2 = A - A0 - A1;


    return A0/A * t0.z + A1/A * t1.z + A2/A * t2.z;


}

