#include <QtWidgets>
#ifndef QT_NO_PRINTER
#include <QPrintDialog>
#endif
#include <iostream>
#include <string>

#include "imageviewer-qt5.h"

void ImageViewer::generateControlPanels() {
  // Uebung1 tab

  m_option_panel1 = new QWidget();
  m_option_layout1 = new QVBoxLayout();
  m_option_panel1->setLayout(m_option_layout1);

  button1 = new QPushButton();
  button1->setText("Apply algorithm");

  button2 = new QPushButton();
  button2->setText("Uebung1c Algorithm");

  checkbox1 = new QCheckBox("Grayscale");
  slider1 = new QSlider(Qt::Horizontal, this);

  QObject::connect(button1, SIGNAL(clicked()), this,
                   SLOT(applyExampleAlgorithm()));
  QObject::connect(button2, SIGNAL(clicked()), this, SLOT(uebung1c()));
  QObject::connect(checkbox1, SIGNAL(clicked()), this, SLOT(checkBoxMethod()));
  QObject::connect(slider1, SIGNAL(valueChanged(int)), this,
                   SLOT(sliderMethod()));

  m_option_layout1->addWidget(button1);
  m_option_layout1->addWidget(button2);
  m_option_layout1->addWidget(checkbox1);
  m_option_layout1->addWidget(slider1);
  tabWidget->addTab(m_option_panel1, "Übung 1");

  // Hinweis: Pro Aufgabe jeweils einen solchen Tab

  // Uebung2 tab

  m_option_panel2 = new QWidget();
  m_option_layout2 = new QVBoxLayout();
  m_option_panel2->setLayout(m_option_layout2);

  // Varianz & Helligkeit
  varianz = new QLabel("Variance: ");
  avgHelligkeit = new QLabel("avg. Brightness: ");
  calculateVarianzButton = new QPushButton("Calc Variance");
  calculateAvgHelligkeitButton = new QPushButton("Calc avg. Brightness");
  QGroupBox *groupBoxVarHell = new QGroupBox(tr("Variance/avg. Brightness"));
  QHBoxLayout *hbox = new QHBoxLayout();
  hbox->addWidget(varianz);
  hbox->addWidget(avgHelligkeit);
  hbox->addWidget(calculateVarianzButton);
  hbox->addWidget(calculateAvgHelligkeitButton);
  groupBoxVarHell->setLayout(hbox);

  // Robuste automatische Konstrastanpassung
  QLabel *pixelsLowSpinboxLabel = new QLabel("Low: ");
  pixelsLowSpinbox = new QSpinBox();
  QLabel *pixelsHighSpinboxLabel = new QLabel("High: ");
  pixelsHighSpinbox = new QSpinBox();
  autoKontrastButton = new QPushButton("rob. auto. Contrast Adj.");
  QGroupBox *groupBoxAutoContrast =
      new QGroupBox(tr("rob. auto. Kontrastanpassung"));
  QHBoxLayout *hboxAutoContrast = new QHBoxLayout();
  hboxAutoContrast->addWidget(pixelsLowSpinboxLabel);
  hboxAutoContrast->addWidget(pixelsLowSpinbox);
  hboxAutoContrast->addWidget(pixelsHighSpinboxLabel);
  hboxAutoContrast->addWidget(pixelsHighSpinbox);
  hboxAutoContrast->addWidget(autoKontrastButton);
  groupBoxAutoContrast->setLayout(hboxAutoContrast);

  kontrastSliderLabel = new QLabel("Kontrast-Slider:");
  kontrastSlider = new QSlider(Qt::Horizontal, this);
  kontrastSlider->setRange(0, 200);
  kontrastSlider->setSliderPosition(100);
  helligkeitSliderLabel = new QLabel("Helligkeit-Slider:");
  helligkeitSlider = new QSlider(Qt::Horizontal, this);
  helligkeitSlider->setRange(-1000, 1000);

  histogramImage = new QLabel();

  // Histogram
  histogramButton = new QPushButton();
  histogramButton->setText("Create histogram");
  linear = new QRadioButton(tr("Linear"));
  logarithmic = new QRadioButton(tr("Logarithmic"));
  linear->setChecked(true);
  QGroupBox *groupBox = new QGroupBox(tr("Histogram settings"));
  QVBoxLayout *vbox = new QVBoxLayout();
  vbox->addWidget(linear);
  vbox->addWidget(logarithmic);
  vbox->addWidget(histogramButton);
  groupBox->setLayout(vbox);

  QObject::connect(calculateVarianzButton, SIGNAL(clicked()), this,
                   SLOT(calcVarianz()));
  QObject::connect(calculateAvgHelligkeitButton, SIGNAL(clicked()), this,
                   SLOT(calcAvgHelligkeit()));
  QObject::connect(histogramButton, SIGNAL(clicked()), this,
                   SLOT(createHistogram()));
  QObject::connect(helligkeitSlider, SIGNAL(valueChanged(int)), this,
                   SLOT(adjustBrightness()));
  QObject::connect(kontrastSlider, SIGNAL(valueChanged(int)), this,
                   SLOT(adjustKontrast()));
  QObject::connect(autoKontrastButton, SIGNAL(clicked()), this,
                   SLOT(robAutoKontrast()));

  m_option_layout2->addWidget(histogramImage);
  m_option_layout2->addWidget(groupBox);
  m_option_layout2->addWidget(groupBoxVarHell);
  m_option_layout2->addWidget(kontrastSliderLabel);
  m_option_layout2->addWidget(kontrastSlider);
  m_option_layout2->addWidget(helligkeitSliderLabel);
  m_option_layout2->addWidget(helligkeitSlider);
  m_option_layout2->addWidget(groupBoxAutoContrast);

  // Übung 3 tab
  m_option_panel3 = new QWidget();
  m_option_layout3 = new QVBoxLayout();
  m_option_panel3->setLayout(m_option_layout3);

  QPushButton *matrixButton = new QPushButton();
  matrixButton->setText("Create input matrix");
  matrixX = new QSpinBox();
  matrixX->setValue(3);
  matrixX->setSingleStep(2);
  matrixY = new QSpinBox();
  matrixY->setValue(3);
  matrixY->setSingleStep(2);
  QLabel *spinBoxTableYLabel = new QLabel("Y:");
  QLabel *spinBoxTableXLabel = new QLabel("X:");
  table = new QTableWidget();
  QPushButton *fillTableButton = new QPushButton();
  fillTableButton->setText("Fill table");
  QPushButton *applyFilter = new QPushButton("Apply filter");
  QPushButton *zeroPaddFilter = new QPushButton("Zero padding filter");
  QPushButton *constantFilter = new QPushButton("Constant filter");
  QPushButton *mirrorFilter = new QPushButton("Mirror filter");
  sigmaSpin = new QSpinBox();
  QPushButton *gaussFilter = new QPushButton("Gauss filter");

  QGroupBox *filterBox = new QGroupBox(tr("Filter settings"));
  QVBoxLayout *vboxFilter = new QVBoxLayout();
  vboxFilter->addWidget(applyFilter);
  vboxFilter->addWidget(zeroPaddFilter);
  vboxFilter->addWidget(constantFilter);
  vboxFilter->addWidget(mirrorFilter);
  vboxFilter->addWidget(sigmaSpin);
  vboxFilter->addWidget(gaussFilter);
  filterBox->setLayout(vboxFilter);

  m_option_layout3->addWidget(spinBoxTableXLabel);
  m_option_layout3->addWidget(matrixX);
  m_option_layout3->addWidget(spinBoxTableYLabel);
  m_option_layout3->addWidget(matrixY);
  m_option_layout3->addWidget(matrixButton);
  m_option_layout3->addWidget(table);
  m_option_layout3->addWidget(fillTableButton);
  m_option_layout3->addWidget(filterBox);

  QObject::connect(matrixButton, SIGNAL(clicked()), this, SLOT(createMatrix()));
  QObject::connect(applyFilter, SIGNAL(clicked()), this, SLOT(applyFilter()));
  QObject::connect(zeroPaddFilter, SIGNAL(clicked()), this,
                   SLOT(applyzeroPaddFilter()));
  QObject::connect(constantFilter, SIGNAL(clicked()), this,
                   SLOT(applyconstantFilter()));
  QObject::connect(mirrorFilter, SIGNAL(clicked()), this,
                   SLOT(applymirrorFilter()));
  QObject::connect(matrixX, SIGNAL(valueChanged(int)), this,
                   SLOT(onSpinEditFinished()));
  QObject::connect(matrixY, SIGNAL(valueChanged(int)), this,
                   SLOT(onSpinEditFinished()));
  QObject::connect(fillTableButton, SIGNAL(clicked()), this, SLOT(fillTable()));
  QObject::connect(gaussFilter, SIGNAL(clicked()), this,
                   SLOT(applygaussFilter()));

  // Übung 4
  m_option_panel4 = new QWidget();
  m_option_layout4 = new QVBoxLayout();
  m_option_panel4->setLayout(m_option_layout4);

  QPushButton *matrix2Button = new QPushButton();
  matrix2Button->setText("Create input matrix");
  matrix2X = new QSpinBox();
  matrix2X->setValue(3);
  matrix2X->setSingleStep(2);
  matrix2Y = new QSpinBox();
  matrix2Y->setValue(3);
  matrix2Y->setSingleStep(2);
  QLabel *spinBoxTable2YLabel = new QLabel("Y:");
  QLabel *spinBoxTable2XLabel = new QLabel("X:");
  table2 = new QTableWidget();
  QPushButton *fillTable2Button = new QPushButton();
  fillTable2Button->setText("Fill table");
  QPushButton *applyEdgeMatrixFilter =
      new QPushButton("Apply edge matrix filter");
  QPushButton *applySobel = new QPushButton("Apply sobel");

  QLabel *sigma = new QLabel("gauss sigma:");
  sigmaCannySpin = new QSpinBox();
  sigmaCannySpin->setValue(1);
  QLabel *thigh = new QLabel("tHigh:");
  thighSpin = new QSpinBox();
  thighSpin->setMaximum(255);
  thighSpin->setValue(196);
  QLabel *tlow = new QLabel("tLow:");
  tlowSpin = new QSpinBox();
  tlowSpin->setMaximum(255);
  QPushButton *cannyButton = new QPushButton("Apply canny");
  QGroupBox *groupBoxCanny = new QGroupBox(tr("Canny Edge Algorithm"));
  QHBoxLayout *cannyhBox = new QHBoxLayout();
  cannyhBox->addWidget(sigma);
  cannyhBox->addWidget(sigmaCannySpin);
  cannyhBox->addWidget(thigh);
  cannyhBox->addWidget(thighSpin);
  cannyhBox->addWidget(tlow);
  cannyhBox->addWidget(tlowSpin);
  cannyhBox->addWidget(cannyButton);
  groupBoxCanny->setLayout(cannyhBox);

  QLabel *sigmaUSM = new QLabel("gauss sigma:");
  sigmaUSMSpin = new QSpinBox();
  USMfactorSlider = new QSlider(Qt::Horizontal, this);
  USMfactorSlider->setRange(0, 40);
  USMfactorSlider->setSliderPosition(10);
  QGroupBox *groupBoxUSM = new QGroupBox(tr("USM sharping"));
  QHBoxLayout *usmhBox = new QHBoxLayout();

  usmhBox->addWidget(sigmaUSM);
  usmhBox->addWidget(sigmaUSMSpin);
  usmhBox->addWidget(USMfactorSlider);
  groupBoxUSM->setLayout(usmhBox);

  m_option_layout4->addWidget(spinBoxTable2XLabel);
  m_option_layout4->addWidget(matrix2X);
  m_option_layout4->addWidget(spinBoxTable2YLabel);
  m_option_layout4->addWidget(matrix2Y);
  m_option_layout4->addWidget(matrix2Button);
  m_option_layout4->addWidget(table2);
  m_option_layout4->addWidget(applyEdgeMatrixFilter);
  m_option_layout4->addWidget(applySobel);
  m_option_layout4->addWidget(groupBoxCanny);
  m_option_layout4->addWidget(groupBoxUSM);

  QObject::connect(matrix2Button, SIGNAL(clicked()), this,
                   SLOT(createMatrix2()));
  QObject::connect(matrix2X, SIGNAL(valueChanged(int)), this,
                   SLOT(onSpinEditFinished()));
  QObject::connect(matrix2Y, SIGNAL(valueChanged(int)), this,
                   SLOT(onSpinEditFinished()));
  QObject::connect(fillTable2Button, SIGNAL(clicked()), this,
                   SLOT(fillTable2()));
  QObject::connect(applyEdgeMatrixFilter, SIGNAL(clicked()), this,
                   SLOT(edgeMatrixFilter()));
  QObject::connect(applySobel, SIGNAL(clicked()), this, SLOT(sobelFilter()));
  QObject::connect(cannyButton, SIGNAL(clicked()), this, SLOT(cannyEdge()));
  QObject::connect(USMfactorSlider, SIGNAL(valueChanged(int)), this,
                   SLOT(unsharpMaskingSharpener()));

  // Übung 5
  m_option_panel5 = new QWidget();
  m_option_layout5 = new QVBoxLayout();
  m_option_panel5->setLayout(m_option_layout5);

  QLabel *AminLabel = new QLabel("Amin (min. accum. count per line)");
  AminSpinner = new QSpinBox();
  AminSpinner->setMaximum(15000);
  AminSpinner->setValue(220);
  QLabel *omegaHoughLabel = new QLabel("Omega (effective line thickness)");
  omegaHough = new QDoubleSpinBox();
  omegaHough->setMaximum(20);
  omegaHough->setValue(1);
  omegaHough->setSingleStep(0.5);
  QLabel *mHoughLabel = new QLabel("m or Theta (angular accum. steps)");
  mHough = new QSpinBox();
  mHough->setMaximum(255);
  mHough->setValue(180);
  QLabel *nHoughLabel = new QLabel("n or Rho (radial accum. steps)");
  nHough = new QSpinBox();
  nHough->setMaximum(255);
  nHough->setValue(180);

  m_option_layout5->addWidget(AminLabel);
  m_option_layout5->addWidget(AminSpinner);
  m_option_layout5->addWidget(omegaHoughLabel);
  m_option_layout5->addWidget(omegaHough);
  m_option_layout5->addWidget(mHoughLabel);
  m_option_layout5->addWidget(mHough);
  m_option_layout5->addWidget(nHoughLabel);
  m_option_layout5->addWidget(nHough);

  houghButton = new QPushButton();
  houghButton->setText("Hough transform");
  m_option_layout5->addWidget(houghButton);

  QObject::connect(houghButton, SIGNAL(clicked()), this,
                   SLOT(houghTransform()));

  // Add tabs
  tabWidget->addTab(m_option_panel2, "Übung 2");
  tabWidget->addTab(m_option_panel3, "Übung 3");
  tabWidget->addTab(m_option_panel4, "Übung 4");
  tabWidget->addTab(m_option_panel5, "Übung 5");
  tabWidget->show();
}
