#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "myplayer.h"
#include "mygraphicsview.h"
#include "drawarea.h"
#include "mywidget.h"
#include "image.h"
#include <QtConcurrent/qtconcurrentrun.h>

#include <QMediaService>
#include <QMediaPlaylist>
#include <QVideoProbe>
#include <QAudioProbe>
#include <QMediaMetaData>
#include <QLabel>

#include <QtWidgets>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QDebug>
#include <QFrame>
#include <QPainter>
#include <QPoint>

using namespace QtConcurrent;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Add placeholder items to combox dropdown
    ui->comboBox->addItem("Doctor");
    ui->comboBox->addItem("Dinosaur");
    ui->comboBox->addItem("Dinosaur2");

    ui->primaryNextFramesButton->setEnabled(false);
    ui->primaryPrevFramesButton->setEnabled(false);
    ui->secondaryNextFramesButton->setEnabled(false);
    ui->secondaryPrevFramesButton->setEnabled(false);

    ui->primaryVideoProgressBar->setValue(0);
    ui->primaryVideoProgressBar->hide();
    ui->secondaryVideoProgressBar->setValue(0);
    ui->secondaryVideoProgressBar->hide();

    graphicsView = new MyGraphicsView(this);
    graphicsView->setGeometry(10, 180, 10 + 352, 180 + 288);
    graphicsView->resize(352, 288);
    this->layout()->addWidget(graphicsView);

    graphicsView2 = new MyGraphicsView(this);
    graphicsView2->setGeometry(352 + 20, 180, 10 + 352, 180 + 288);
    graphicsView2->resize(352, 288);
    this->layout()->addWidget(graphicsView2);

    primList = QList<QImage>();
    secList = QList<QImage>();

    // Connect sliders to function
    connect(ui->horizontalSliderLeft, SIGNAL(valueChanged(int)), this, SLOT(on_sliderLeft_changed()));
    connect(ui->horizontalSliderRight, SIGNAL(valueChanged(int)), this, SLOT(on_sliderRight_changed()));
    // Connect import buttons
    connect(ui->importPrimaryButton, SIGNAL(clicked()), this, SLOT(import()));
    connect(ui->importSecondaryButton, SIGNAL(clicked()), this, SLOT(import()));
    resize(960, 640);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void imageLoading(QStringList imageFileNames,
                              QStringList constStrs,
                              QList<QImage> *images,
                              QStringList *fileNames,
                              QStringList *fileNamesPrev)
{
    int count = 0;
    foreach(QString filename, imageFileNames)
    {
        if(count > 1000)
        {
            continue;
        }

        QString filePath = constStrs.at(0) + "/" + filename;
        QFile file(filePath);
        if(!file.open(QFile::ReadOnly))
        {
            qDebug() << "Issue occurred in opening the file";
            return;
        }

        QByteArray data = file.readAll();
        file.flush();
        file.close();

        QImage img(352,288,QImage::Format_RGB32);
        QRgb *pixels = reinterpret_cast<QRgb*>(img.bits());
        for(size_t i = 0; i < 352*288; i++)
        {
            uchar pixel1 = data[i];
            uchar pixel2 = data[i+1];
            uchar pixel3 = data[i+2];
            pixels[i] = qRgb(pixel1,pixel2,pixel3);
        }

        img = img.convertToFormat(QImage::Format_RGB444);//Format_Indexed8 //Format_RGB16
        images->push_back(img);

        if(constStrs.at(1) == "primaryNextFramesButton" || constStrs.at(1) == "secondaryNextFramesButton" || constStrs.at(1) == "importPrimaryButton" || constStrs.at(1) == "importSecondaryButton"){
            fileNamesPrev->append(filename);
            fileNames->removeFirst();
        }
        else{
            QString s = fileNamesPrev->last();
            fileNamesPrev->prepend(s);
            fileNames->removeLast();
        }
        count++;
    }
        qDebug() << "Done Loading!";
}

void MainWindow::import()
{
    QFileDialog upload(this);
    upload.setFileMode(QFileDialog::Directory);
    upload.setAcceptMode(QFileDialog::AcceptOpen);
    upload.setWindowTitle("Import Files");
    upload.exec();

    QString directoryPath = upload.directory().path();
    QDir directory(upload.directory().path());
    QStringList imageFileNames = directory.entryList(QStringList() << "*.rgb", QDir::Files);

    QString caller = QObject::sender()->objectName();

    caller == "importPrimaryButton" ?
                ui->primaryVideoProgressBar->show() :
                ui->secondaryVideoProgressBar->show();

    if(caller == "importPrimaryButton")
    {
        numberImageFilesPrimary = imageFileNames.size();
        staticConstStringsPrimary.append(directoryPath);
        staticConstStringsPrimary.append(caller);
        primaryFileNames_n = imageFileNames;
        QFuture<void> f1 = run(imageLoading, primaryFileNames_n, staticConstStringsPrimary, &primList, &primaryFileNames_n, &primaryFileNames_p);
        f1.waitForFinished();
    }
    else
    {
        numberImageFilesSecondary = imageFileNames.size();
        staticConstStringsSecondary.append(directoryPath);
        staticConstStringsSecondary.append(caller);
        secondaryFileNames_n = imageFileNames;
        QFuture<void> f2 = run(imageLoading, secondaryFileNames_n, staticConstStringsSecondary, &secList, &secondaryFileNames_n, &secondaryFileNames_p);
        f2.waitForFinished();
    }

    caller == "importPrimaryButton" ? ui->primaryNextFramesButton->setEnabled(true) :  ui->secondaryNextFramesButton->setEnabled(true);
    qDebug() << "Done Loading!";
}

void MainWindow::on_sliderLeft_changed()
{
    if((segmentIndexPrimary*200) + ui->horizontalSliderLeft->value() >= primList.size())
    {
        return;
    }

    //qDebug() << "on slider left changed";
    pixMapPrim = new QGraphicsPixmapItem(QPixmap::fromImage(primList.at((segmentIndexPrimary*200) + ui->horizontalSliderLeft->value())));
    graphicsView->scene->addItem(pixMapPrim);
    ui->left_sliderLabel->setText(QString::number((segmentIndexPrimary*200) + ui->horizontalSliderLeft->value()));
}

void MainWindow::on_sliderRight_changed()
{
    if((segmentIndexSecondary*200) + ui->horizontalSliderRight->value() >= secList.size())
    {
        return;
    }

    //qDebug() << "on slider right changed";
    pixMapSec = new QGraphicsPixmapItem(QPixmap::fromImage(secList.at((segmentIndexSecondary*200) + ui->horizontalSliderRight->value())));
    graphicsView2->scene->addItem(pixMapSec);
    ui->right_sliderLabel->setText(QString::number((segmentIndexSecondary*200) + ui->horizontalSliderRight->value()));
}

void MainWindow::on_primaryNextFramesButton_clicked()
{
    if(primaryFileNames_n.size() != 0)
    {
        ui->primaryPrevFramesButton->setEnabled(true);
    }

    qDebug() << ((segmentIndexPrimary+2)*200);

    if((((segmentIndexPrimary+1)*200)) == 1000 && primaryFileNames_p.size() == numberImageFilesPrimary)
    {
        ui->primaryNextFramesButton->setEnabled(false);
    }
    else if((((segmentIndexPrimary+1)*200)) >= 1000)
    {
        QString caller = QObject::sender()->objectName();
        staticConstStringsPrimary[1] = caller;
        primList.clear();
        segmentIndexPrimary = 0;
        QFuture<void> loadNext = run(imageLoading, primaryFileNames_n, staticConstStringsPrimary, &primList, &primaryFileNames_n, &primaryFileNames_p);
        loadNext.waitForFinished();
    }
    else{
        segmentIndexPrimary++;
    }
}

void MainWindow::on_secondaryNextFramesButton_clicked()
{
    if(secondaryFileNames_n.size() != 0)
    {
        ui->secondaryPrevFramesButton->setEnabled(true);
    }

    if((((segmentIndexSecondary+1)*200)) == 1000 && secondaryFileNames_p.size() == numberImageFilesSecondary)
    {
        qDebug() << secondaryFileNames_p.size();
        qDebug() << numberImageFilesSecondary;
        ui->secondaryNextFramesButton->setEnabled(false);
    }
    else if((((segmentIndexSecondary+1)*200)) >= 1000)
    {
        QString caller = QObject::sender()->objectName();
        staticConstStringsSecondary[1] = caller;
        secList.clear();
        segmentIndexSecondary = 0;
        QFuture<void> loadNext = run(imageLoading, secondaryFileNames_n, staticConstStringsSecondary, &secList, &secondaryFileNames_n, &secondaryFileNames_p);
        loadNext.waitForFinished();
    }
    else{
        segmentIndexSecondary++;
    }
}

void MainWindow::on_primaryPrevFramesButton_clicked()
{
    if(primaryFileNames_p.size() == 0)
    {
        ui->primaryPrevFramesButton->setEnabled(false);
    }
    if(segmentIndexPrimary == 0)
    {
        QString caller = QObject::sender()->objectName();
        staticConstStringsPrimary[1] = caller;
        primList.clear();
        segmentIndexPrimary = (1000/200 - 1);
        QFuture<void> loadNext = run(imageLoading, primaryFileNames_p, staticConstStringsPrimary, &primList, &primaryFileNames_p, &primaryFileNames_n);
        loadNext.waitForFinished();
    }
    else{
        segmentIndexPrimary--;
    }
}

void MainWindow::on_secondaryPrevFramesButton_clicked()
{
    if(secondaryFileNames_p.size() == 0)
    {
        ui->secondaryPrevFramesButton->setEnabled(false);
    }
    if(segmentIndexSecondary == 0)
    {
        QString caller = QObject::sender()->objectName();
        staticConstStringsSecondary[1] = caller;
        secList.clear();
        segmentIndexSecondary = (1000/200 - 1);
        QFuture<void> loadNext = run(imageLoading, secondaryFileNames_p, staticConstStringsSecondary, &secList, &secondaryFileNames_p, &secondaryFileNames_n);
        loadNext.waitForFinished();
    }
    else{
        segmentIndexSecondary--;
    }
}
