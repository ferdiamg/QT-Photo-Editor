/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include <QMainWindow>
#ifndef QT_NO_PRINTER
#include <QPrinter>
#endif
#include "akkumulator.h"
#include "fstream"
#include <vector>

using namespace std;

class QAction;
class QLabel;
class QMenu;
class QScrollArea;
class QScrollBar;
class QTextEdit;
class QVBoxLayout;
class QHBoxLayout;
class QTabWidget;
class QPushButton;
class QSpinBox;
class QCheckBox;
class QSlider;
class QBarSet;
class QBarSeries;
class QChart;
class QStringList;
class QBarCategoryAxis;
class QValueAxis;
class QChartView;
class QGroupBox;
class QRadioButton;
class QTableWidget;
class QDoubleSpinBox;

class ImageViewer : public QMainWindow {
  Q_OBJECT

private:
  // Beispiel für GUI Elemente
  QWidget *m_option_panel1;
  QVBoxLayout *m_option_layout1;

  QWidget *m_option_panel2;
  QVBoxLayout *m_option_layout2;

  QWidget *m_option_panel3;
  QVBoxLayout *m_option_layout3;

  QWidget *m_option_panel4;
  QVBoxLayout *m_option_layout4;

  QWidget *m_option_panel5;
  QVBoxLayout *m_option_layout5;

  // hier können weitere GUI Objekte hin wie Buttons Slider etc.
  QPushButton *button1;
  QPushButton *button2;
  QPushButton *calculateVarianzButton;
  QPushButton *calculateAvgHelligkeitButton;
  QPushButton *histogramButton;
  QPushButton *autoKontrastButton;
  QLabel *varianz;
  QLabel *avgHelligkeit;
  QLabel *helligkeitSliderLabel;
  QLabel *kontrastSliderLabel;
  QCheckBox *checkbox1;
  QSlider *slider1;
  QSlider *helligkeitSlider;
  QSlider *kontrastSlider;
  QSpinBox *pixelsLowSpinbox;
  QSpinBox *pixelsHighSpinbox;
  QLabel *histogramImage;
  QRadioButton *linear;
  QRadioButton *logarithmic;
  QSpinBox *matrixX;
  QSpinBox *matrixY;
  QTableWidget *table;
  QSpinBox *sigmaSpin;
  QSpinBox *matrix2X;
  QSpinBox *matrix2Y;
  QTableWidget *table2;
  QSpinBox *sigmaCannySpin;
  QSpinBox *thighSpin;
  QSpinBox *tlowSpin;
  QSpinBox *sigmaUSMSpin;
  QSlider *USMfactorSlider;
  QPushButton *houghButton;
  QSpinBox *AminSpinner;
  QSpinBox *mHough;
  QSpinBox *nHough;
  QDoubleSpinBox *omegaHough;

private slots:

  // Beispiel für einen Algorithmus
  void applyExampleAlgorithm();
  void uebung1c();
  void checkBoxMethod();
  void sliderMethod();
  void calcVarianz();
  int calcEchteVarianz();
  void calcAvgHelligkeit();
  void convertToGray();
  QImage copyConvertToGray();
  QImage greyify(QImage toGreifyImage);
  int getY(QColor pixelColor);
  int getCb(QColor pixelColor);
  int getCr(QColor pixelColor);
  QColor YCbCrtoRGB(int Y, int Cb, int Cr);
  void createHistogram();
  void adjustBrightness();
  void adjustKontrast();
  void robAutoKontrast();
  void createMatrix();
  void createMatrix2();
  void applyFilter();
  void onSpinEditFinished();
  void onSpinEditFinished2();
  void fillTable();
  void fillTable2();
  void applyzeroPaddFilter();
  void applyconstantFilter();
  void applymirrorFilter();
  vector<int> makeGaussKernel1D(double sigma);
  void applygaussFilter();
  void edgeMatrixFilter();
  void sobelFilter();
  void cannyEdge();
  int getOrientationSector(int dx, int dy);
  bool isLocalMax(QImage Emag, int u, int v, int s0, int tlo);
  void traceAndThreshold(QImage Enms, QImage *Ebin, int u0, int v0, int tlo);
  QImage applygaussFilter2(int sigma);
  void unsharpMaskingSharpener();
  void houghTransform();
  vector<vector<int>> getCurrentAmax(QImage AmaxWhiteImage);
  bool isLocalMax2(vector<vector<Akkumulator>> &accumulatorVector, int u,
                   int v);

  // hier können weitere als SLOTS definierte Funktionen hin, die auf Knopfdruck
  // etc. aufgerufen werden.

  void open();
  void print();
  void zoomIn();
  void zoomOut();
  void normalSize();
  void fitToWindow();
  void about();

public:
  ImageViewer();
  bool loadFile(const QString &);
  void updateImageDisplay();

protected:
  void resizeEvent(QResizeEvent *event);

private:
  // in diesen Beiden Methoden sind Änderungen nötig bzw. sie dienen als
  // Vorlage für eigene Methoden.
  void generateControlPanels();

  // Ab hier technische Details die nicht für das Verständnis notwendig sind.
  void startLogging();
  void generateMainGui();

  void createActions();
  void createMenus();
  void updateActions();
  void scaleImage(double factor);
  void adjustScrollBar(QScrollBar *scrollBar, double factor);
  void renewLogging();

  QTabWidget *tabWidget;
  QTextEdit *logBrowser;
  QWidget *centralwidget;
  QLabel *imageLabel;
  QScrollArea *scrollArea;
  double scaleFactor;
  QImage *image;
  QImage *EmagGlobal;

  std::fstream logFile;

#ifndef QT_NO_PRINTER
  QPrinter printer;
#endif

  QAction *openAct;
  QAction *printAct;
  QAction *exitAct;
  QAction *zoomInAct;
  QAction *zoomOutAct;
  QAction *normalSizeAct;
  QAction *fitToWindowAct;
  QAction *aboutAct;
  QAction *aboutQtAct;

  QMenu *fileMenu;
  QMenu *viewMenu;
  QMenu *helpMenu;
};

#endif
