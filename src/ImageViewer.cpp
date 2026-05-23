#include "ImageViewer.h"


ImageViewer::ImageViewer(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::ImageViewerClass)
{
    ui->setupUi(this);
    vW = new ViewerWidget(QSize(500, 500), ui->scrollArea);
    ui->scrollArea->setWidget(vW);
    ui->scrollArea->setBackgroundRole(QPalette::Dark);
    ui->scrollArea->setWidgetResizable(false);
    ui->scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    vW->setObjectName("ViewerWidget");
    vW->installEventFilter(this);

    globalColor = Qt::blue;
    QString style_sheet = QString("background-color: %1;").arg(globalColor.name(QColor::HexRgb));
    ui->pushButtonSetColor->setStyleSheet(style_sheet);
    ui->deg->setRange(-360.0, 360.0);
    ui->azimut->setRange(0, 360);
    ui->zenit->setRange(-90, 90);
    
    colorT0 = Qt::red;
    colorT1 = Qt::green;
    colorT2 = Qt::blue;
    
}

// Event filters
bool ImageViewer::eventFilter(QObject* obj, QEvent* event)
{
    if (obj->objectName() == "ViewerWidget") {
        return ViewerWidgetEventFilter(obj, event);
    }
    return QMainWindow::eventFilter(obj, event);
}

//ViewerWidget Events
bool ImageViewer::ViewerWidgetEventFilter(QObject* obj, QEvent* event)
{
    ViewerWidget* w = static_cast<ViewerWidget*>(obj);

    if (!w) {
        return false;
    }

    if (event->type() == QEvent::MouseButtonPress) {
        ViewerWidgetMouseButtonPress(w, event);
    }
    else if (event->type() == QEvent::MouseButtonRelease) {
        ViewerWidgetMouseButtonRelease(w, event);
    }
    else if (event->type() == QEvent::MouseMove) {
        ViewerWidgetMouseMove(w, event);
    }
    else if (event->type() == QEvent::Leave) {
        ViewerWidgetLeave(w, event);
    }
    else if (event->type() == QEvent::Enter) {
        ViewerWidgetEnter(w, event);
    }
    else if (event->type() == QEvent::Wheel) {
        ViewerWidgetWheel(w, event);
    }

    return QObject::eventFilter(obj, event);
}
void ImageViewer::ViewerWidgetMouseButtonPress(ViewerWidget* w, QEvent* event)
{
    QMouseEvent* e = static_cast<QMouseEvent*>(event);

    if (ui->toolButtonDrawLine->isChecked())
    {
        if (e->button() == Qt::LeftButton)
        {
            if (w->isPolygonFinished()) return;
            w->getPolygonPoints().push_back(e->pos());

            w->setPixel(e->pos().x(), e->pos().y(), globalColor);

            int n = w->getPolygonPoints().size();
            if (n >= 2) {
                QPoint a = w->getPolygonPoints()[n - 2];
                QPoint b = w->getPolygonPoints()[n - 1];
                w->drawLine(a, b, globalColor, ui->comboBoxLineAlg->currentIndex());
            }
            else {
                w->update();
            }
        }
        else if (e->button() == Qt::RightButton)
        {
            QVector<QPoint>& pts = w->getPolygonPoints();
            if (pts.size() >= 3)
            {
                w->drawPolygon(
                    globalColor,
                    ui->comboBoxLineAlg->currentIndex(),
                    true
                );
            }
            else if (pts.size() == 2)
            {
                w->drawLine(pts[0], pts[1], globalColor, ui->comboBoxLineAlg->currentIndex());
            }
            w->setPolygonFinished(true);
            
            
        }
        if (w->isPolygonFinished()) {
            ui->toolButtonDrawLine->setChecked(false);
       }
        
    }
    else if (ui->Move->isChecked() && w->isPolygonFinished())
    {
        
        QMouseEvent* e = static_cast<QMouseEvent*>(event);
        
        if (e->button() == Qt::LeftButton) {
            w->setDraggingPolygon(true);
            w->setLastMousePos(e->pos());
        }
        if (e->button() == Qt::RightButton) {
            w->setDraggingPolygon(false);
            w->setLastMousePos(e->pos());
        }

        return;
    }
    else if (ui->Curve->isChecked())
    {
        int type = ui->comboBoxCurve->currentIndex();
        

        if (e->button() == Qt::LeftButton)
        {
            w->getPolygonPoints().push_back(e->pos());

            w->setPixel(e->pos().x(), e->pos().y(), globalColor);
           
        }
        if (e->button() == Qt::RightButton)
        {
            w->finishCurve(type,globalColor);
            
        }
        if (w->isPolygonFinished()) {
            ui->Curve->setChecked(false);
        }

        return;
    }
    else if (ui->Circle->isChecked()) {

        QMouseEvent* me = static_cast<QMouseEvent*>(event);
        if (me->button() != Qt::LeftButton)
            return;

        if (w->isPolygonFinished())
            return;

        QPoint pos = me->pos();

      
        if (w->getPolygonPoints().size() == 0)
        {
            w->getPolygonPoints().push_back(pos);
            w->setPixel(pos.x(), pos.y(), globalColor);
            w->update();
        }
        
        else if (w->getPolygonPoints().size() == 1)
        {
            QPoint center = w->getPolygonPoints()[0];
            QPoint radiusPoint = pos;

            
            
            w->drawCircle(center, radiusPoint, globalColor);
          
            w->getPolygonPoints().clear();
            w->setPolygonFinished(true);

            w->update();

            ui->Circle->setChecked(false);
        }

        return;

    }
}
void ImageViewer::ViewerWidgetMouseButtonRelease(ViewerWidget* w, QEvent* event)
{
    QMouseEvent* e = static_cast<QMouseEvent*>(event);
}
void ImageViewer::ViewerWidgetMouseMove(ViewerWidget* w, QEvent* event)
{
    QMouseEvent* e = static_cast<QMouseEvent*>(event);

    if (!w->isDraggingPolygon()) return;

    QPoint currentPos = e->pos();
    QPoint lastPos = w->getLastMousePos();

    int dx = currentPos.x() - lastPos.x();
    int dy = currentPos.y() - lastPos.y();

    w->movePolygon(dx, dy);
    w->setLastMousePos(currentPos);
    w->drawPolygon(globalColor, ui->comboBoxLineAlg->currentIndex(),true);
}
void ImageViewer::ViewerWidgetLeave(ViewerWidget* w, QEvent* event)
{
}
void ImageViewer::ViewerWidgetEnter(ViewerWidget* w, QEvent* event)
{
}
void ImageViewer::ViewerWidgetWheel(ViewerWidget* w, QEvent* event)
{
    QWheelEvent* wheelEvent = static_cast<QWheelEvent*>(event);

    if (!w->isPolygonFinished()) return;

    int delta = wheelEvent->angleDelta().y();

    if (delta > 0)
    {
        w->Scale(1.25,1.25);
    }
    else
    {
        w->Scale(0.75, 0.75);
    }

    vW->drawPolygon(globalColor, ui->comboBoxLineAlg->currentIndex(), true);
}


void ImageViewer::closeEvent(QCloseEvent* event)
{
    if (QMessageBox::Yes == QMessageBox::question(this, "Close Confirmation", "Are you sure you want to exit?", QMessageBox::Yes | QMessageBox::No))
    {
        event->accept();
    }
    else {
        event->ignore();
    }
}


bool ImageViewer::openImage(QString filename)
{
    QImage loadedImg(filename);
    if (!loadedImg.isNull()) {
        return vW->setImage(loadedImg);
    }
    return false;
}
bool ImageViewer::saveImage(QString filename)
{
    QFileInfo fi(filename);
    QString extension = fi.completeSuffix();

    QImage* img = vW->getImage();
    return img->save(filename, extension.toStdString().c_str());
}


void ImageViewer::on_actionOpen_triggered()
{
    QString folder = settings.value("folder_img_load_path", "").toString();

    QString fileFilter = "Image data (*.bmp *.gif *.jpg *.jpeg *.png *.pbm *.pgm *.ppm *.xbm *.xpm);;All files (*)";
    QString fileName = QFileDialog::getOpenFileName(this, "Load image", folder, fileFilter);
    if (fileName.isEmpty()) { return; }

    QFileInfo fi(fileName);
    settings.setValue("folder_img_load_path", fi.absoluteDir().absolutePath());

    if (!openImage(fileName)) {
        msgBox.setText("Unable to open image.");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.exec();
    }
}
void ImageViewer::on_actionOpen_vtk_triggered()
{
    QString file1 = QFileDialog::getOpenFileName(this, "Load vtk", "", "VTK Files (*.vtk)");

    if (!file1.isEmpty()) {
        if (w3D.vtkLoad(file1)) {
            QMessageBox::information(this, "Success", "Vtk file loaded successfully!");
        }
        else {
            QMessageBox::critical(this, "Error", "Failed to read vtk file.");
        }




    }
}
void ImageViewer::on_actionSave_as_triggered()
{
    QString folder = settings.value("folder_img_save_path", "").toString();

    QString fileFilter = "Image data (*.bmp *.gif *.jpg *.jpeg *.png *.pbm *.pgm *.ppm *.xbm *.xpm);;All files (*)";
    QString fileName = QFileDialog::getSaveFileName(this, "Save image", folder, fileFilter);
    if (!fileName.isEmpty()) {
        QFileInfo fi(fileName);
        settings.setValue("folder_img_save_path", fi.absoluteDir().absolutePath());

        if (!saveImage(fileName)) {
            msgBox.setText("Unable to save image.");
            msgBox.setIcon(QMessageBox::Warning);
        }
        else {
            msgBox.setText(QString("File %1 saved.").arg(fileName));
            msgBox.setIcon(QMessageBox::Information);
        }
        msgBox.exec();
    }
}
void ImageViewer::on_actionSave_vtk_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Save 3D model", "", "VTK files (*.vtk)");
    if (fileName.isEmpty()) return;

    if (w3D.vtkSave(fileName)) {
        msgBox.setText("Unable to save image.");
        msgBox.setIcon(QMessageBox::Warning);
    }
    else {
        msgBox.setText(QString("Vtk file saved.").arg(fileName));
        msgBox.setIcon(QMessageBox::Information);
    }

}
void ImageViewer::on_actionClear_triggered()
{
    vW->clear();
}
void ImageViewer::on_actionExit_triggered()
{
    this->close();
}

void ImageViewer::on_Scale_clicked()
{
    double x = ui->spinX->value();
    double y = ui->spinY->value();
    vW->Scale(x, y);
    vW->drawPolygon(globalColor, ui->comboBoxLineAlg->currentIndex(), true);
}

void ImageViewer::on_pushButtonSetColor_clicked()
{
    QColor newColor = QColorDialog::getColor(globalColor, this);
    if (newColor.isValid()) {
        QString style_sheet = QString("background-color: %1;").arg(newColor.name(QColor::HexRgb));
        ui->pushButtonSetColor->setStyleSheet(style_sheet);
        globalColor = newColor;
    }
}

void ImageViewer::on_Rotate_clicked()
{
    double deg1 = ui->deg->value();

    vW->rotateObject(deg1);

    vW->drawPolygon(globalColor, ui->comboBoxLineAlg->currentIndex(), true);
}

void ImageViewer::on_Shear_clicked() {
    double pS = ui->spinShear->value();
    vW->Shear(pS, ui->comboBoxShear->currentIndex());
    vW->drawPolygon(globalColor, ui->comboBoxLineAlg->currentIndex(), true);
}

void ImageViewer::on_OsSum_clicked() {
    vW->OsSum();
    vW->drawPolygon(globalColor, ui->comboBoxLineAlg->currentIndex(), true);

}

void ImageViewer::on_Fill_clicked() {
   
    QVector<QPoint> pts = vW->getPolygonPoints();
    bool isChecked = ui->Fill->isChecked();
    int fillType = ui->comboBoxColor->currentIndex();

    if (pts.size() == 3) {

        Vertex t0 = { pts[0], colorT0 };
        Vertex t1 = { pts[1], colorT1 };
        Vertex t2 = { pts[2], colorT2 };
        vW->setTriangleVertixes(t0, t1, t2);
    }
    vW->setFillEnabled(ui->Fill->isChecked());
    vW->setFillType(ui->comboBoxColor->currentIndex());

    vW->drawPolygon(globalColor, ui->comboBoxLineAlg->currentIndex(), true);
}
void ImageViewer::on_T0_clicked()
{
    QColor newColor = QColorDialog::getColor(colorT0, this);

    if (newColor.isValid()) {
        colorT0 = newColor;

        QString style = QString("background-color: %1;")
            .arg(colorT0.name(QColor::HexRgb));
        ui->T0->setStyleSheet(style);
        
        QVector<QPoint> pts = vW->getPolygonPoints();
        if (pts.size() == 3) {
            vW->setTriangleVertixes({ pts[0], colorT0 }, { pts[1], colorT1 }, { pts[2], colorT2 });
        }
       
        vW->drawPolygon(globalColor, ui->comboBoxLineAlg->currentIndex(), true);
    }
}

void ImageViewer::on_T1_clicked()
{
    QColor newColor = QColorDialog::getColor(colorT1, this);

    if (newColor.isValid()) {
        colorT1 = newColor;

        QString style = QString("background-color: %1;")
            .arg(colorT1.name(QColor::HexRgb));
        ui->T1->setStyleSheet(style);
        
        QVector<QPoint> pts = vW->getPolygonPoints();
        if (pts.size() == 3) {
            vW->setTriangleVertixes({ pts[0], colorT0 }, { pts[1], colorT1 }, { pts[2], colorT2 });
        }
        
        vW->drawPolygon(globalColor, ui->comboBoxLineAlg->currentIndex(), true);
    }

}

void ImageViewer::on_T2_clicked()
{
    QColor newColor = QColorDialog::getColor(colorT2, this);

    if (newColor.isValid()) {
        colorT2 = newColor;

        QString style = QString("background-color: %1;")
            .arg(colorT2.name(QColor::HexRgb));
        ui->T2->setStyleSheet(style);
       
        QVector<QPoint> pts = vW->getPolygonPoints();
        if (pts.size() == 3) {
            vW->setTriangleVertixes({ pts[0], colorT0 }, { pts[1], colorT1 }, { pts[2], colorT2 });
        }
       
        vW->drawPolygon(globalColor, ui->comboBoxLineAlg->currentIndex(), true);
    }
}

void ImageViewer::on_FillType_currentIndexChanged(int index)
{
    vW->setFillType(index);
    vW->drawPolygon(globalColor, ui->comboBoxLineAlg->currentIndex(), true);
}

void ImageViewer::on_cubeButton_clicked()
{
    double l = ui->length->value();
    w3D.cubeCreator(l);


}

void ImageViewer::on_sphereButton_clicked()
{
    double r = ui->radius->value();
    double p = ui->poludnik->value();
    double m = ui->rovnobezka->value();
    w3D.sphereCreator(r, p, m);


}

void ImageViewer::on_draw3D_clicked()
{
    w3D.setAzimut(ui->azimut->value());
    w3D.setZenit(ui->zenit->value());
    w3D.setProjectionType(ui->projectionTypeBox->currentIndex());
    w3D.setRange(ui->range->value());
    vW->draw3D(w3D);
}

void ImageViewer::on_azimut_valueChanged()
{
    w3D.setAzimut(ui->azimut->value());
    vW->draw3D(w3D);
}

void ImageViewer::on_zenit_valueChanged()
{
    w3D.setZenit(ui->zenit->value());
    vW->draw3D(w3D);
}



