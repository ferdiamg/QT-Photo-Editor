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

#include <QtWidgets>
#ifndef QT_NO_PRINTER
#include <QPrintDialog>
#endif
#include <iostream>
#include <string>
#include <vector>

#include "imageviewer-qt5.h"

ImageViewer::ImageViewer() {

  image = NULL;
  EmagGlobal = NULL;
  resize(1600, 600);

  startLogging();

  generateMainGui();
  renewLogging();

  generateControlPanels();
  createActions();
  createMenus();

  resize(QGuiApplication::primaryScreen()->availableSize() * 0.85);
}

void ImageViewer::applyExampleAlgorithm() {
  if (image != NULL) {
    for (int i = 0; i < std::min(image->width(), image->height()); i++) {
      // macht die Farbe schwarz, bitte recherchieren wie eine andere Farbe
      // gesetzt wird ...
      image->setPixel(i, i, 0);
    }
    updateImageDisplay();
    logFile << "Example algorithm applied!" << std::endl;
    renewLogging();
  }
}

// Für Algorithmen siehe myAlgorithms.cpp

/****************************************************************************************
 *
 *Mit dieser Methode können sie sich pro Aufgabe ein Tab anlegen, in der die
 *Ein- stellungen per Slider, Button, Checkbox etc. gemacht werden und die zu
 *implementierenden Algorithmen gestartet werden.
 *
 *****************************************************************************************/

// siehe generateControlPanels.cpp

/****************************************************************************************
 *
 *   ab hier kommen technische Details, die nicht notwenig für das Verständnis
 *und die Bearbeitung sind.
 *
 *
 *****************************************************************************************/

void ImageViewer::startLogging() {
  // LogFile
  logFile.open("log.txt", std::ios::out);
  logFile << "Logging: \n" << std::endl;
}

void ImageViewer::renewLogging() {
  QFile file("log.txt"); // Create a file handle for the file named
  QString line;
  file.open(QIODevice::ReadOnly); // Open the file

  QTextStream stream(&file); // Set the stream to read from myFile
  logBrowser->clear();
  while (!stream.atEnd()) {

    line = stream.readLine(); // this reads a line (QString) from the file
    logBrowser->append(line);
  }
}

void ImageViewer::resizeEvent(QResizeEvent *event) {
  QMainWindow::resizeEvent(event);
  centralwidget->setMinimumWidth(width());
  centralwidget->setMinimumHeight(height());
  centralwidget->setMaximumWidth(width());
  centralwidget->setMaximumHeight(height());
  logBrowser->setMinimumWidth(width() - 40);
  logBrowser->setMaximumWidth(width() - 40);
}

void ImageViewer::updateImageDisplay() {
  imageLabel->setPixmap(QPixmap::fromImage(*image));
}

void ImageViewer::generateMainGui() {
  /* Tab widget */
  tabWidget = new QTabWidget(this);
  tabWidget->setObjectName(QStringLiteral("tabWidget"));

  /* Center widget */
  centralwidget = new QWidget(this);
  centralwidget->setObjectName(QStringLiteral("centralwidget"));
  // centralwidget->setFixedSize(200,200);
  // setCentralWidget(centralwidget);

  imageLabel = new QLabel;
  imageLabel->setBackgroundRole(QPalette::Base);
  imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
  imageLabel->setScaledContents(true);

  /* Center widget */
  scrollArea = new QScrollArea;
  scrollArea->setBackgroundRole(QPalette::Dark);
  scrollArea->setWidget(imageLabel);

  setCentralWidget(scrollArea);

  /* HBox layout */
  QGridLayout *gLayout = new QGridLayout(centralwidget);
  gLayout->setObjectName(QStringLiteral("hboxLayout"));
  gLayout->addWidget(new QLabel(), 1, 1);
  gLayout->setVerticalSpacing(50);
  gLayout->addWidget(tabWidget, 2, 1);
  gLayout->addWidget(scrollArea, 2, 2);

  logBrowser = new QTextEdit(this);
  logBrowser->setMinimumHeight(100);
  logBrowser->setMaximumHeight(200);
  logBrowser->setMinimumWidth(width());
  logBrowser->setMaximumWidth(width());
  gLayout->addWidget(logBrowser, 3, 1, 1, 2);
  gLayout->setVerticalSpacing(50);
}

bool ImageViewer::loadFile(const QString &fileName) {
  if (image != NULL) {
    delete image;
    image = NULL;
  }

  image = new QImage(fileName);

  if (image->isNull()) {
    QMessageBox::information(
        this, QGuiApplication::applicationDisplayName(),
        tr("Cannot load %1.").arg(QDir::toNativeSeparators(fileName)));
    setWindowFilePath(QString());
    imageLabel->setPixmap(QPixmap());
    imageLabel->adjustSize();
    return false;
  }

  scaleFactor = 1.0;

  updateImageDisplay();

  printAct->setEnabled(true);
  fitToWindowAct->setEnabled(true);
  updateActions();

  if (!fitToWindowAct->isChecked())
    imageLabel->adjustSize();

  setWindowFilePath(fileName);
  logFile << "geladen: " << fileName.toStdString().c_str() << std::endl;
  renewLogging();
  return true;
}

void ImageViewer::open() {
  QStringList mimeTypeFilters;
  foreach (const QByteArray &mimeTypeName, QImageReader::supportedMimeTypes())
    mimeTypeFilters.append(mimeTypeName);
  mimeTypeFilters.sort();
  const QStringList picturesLocations =
      QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
  QFileDialog dialog(this, tr("Open File"),
                     picturesLocations.isEmpty() ? QDir::currentPath()
                                                 : picturesLocations.first());
  dialog.setAcceptMode(QFileDialog::AcceptOpen);
  dialog.setMimeTypeFilters(mimeTypeFilters);
  dialog.selectMimeTypeFilter("image/jpeg");

  while (dialog.exec() == QDialog::Accepted &&
         !loadFile(dialog.selectedFiles().first())) {
  }
}

void ImageViewer::print() {
  Q_ASSERT(imageLabel->pixmap());
#if !defined(QT_NO_PRINTER) && !defined(QT_NO_PRINTDIALOG)
  QPrintDialog dialog(&printer, this);
  if (dialog.exec()) {
    QPainter painter(&printer);
    QRect rect = painter.viewport();
    QSize size = imageLabel->pixmap()->size();
    size.scale(rect.size(), Qt::KeepAspectRatio);
    painter.setViewport(rect.x(), rect.y(), size.width(), size.height());
    painter.setWindow(imageLabel->pixmap()->rect());
    painter.drawPixmap(0, 0, *imageLabel->pixmap());
  }
#endif
}

void ImageViewer::zoomIn() { scaleImage(1.25); }

void ImageViewer::zoomOut() { scaleImage(0.8); }

void ImageViewer::normalSize() {
  imageLabel->adjustSize();
  scaleFactor = 1.0;
}

void ImageViewer::fitToWindow() {
  bool fitToWindow = fitToWindowAct->isChecked();
  scrollArea->setWidgetResizable(fitToWindow);
  if (!fitToWindow) {
    normalSize();
  }
  updateActions();
}

void ImageViewer::about() {
  QMessageBox::about(
      this, tr("About Image Viewer"),
      tr("<p>The <b>Image Viewer</b> example shows how to combine QLabel "
         "and QScrollArea to display an image. QLabel is typically used "
         "for displaying a text, but it can also display an image. "
         "QScrollArea provides a scrolling view around another widget. "
         "If the child widget exceeds the size of the frame, QScrollArea "
         "automatically provides scroll bars. </p><p>The example "
         "demonstrates how QLabel's ability to scale its contents "
         "(QLabel::scaledContents), and QScrollArea's ability to "
         "automatically resize its contents "
         "(QScrollArea::widgetResizable), can be used to implement "
         "zooming and scaling features. </p><p>In addition the example "
         "shows how to use QPainter to print an image.</p>"));
}

void ImageViewer::createActions() {
  openAct = new QAction(tr("&Open..."), this);
  openAct->setShortcut(tr("Ctrl+O"));
  connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

  printAct = new QAction(tr("&Print..."), this);
  printAct->setShortcut(tr("Ctrl+P"));
  printAct->setEnabled(false);
  connect(printAct, SIGNAL(triggered()), this, SLOT(print()));

  exitAct = new QAction(tr("E&xit"), this);
  exitAct->setShortcut(tr("Ctrl+Q"));
  connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

  zoomInAct = new QAction(tr("Zoom &In (25%)"), this);
  zoomInAct->setShortcut(tr("Ctrl++"));
  zoomInAct->setEnabled(false);
  connect(zoomInAct, SIGNAL(triggered()), this, SLOT(zoomIn()));

  zoomOutAct = new QAction(tr("Zoom &Out (25%)"), this);
  zoomOutAct->setShortcut(tr("Ctrl+-"));
  zoomOutAct->setEnabled(false);
  connect(zoomOutAct, SIGNAL(triggered()), this, SLOT(zoomOut()));

  normalSizeAct = new QAction(tr("&Normal Size"), this);
  normalSizeAct->setShortcut(tr("Ctrl+S"));
  normalSizeAct->setEnabled(false);
  connect(normalSizeAct, SIGNAL(triggered()), this, SLOT(normalSize()));

  fitToWindowAct = new QAction(tr("&Fit to Window"), this);
  fitToWindowAct->setEnabled(false);
  fitToWindowAct->setCheckable(true);
  fitToWindowAct->setShortcut(tr("Ctrl+F"));
  connect(fitToWindowAct, SIGNAL(triggered()), this, SLOT(fitToWindow()));

  aboutAct = new QAction(tr("&About"), this);
  connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

  aboutQtAct = new QAction(tr("About &Qt"), this);
  connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

void ImageViewer::createMenus() {
  fileMenu = new QMenu(tr("&File"), this);
  fileMenu->addAction(openAct);
  fileMenu->addAction(printAct);
  fileMenu->addSeparator();
  fileMenu->addAction(exitAct);

  viewMenu = new QMenu(tr("&View"), this);
  viewMenu->addAction(zoomInAct);
  viewMenu->addAction(zoomOutAct);
  viewMenu->addAction(normalSizeAct);
  viewMenu->addSeparator();
  viewMenu->addAction(fitToWindowAct);

  helpMenu = new QMenu(tr("&Help"), this);
  helpMenu->addAction(aboutAct);
  helpMenu->addAction(aboutQtAct);

  menuBar()->addMenu(fileMenu);
  menuBar()->addMenu(viewMenu);
  menuBar()->addMenu(helpMenu);
}

void ImageViewer::updateActions() {
  zoomInAct->setEnabled(!fitToWindowAct->isChecked());
  zoomOutAct->setEnabled(!fitToWindowAct->isChecked());
  normalSizeAct->setEnabled(!fitToWindowAct->isChecked());
}

void ImageViewer::scaleImage(double factor) {
  Q_ASSERT(imageLabel->pixmap());
  scaleFactor *= factor;
  imageLabel->resize(scaleFactor * imageLabel->pixmap()->size());

  adjustScrollBar(scrollArea->horizontalScrollBar(), factor);
  adjustScrollBar(scrollArea->verticalScrollBar(), factor);

  zoomInAct->setEnabled(scaleFactor < 10.0);
  zoomOutAct->setEnabled(scaleFactor > 0.05);
}

void ImageViewer::adjustScrollBar(QScrollBar *scrollBar, double factor) {
  scrollBar->setValue(int(factor * scrollBar->value() +
                          ((factor - 1) * scrollBar->pageStep() / 2)));
}
