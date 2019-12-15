#include <QtWidgets>
#ifndef QT_NO_PRINTER
#include <QPrintDialog>
#endif
#include <iostream>
#include <string>
#include <vector>
using namespace std;
#include "imageviewer-qt5.h"

/****************************************************************************************
 *
 * NIEMALS das main "image" manipulieren, immer nur als Ausgangsimage für die
 * Algorithmen verwenden und dann als newLayer für imageLabel setzen.
 *
 *****************************************************************************************/

void ImageViewer::uebung1c() {
  if (image != NULL) {
    for (int i = 0; i < std::min(image->width(), image->height()); i++) {
      image->setPixel(i, i, qRgb(255, 0, 0));
    }
    updateImageDisplay();
  }
}

void ImageViewer::checkBoxMethod() {
  if (image != NULL) {
    if (checkbox1->isChecked()) {
      QImage backup = image->copy();
      QImage *newLayer = &backup;
      Q_ASSERT(newLayer->format() == QImage::Format_RGB32);
      for (int ii = 0; ii < newLayer->height(); ii++) {
        QRgb *pixel = reinterpret_cast<QRgb *>(newLayer->scanLine(ii));
        QRgb *end = pixel + newLayer->width();
        for (; pixel != end; pixel++) {
          int gray = qGray(*pixel);
          *pixel = QColor(gray, gray, gray).rgb();
        }
      }
      imageLabel->setPixmap(QPixmap::fromImage(*newLayer));
    } else {
      updateImageDisplay();
    }
  }
}

void ImageViewer::sliderMethod() {
  if (image != NULL) {
    QImage newLayer = image->copy();
    slider1->setRange(0, std::min(image->width(), image->height()) / 2);
    int middle = std::min(image->width(), image->height()) / 2;
    for (int i = middle - slider1->value(); i < middle + slider1->value();
         i++) {
      newLayer.setPixel(i, i, qRgb(255, 0, 0));
      newLayer.setPixel(std::min(image->width(), image->height()) - i, i,
                        qRgb(255, 0, 0));
    }
    imageLabel->setPixmap(QPixmap::fromImage(newLayer));
  }
}

// converts main image to gray
void ImageViewer::convertToGray() {
  for (int y = 0; y < image->width(); ++y) {
    for (int x = 0; x < image->height(); ++x) {
      double newcolor = (0.299 * image->pixelColor(x, y).red()) +
                        (0.587 * image->pixelColor(x, y).green()) +
                        (0.114 * image->pixelColor(x, y).blue());
      int converted = (int)newcolor;
      image->setPixelColor(x, y, QColor(converted, converted, converted));
    }
  }
  updateImageDisplay();
}

// makes a copy of main image gray (leaves main image unaltered)
QImage ImageViewer::copyConvertToGray() {
  QImage copy = image->copy();
  QImage *greyImage = &copy;
  for (int y = 0; y < image->width(); ++y) {
    for (int x = 0; x < image->height(); ++x) {
      double newcolor = (0.299 * greyImage->pixelColor(x, y).red()) +
                        (0.587 * greyImage->pixelColor(x, y).green()) +
                        (0.114 * greyImage->pixelColor(x, y).blue());
      int converted = (int)newcolor;
      greyImage->setPixelColor(x, y, QColor(converted, converted, converted));
    }
  }
  return *greyImage;
}

// makes a given image grey and returns it
QImage ImageViewer::greyify(QImage toGreifyImage) {
  for (int y = 0; y < toGreifyImage.width(); ++y) {
    for (int x = 0; x < toGreifyImage.height(); ++x) {
      double newcolor = (0.299 * toGreifyImage.pixelColor(x, y).red()) +
                        (0.587 * toGreifyImage.pixelColor(x, y).green()) +
                        (0.114 * toGreifyImage.pixelColor(x, y).blue());
      int converted = (int)newcolor;
      toGreifyImage.setPixelColor(x, y,
                                  QColor(converted, converted, converted));
    }
  }
  return toGreifyImage;
}

void ImageViewer::calcVarianz() {
  if (image != NULL) {
    varianz->setText("Variance: " + QString::number(calcEchteVarianz()));
  }
}

int ImageViewer::calcEchteVarianz() {
  const QPixmap *pixmap = imageLabel->pixmap();
  QImage displayImage = pixmap->toImage();
  QImage makeGrey = greyify(displayImage);
  QImage newImage = makeGrey;

  int M = newImage.width();
  int N = newImage.height();
  int sum = 0;
  for (int y = 0; y <= M - 1; ++y) {
    for (int x = 0; x <= N - 1; ++x) {
      sum += newImage.pixelColor(x, y).green();
    }
  }
  return sum / (M * N);
}

void ImageViewer::calcAvgHelligkeit() {
  if (image != NULL) {
    const QPixmap *pixmap = imageLabel->pixmap();
    QImage displayImage = pixmap->toImage();
    QImage makeGrey = greyify(displayImage);
    QImage newImage = makeGrey;

    int M = newImage.width();
    int N = newImage.height();
    int sum = 0;
    int varianz = calcEchteVarianz();

    //    r = x
    //    c = y
    for (int y = 0; y <= M - 1; ++y) {
      for (int x = 0; x <= N - 1; ++x) {
        sum += qPow((newImage.pixelColor(x, y).green() - varianz), 2);
      }
    }
    int output = sum / (M * N);
    avgHelligkeit->setText("avg. Brightness: " + QString::number(output));
  }
}

void ImageViewer::createHistogram() {
  if (image != NULL) {
    const QPixmap *pixmap = imageLabel->pixmap();
    QImage displayImage = pixmap->toImage();
    QImage makeGrey = greyify(displayImage);
    QImage newImage = makeGrey;

    // Array von 0 bis 255 mit 0 fuellen:
    int brightnessCounts[256];
    for (int i = 0; i <= 255; i++) {
      brightnessCounts[i] = 0;
    }

    int M = newImage.width();
    int N = newImage.height();
    for (int y = 0; y <= M - 1; ++y) {
      for (int x = 0; x <= N - 1; ++x) {
        brightnessCounts[newImage.pixelColor(x, y).green()] =
            brightnessCounts[newImage.pixelColor(x, y).green()] + 1;
      }
    }

    int maximum = INT_MIN;
    for (int i = 0; i <= 255; i++) {
      if (brightnessCounts[i] > maximum) {
        maximum = brightnessCounts[i];
      }
    }

    QPicture pi;
    QPainter p(&pi);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(Qt::green, 1));

    // Histogram lines from 0-255 (x-Axis) with 300px height (y-Axis)
    const int HISTOGRAM_HEIGHT = 70;
    p.drawLine(0, 0, 0, HISTOGRAM_HEIGHT);
    p.drawLine(255, 0, 255, HISTOGRAM_HEIGHT);
    p.drawLine(0, 0, 255, 0);

    p.setPen(QPen(Qt::red, 1));

    if (linear->isChecked()) {
      double fictiveHeight = HISTOGRAM_HEIGHT * 0.9;
      for (int i = 0; i <= 255; i++) {
        // Calculate line height to not exceed HISTOGRAM_HEIGHT pixels
        int lineHeight = brightnessCounts[i] * (fictiveHeight / maximum);
        p.drawLine(i, HISTOGRAM_HEIGHT, i, HISTOGRAM_HEIGHT - lineHeight);
      }
      p.end();
    } else {
      double fictiveHeight = HISTOGRAM_HEIGHT * 0.9;
      for (int i = 0; i <= 255; i++) {
        // Calculate line height to not exceed HISTOGRAM_HEIGHT pixels
        int newBrightnesscount;
        if (brightnessCounts[i] == 0) {
          newBrightnesscount = 0;
        } else {
          newBrightnesscount = qLn(brightnessCounts[i]) * 100;
        }

        int lineHeight = newBrightnesscount * (fictiveHeight / maximum);
        p.drawLine(i, HISTOGRAM_HEIGHT, i, (HISTOGRAM_HEIGHT - lineHeight));
      }
      p.end();
    }

    histogramImage->setPicture(pi);
  }
}

int ImageViewer::getY(QColor pixelColor) {
  double Y = (0.299 * pixelColor.red()) + (0.587 * pixelColor.green()) +
             (0.114 * pixelColor.blue());
  return std::max(0, std::min(255, (int)Y));
}

int ImageViewer::getCb(QColor pixelColor) {
  double Cb = 128 - (0.168736 * pixelColor.red()) -
              (0.331264 * pixelColor.green()) + (0.5 * pixelColor.blue());
  return std::max(0, std::min(255, (int)Cb));
}

int ImageViewer::getCr(QColor pixelColor) {
  double Cr = 128 + (0.5 * pixelColor.red()) - (0.418688 * pixelColor.green()) -
              (0.081312 * pixelColor.blue());
  return std::max(0, std::min(255, (int)Cr));
}

QColor ImageViewer::YCbCrtoRGB(int Y, int Cb, int Cr) {

  int R = (int)Y + 1.402 * (Cr - 128);
  int G = (int)Y - 0.344136 * (Cb - 128) - 0.714136 * (Cr - 128);
  int B = (int)Y + 1.772 * (Cb - 128);

  R = std::max(0, std::min(255, R));
  G = std::max(0, std::min(255, G));
  B = std::max(0, std::min(255, B));

  return QColor(R, G, B);
}

void ImageViewer::adjustKontrast() {
  if (image != NULL) {
    QImage imageCopy = image->copy();
    // Min und Max Value erhalten
    int maximum = INT_MIN;
    int minimum = INT_MAX;
    for (int y = 0; y < imageCopy.width(); ++y) {
      for (int x = 0; x < imageCopy.height(); ++x) {
        int Y = getY(imageCopy.pixelColor(x, y));

        if (maximum < Y) {
          maximum = Y;
        }
        if (Y < minimum) {
          minimum = Y;
        }
      }
    }

    int diff = (maximum + minimum) / 2;
    for (int y = 0; y < imageCopy.width(); ++y) {
      for (int x = 0; x < imageCopy.height(); ++x) {
        QColor pixelColor = imageCopy.pixelColor(x, y);
        int Y = getY(pixelColor);
        int Cb = getCb(pixelColor);
        int Cr = getCr(pixelColor);

        int curVal = Y - diff;
        int newColor =
            (int)((curVal * (kontrastSlider->value() / 100.0)) + 0.5);
        newColor += diff;
        if (newColor > 255) {
          newColor = 255;
        } else if (newColor < 0) {
          newColor = 0;
        }
        imageCopy.setPixelColor(x, y, YCbCrtoRGB(newColor, Cb, Cr));
      }
    }

    imageLabel->setPixmap(QPixmap::fromImage(imageCopy));
  }
}

void ImageViewer::adjustBrightness() {
  if (image != NULL) {
    QImage imageCopy = image->copy();
    for (int y = 0; y < imageCopy.width(); ++y) {
      for (int x = 0; x < imageCopy.height(); ++x) {

        QColor pixelColor = imageCopy.pixelColor(x, y);
        int Y = getY(pixelColor);
        int Cb = getCb(pixelColor);
        int Cr = getCr(pixelColor);

        int newY = (int)(Y + (helligkeitSlider->value() / 10) + 0.5);
        QColor finalRGBcolor = YCbCrtoRGB(newY, Cb, Cr);
        imageCopy.setPixelColor(x, y, finalRGBcolor);
      }
    }
    imageLabel->setPixmap(QPixmap::fromImage(imageCopy));
  }
}

void ImageViewer::robAutoKontrast() {
  if (image != NULL) {
    const int A_MIN = 0;
    const int A_MAX = 255;
    int alowPercentage = pixelsLowSpinbox->value();
    int ahighPercantage = 100 - pixelsHighSpinbox->value();
    QImage imageCopy = image->copy();

    // Array Werte mit 0 überschreiben, C++ Ding
    int brightnessCounts[256];
    int cumulative[256];
    for (int i = 0; i <= 255; i++) {
      brightnessCounts[i] = 0;
      cumulative[i] = 0;
    }

    // Zählen wie oft bestimmter Helligkeitswert in Bild vorkommt (Histogramm)
    for (int y = 0; y < imageCopy.width(); ++y) {
      for (int x = 0; x < imageCopy.height(); ++x) {
        int Y = getY(imageCopy.pixelColor(x, y));
        brightnessCounts[Y] = brightnessCounts[Y] + 1;
      }
    }

    // Kumulatives Histogramm erstellen
    for (int i = 0; i < 256; ++i) {
      for (int j = 0; j < i; ++j) {
        cumulative[i] += brightnessCounts[j];
      }
    }

    // Berechnungen
    int alow = 0;
    int ahigh = 0;

    int amountPixels = imageCopy.width() * imageCopy.height();
    int amountPixelsforaLow = (amountPixels * alowPercentage) / 100;
    int amountPixelsforaHigh = (amountPixels * ahighPercantage) / 100;

    // Erhalte Index (von 0-255) in dem X% aller zu schwärzenden Pixel liegen
    for (int i = 0; i < 256; ++i) {
      if (cumulative[i] >= amountPixelsforaLow) {
        alow = i;
        break;
      }
    }

    // Erhalte Index von (0-255) in dem X% aller zu erhellenden Pixel liegen
    for (int i = 0; i < 256; ++i) {
      if (cumulative[i] >= amountPixelsforaHigh) {
        ahigh = i - 1;
        break;
      }
    }

    // Formel der robusten automatischen Kontrastanpassung MIT Verschiebung
    // (macht die auto.rob. schon selber)
    for (int y = 0; y < imageCopy.width(); ++y) {
      for (int x = 0; x < imageCopy.height(); ++x) {
        QColor pixelColor = imageCopy.pixelColor(x, y);
        int Y = getY(pixelColor);
        int Cb = getCb(pixelColor);
        int Cr = getCr(pixelColor);

        if (Y <= alow) {
          imageCopy.setPixelColor(x, y, QColor(A_MIN, A_MIN, A_MIN));
        } else if ((Y > alow) && (Y <= ahigh)) {
          int newColor =
              A_MIN + (Y - alow) * ((A_MAX - A_MIN) / (ahigh - alow));
          imageCopy.setPixelColor(x, y, YCbCrtoRGB(newColor, Cb, Cr));
        } else if (Y >= ahigh) {
          imageCopy.setPixelColor(x, y, QColor(A_MAX, A_MAX, A_MAX));
        }
      }
    }
    imageLabel->setPixmap(QPixmap::fromImage(imageCopy));
  }
}

void ImageViewer::createMatrix() {
  table->setColumnCount(matrixX->value());
  table->setRowCount(matrixY->value());
}

void ImageViewer::createMatrix2() {
  table2->setColumnCount(matrix2X->value());
  table2->setRowCount(matrix2Y->value());
}

void ImageViewer::onSpinEditFinished() {
  // Only allow odd values for QSpinBoxes
  int val = matrixX->value();
  if (val % 2 == 0) {
    matrixX->setValue(val - 1);
  } else if (val >= 15) {
    matrixX->setValue(15);
  }

  int val2 = matrixY->value();
  if (val2 % 2 == 0) {
    matrixY->setValue(val2 - 1);
  } else if (val2 >= 15) {
    matrixY->setValue(15);
  }
}

void ImageViewer::onSpinEditFinished2() {
  // Only allow odd values for QSpinBoxes
  int val = matrix2X->value();
  if (val % 2 == 0) {
    matrix2X->setValue(val - 1);
  } else if (val >= 15) {
    matrix2X->setValue(15);
  }

  int val2 = matrix2Y->value();
  if (val2 % 2 == 0) {
    matrix2Y->setValue(val2 - 1);
  } else if (val2 >= 15) {
    matrix2Y->setValue(15);
  }
}

void ImageViewer::fillTable() {
  int counter = 0;
  for (int i = 0; i < table->rowCount(); i++) {
    for (int j = 0; j < table->columnCount(); j++) {
      QTableWidgetItem *pCell = table->item(i, j);
      if (!pCell) {
        pCell = new QTableWidgetItem;
        table->setItem(i, j, pCell);
      }
      counter++;
      pCell->setText(QString::number(1));
    }
  }
}

void ImageViewer::fillTable2() {
  int counter = 0;
  for (int i = 0; i < table2->rowCount(); i++) {
    for (int j = 0; j < table2->columnCount(); j++) {
      QTableWidgetItem *pCell = table2->item(i, j);
      if (!pCell) {
        pCell = new QTableWidgetItem;
        table2->setItem(i, j, pCell);
      }
      counter++;
      pCell->setText(QString::number(1));
    }
  }
}

void ImageViewer::applyFilter() {
  if (image != NULL) {
    QImage imageCopy = image->copy();

    int X = table->rowCount();
    int Y = table->columnCount();
    int filterValues[X][Y];

    // Filter-Matrix aus UI auslesen und in filterValues einfügen und
    // gleichzeitig Summe berechnen
    int matrixSum = 0;
    for (int i = 0; i < X; ++i) {
      for (int j = 0; j < Y; ++j) {
        filterValues[i][j] = table->item(i, j)->text().toInt();
        matrixSum += table->item(i, j)->text().toInt();
      }
    }
    double s = 1.0 / matrixSum;

    int K = X / 2;
    int L = Y / 2;

    for (int v = L; v <= imageCopy.width() - L - 1; v++) {
      for (int u = K; u <= imageCopy.height() - K - 1; u++) {
        int sumY = 0;
        int sumCb = 0;
        int sumCr = 0;
        for (int j = -L; j <= L; j++) {
          for (int i = -K; i <= K; i++) {
            QColor pixelColor = imageCopy.pixelColor(u + i, v + j);
            int Y = getY(pixelColor);
            int Cb = getCb(pixelColor);
            int Cr = getCr(pixelColor);
            int c = filterValues[j + L][i + K];
            sumY = sumY + c * Y;
            sumCb = sumCb + c * Cb;
            sumCr = sumCr + c * Cr;
          }
        }
        int qY = (int)std::round(s * sumY);
        if (qY < 0)
          qY = 0;
        if (qY > 255)
          qY = 255;
        int qCb = (int)std::round(s * sumCb);
        if (qCb < 0)
          qCb = 0;
        if (qCb > 255)
          qCb = 255;
        int qCr = (int)std::round(s * sumCr);
        if (qCr < 0)
          qCr = 0;
        if (qCr > 255)
          qCr = 255;
        imageCopy.setPixelColor(u, v, YCbCrtoRGB(qY, qCb, qCr));
      }
    }
    imageLabel->setPixmap(QPixmap::fromImage(imageCopy));
  }
}

void ImageViewer::applyzeroPaddFilter() {
  if (image != NULL) {
    QImage imageCopy = image->copy();

    int X = table->rowCount();
    int Y = table->columnCount();
    int filterValues[X][Y];

    // Filter-Matrix aus UI auslesen und in filterValues einfügen und
    // gleichzeitig Summe berechnen
    int matrixSum = 0;
    for (int i = 0; i < X; ++i) {
      for (int j = 0; j < Y; ++j) {
        filterValues[i][j] = table->item(i, j)->text().toInt();
        matrixSum += table->item(i, j)->text().toInt();
      }
    }
    double s = 1.0 / matrixSum;

    int K = X / 2;
    int L = Y / 2;

    for (int v = 0; v < imageCopy.width(); v++) {
      for (int u = 0; u < imageCopy.height(); u++) {
        int sumY = 0;
        int sumCb = 0;
        int sumCr = 0;
        for (int j = -L; j <= L; j++) {
          for (int i = -K; i <= K; i++) {
            QColor pixelColor;
            if (u + i >= imageCopy.width() || v + j >= imageCopy.height() ||
                u + i < 0 || v + j < 0) {
              // Padding color
              pixelColor = QColor(128, 128, 128);
            } else {
              pixelColor = imageCopy.pixelColor(u + i, v + j);
            }
            int Y = getY(pixelColor);
            int Cb = getCb(pixelColor);
            int Cr = getCr(pixelColor);
            int c = filterValues[j + L][i + K];
            sumY = sumY + c * Y;
            sumCb = sumCb + c * Cb;
            sumCr = sumCr + c * Cr;
          }
        }
        int qY = (int)std::round(s * sumY);
        if (qY < 0)
          qY = 0;
        if (qY > 255)
          qY = 255;
        int qCb = (int)std::round(s * sumCb);
        if (qCb < 0)
          qCb = 0;
        if (qCb > 255)
          qCb = 255;
        int qCr = (int)std::round(s * sumCr);
        if (qCr < 0)
          qCr = 0;
        if (qCr > 255)
          qCr = 255;
        imageCopy.setPixelColor(u, v, YCbCrtoRGB(qY, qCb, qCr));
      }
    }
    imageLabel->setPixmap(QPixmap::fromImage(imageCopy));
  }
}

void ImageViewer::applyconstantFilter() {
  if (image != NULL) {
    QImage imageCopy = image->copy();

    int X = table->rowCount();
    int Y = table->columnCount();
    int filterValues[X][Y];

    // Filter-Matrix aus UI auslesen und in filterValues einfügen und
    // gleichzeitig Summe berechnen
    int matrixSum = 0;
    for (int i = 0; i < X; ++i) {
      for (int j = 0; j < Y; ++j) {
        filterValues[i][j] = table->item(i, j)->text().toInt();
        matrixSum += table->item(i, j)->text().toInt();
      }
    }
    double s = 1.0 / matrixSum;

    int K = X / 2;
    int L = Y / 2;

    for (int v = 0; v < imageCopy.width(); v++) {
      for (int u = 0; u < imageCopy.height(); u++) {
        int sumY = 0;
        int sumCb = 0;
        int sumCr = 0;
        for (int j = -L; j <= L; j++) {
          for (int i = -K; i <= K; i++) {
            QColor pixelColor;
            if (u + i >= imageCopy.width() || v + j >= imageCopy.height() ||
                u + i < 0 || v + j < 0) {
              pixelColor = imageCopy.pixelColor(u, v);
            } else {
              pixelColor = imageCopy.pixelColor(u + i, v + j);
            }

            int Y = getY(pixelColor);
            int Cb = getCb(pixelColor);
            int Cr = getCr(pixelColor);
            int c = filterValues[j + L][i + K];
            sumY = sumY + c * Y;
            sumCb = sumCb + c * Cb;
            sumCr = sumCr + c * Cr;
          }
        }
        int qY = (int)std::round(s * sumY);
        if (qY < 0)
          qY = 0;
        if (qY > 255)
          qY = 255;
        int qCb = (int)std::round(s * sumCb);
        if (qCb < 0)
          qCb = 0;
        if (qCb > 255)
          qCb = 255;
        int qCr = (int)std::round(s * sumCr);
        if (qCr < 0)
          qCr = 0;
        if (qCr > 255)
          qCr = 255;
        imageCopy.setPixelColor(u, v, YCbCrtoRGB(qY, qCb, qCr));
      }
    }
    imageLabel->setPixmap(QPixmap::fromImage(imageCopy));
  }
}

void ImageViewer::applymirrorFilter() {
  if (image != NULL) {
    QImage imageCopy = image->copy();

    int X = table->rowCount();
    int Y = table->columnCount();
    int filterValues[X][Y];

    // Filter-Matrix aus UI auslesen und in filterValues einfügen und
    // gleichzeitig Summe berechnen
    int matrixSum = 0;
    for (int i = 0; i < X; ++i) {
      for (int j = 0; j < Y; ++j) {
        filterValues[i][j] = table->item(i, j)->text().toInt();
        matrixSum += table->item(i, j)->text().toInt();
      }
    }
    double s = 1.0 / matrixSum;

    int K = X / 2;
    int L = Y / 2;

    for (int v = 0; v < imageCopy.width(); v++) {
      for (int u = 0; u < imageCopy.height(); u++) {
        int sumY = 0;
        int sumCb = 0;
        int sumCr = 0;
        for (int j = -L; j <= L; j++) {
          for (int i = -K; i <= K; i++) {
            QColor pixelColor;

            if (u + i >= imageCopy.width()) {
              pixelColor = imageCopy.pixelColor(u - i, v);
            } else if (v + j >= imageCopy.height()) {
              pixelColor = imageCopy.pixelColor(u, v - j);
            } else if (u + i < 0) {
              pixelColor = imageCopy.pixelColor(u + std::abs(i), v);
            } else if (v + j < 0) {
              pixelColor = imageCopy.pixelColor(u, v + std::abs(j));
            } else {
              pixelColor = imageCopy.pixelColor(u + i, v + j);
            }

            int Y = getY(pixelColor);
            int Cb = getCb(pixelColor);
            int Cr = getCr(pixelColor);
            int c = filterValues[j + L][i + K];
            sumY = sumY + c * Y;
            sumCb = sumCb + c * Cb;
            sumCr = sumCr + c * Cr;
          }
        }
        int qY = (int)std::round(s * sumY);
        if (qY < 0)
          qY = 0;
        if (qY > 255)
          qY = 255;
        int qCb = (int)std::round(s * sumCb);
        if (qCb < 0)
          qCb = 0;
        if (qCb > 255)
          qCb = 255;
        int qCr = (int)std::round(s * sumCr);
        if (qCr < 0)
          qCr = 0;
        if (qCr > 255)
          qCr = 255;
        imageCopy.setPixelColor(u, v, YCbCrtoRGB(qY, qCb, qCr));
      }
    }
    imageLabel->setPixmap(QPixmap::fromImage(imageCopy));
  }
}

vector<int> ImageViewer::makeGaussKernel1D(double sigma) {
  // kernel h
  int center = (int)(3.0 * sigma);
  vector<float> h(2 * center + 1);
  double sigma2 = sigma * sigma;

  for (int i = 0; i < 2 * center + 1; i++) {
    double r = center - i;
    h.at(i) = (float)std::exp(-0.5 * (r * r) / sigma2);
  }

  vector<int> exportVector(2 * center + 1);
  for (vector<float>::size_type i = 0; i != h.size(); i++) {
    exportVector.at(i) = (int)(h[i] * 10 + 0.5);
  }

  return exportVector;
}

void ImageViewer::applygaussFilter() {
  if (image != NULL) {
    QImage imageCopy = image->copy();

    int sigma = sigmaSpin->value();
    vector<int> gauss1Dfilter = makeGaussKernel1D(sigma);

    int gaussFilterCenter = gauss1Dfilter.size() / 2;

    // Filter-Matrix aus UI auslesen und in filterValues einfügen und
    // gleichzeitig Summe berechnen
    int matrixSum = 0;
    for (vector<int>::size_type i = 0; i != gauss1Dfilter.size(); i++) {
      matrixSum += gauss1Dfilter[i];
    }
    double s = 1.0 / matrixSum;

    // Iterate along X
    for (int v = 0; v < imageCopy.width(); v++) {
      for (int u = 0; u < imageCopy.height(); u++) {
        int sumY = 0;
        int sumCb = 0;
        int sumCr = 0;
        for (int i = -gaussFilterCenter; i <= gaussFilterCenter; i++) {
          QColor pixelColor;
          if (u + i >= imageCopy.width() || u + i < 0) {
            pixelColor = imageCopy.pixelColor(u, v);
          } else {
            pixelColor = imageCopy.pixelColor(u + i, v);
          }
          int Y = getY(pixelColor);
          int Cb = getCb(pixelColor);
          int Cr = getCr(pixelColor);
          float c = gauss1Dfilter[i + gaussFilterCenter];
          sumY = sumY + c * Y;
          sumCb = sumCb + c * Cb;
          sumCr = sumCr + c * Cr;
        }
        int qY = (int)std::round(s * sumY);
        if (qY < 0)
          qY = 0;
        if (qY > 255)
          qY = 255;
        int qCb = (int)std::round(s * sumCb);
        if (qCb < 0)
          qCb = 0;
        if (qCb > 255)
          qCb = 255;
        int qCr = (int)std::round(s * sumCr);
        if (qCr < 0)
          qCr = 0;
        if (qCr > 255)
          qCr = 255;
        imageCopy.setPixelColor(u, v, YCbCrtoRGB(qY, qCb, qCr));
      }
    }

    // Iterate along Y
    for (int u = 0; u < imageCopy.height(); u++) {
      for (int v = 0; v < imageCopy.width(); v++) {
        int sumY = 0;
        int sumCb = 0;
        int sumCr = 0;
        for (int i = -gaussFilterCenter; i <= gaussFilterCenter; i++) {
          QColor pixelColor;
          if (v + i >= imageCopy.height() || v + i < 0) {
            pixelColor = imageCopy.pixelColor(u, v);
          } else {
            pixelColor = imageCopy.pixelColor(u, v + i);
          }
          int Y = getY(pixelColor);
          int Cb = getCb(pixelColor);
          int Cr = getCr(pixelColor);
          float c = gauss1Dfilter[i + gaussFilterCenter];
          sumY = sumY + c * Y;
          sumCb = sumCb + c * Cb;
          sumCr = sumCr + c * Cr;
        }
        int qY = (int)std::round(s * sumY);
        if (qY < 0)
          qY = 0;
        if (qY > 255)
          qY = 255;
        int qCb = (int)std::round(s * sumCb);
        if (qCb < 0)
          qCb = 0;
        if (qCb > 255)
          qCb = 255;
        int qCr = (int)std::round(s * sumCr);
        if (qCr < 0)
          qCr = 0;
        if (qCr > 255)
          qCr = 255;
        imageCopy.setPixelColor(u, v, YCbCrtoRGB(qY, qCb, qCr));
      }
    }

    imageLabel->setPixmap(QPixmap::fromImage(imageCopy));
  }
}

void ImageViewer::edgeMatrixFilter() {
  if (image != NULL) {
    QImage imageCopy = image->copy();
    QImage greyedImage = greyify(imageCopy);

    int X = table2->rowCount();
    int Y = table2->columnCount();
    int filterValues[X][Y];

    // Filter-Matrix aus UI auslesen und in filterValues einfügen und
    // gleichzeitig Summe berechnen
    int matrixSum = 0;
    for (int i = 0; i < X; ++i) {
      for (int j = 0; j < Y; ++j) {
        filterValues[i][j] = table2->item(i, j)->text().toInt();
        matrixSum += table2->item(i, j)->text().toInt();
      }
    }
    if (matrixSum == 0) {
      for (int i = 0; i < X; ++i) {
        for (int j = 0; j < Y; ++j) {
          matrixSum += abs(table2->item(i, j)->text().toInt());
        }
      }
    }

    double s = 1.0 / matrixSum;

    int K = X / 2;
    int L = Y / 2;

    for (int v = 0; v < greyedImage.width(); v++) {
      for (int u = 0; u < greyedImage.height(); u++) {
        int sumColor = 0;
        for (int j = -L; j <= L; j++) {
          for (int i = -K; i <= K; i++) {
            int pixelColor;

            if (u + i >= greyedImage.width()) {
              pixelColor = greyedImage.pixelColor(u - i, v).green();
            } else if (v + j >= greyedImage.height()) {
              pixelColor = greyedImage.pixelColor(u, v - j).green();
            } else if (u + i < 0) {
              pixelColor = greyedImage.pixelColor(u + std::abs(i), v).green();
            } else if (v + j < 0) {
              pixelColor = greyedImage.pixelColor(u, v + std::abs(j)).green();
            } else {
              pixelColor = greyedImage.pixelColor(u + i, v + j).green();
            }

            int c = filterValues[j + L][i + K];
            sumColor = sumColor + c * pixelColor;
          }
        }
        int newIntensity = (int)round(s * sumColor) + 127;
        newIntensity = std::max(0, std::min(255, newIntensity));
        image->setPixelColor(u, v,
                             QColor(newIntensity, newIntensity, newIntensity));
      }
    }

    updateImageDisplay();
  }
}

void ImageViewer::sobelFilter() {
  if (image != NULL) {
    QImage backupImage1 = image->copy();
    QImage backupImage2 = image->copy();
    QImage imageCopy = image->copy();
    QImage imageCopy2 = image->copy();
    QImage greyedImageSobelX = greyify(imageCopy);
    QImage greyedImageSobelY = greyify(imageCopy2);

    int sobelX[3][3] = {
        {-1, 0, 1},
        {-2, 0, 2},
        {-1, 0, 1},
    };

    int sobelY[3][3] = {
        {-1, -2, -1},
        {0, 0, 0},
        {1, 2, 1},
    };

    double s = 1.0 / 8;

    int K = 3 / 2;
    int L = 3 / 2;

    for (int v = 0; v < greyedImageSobelX.width(); v++) {
      for (int u = 0; u < greyedImageSobelX.height(); u++) {
        int sumColor = 0;
        for (int j = -L; j <= L; j++) {
          for (int i = -K; i <= K; i++) {
            int pixelColor;

            if (u + i >= greyedImageSobelX.width()) {
              pixelColor = greyedImageSobelX.pixelColor(u - i, v).green();
            } else if (v + j >= greyedImageSobelX.height()) {
              pixelColor = greyedImageSobelX.pixelColor(u, v - j).green();
            } else if (u + i < 0) {
              pixelColor =
                  greyedImageSobelX.pixelColor(u + std::abs(i), v).green();
            } else if (v + j < 0) {
              pixelColor =
                  greyedImageSobelX.pixelColor(u, v + std::abs(j)).green();
            } else {
              pixelColor = greyedImageSobelX.pixelColor(u + i, v + j).green();
            }

            int c = sobelX[j + L][i + K];
            sumColor = sumColor + c * pixelColor;
          }
        }
        int newIntensity = (int)round(s * sumColor) + 127;
        newIntensity = std::max(0, std::min(255, newIntensity));
        backupImage1.setPixelColor(
            u, v, QColor(newIntensity, newIntensity, newIntensity));
      }
    }

    for (int v = 0; v < greyedImageSobelY.width(); v++) {
      for (int u = 0; u < greyedImageSobelY.height(); u++) {
        int sumColor = 0;
        for (int j = -L; j <= L; j++) {
          for (int i = -K; i <= K; i++) {
            int pixelColor;

            if (u + i >= greyedImageSobelY.width()) {
              pixelColor = greyedImageSobelY.pixelColor(u - i, v).green();
            } else if (v + j >= greyedImageSobelY.height()) {
              pixelColor = greyedImageSobelY.pixelColor(u, v - j).green();
            } else if (u + i < 0) {
              pixelColor =
                  greyedImageSobelY.pixelColor(u + std::abs(i), v).green();
            } else if (v + j < 0) {
              pixelColor =
                  greyedImageSobelY.pixelColor(u, v + std::abs(j)).green();
            } else {
              pixelColor = greyedImageSobelY.pixelColor(u + i, v + j).green();
            }

            int c = sobelY[j + L][i + K];
            sumColor = sumColor + c * pixelColor;
          }
        }
        int newIntensity = (int)round(s * sumColor) + 127;
        newIntensity = std::max(0, std::min(255, newIntensity));
        backupImage2.setPixelColor(
            u, v, QColor(newIntensity, newIntensity, newIntensity));
      }
    }

    // Finales Sobelbild erstellen (entweder absolut Beträge addieren oder
    // quadrieren und Wurzel ziehen)
    for (int v = 0; v < image->width(); v++) {
      for (int u = 0; u < image->height(); u++) {
        int hsX = backupImage1.pixelColor(u, v).green();
        int hsY = backupImage2.pixelColor(u, v).green();
        int sum = abs(hsX) + abs(hsY);
        sum = std::max(0, std::min(255, sum));
        image->setPixelColor(u, v, QColor(sum, sum, sum));
      }
    }
    imageLabel->setPixmap(QPixmap::fromImage(*image));
  }
}

void ImageViewer::unsharpMaskingSharpener() {
  if (image != NULL) {
    QImage imageCopy = image->copy();
    QImage greyedOriginalImage = greyify(imageCopy);
    QImage gaussedImage = applygaussFilter2(sigmaUSMSpin->value());
    QImage greyedgaussedImage = greyify(gaussedImage);
    QImage finalImage = greyedOriginalImage.copy();

    QImage maskImage = greyedgaussedImage.copy();

    for (int v = 0; v < gaussedImage.width(); v++) {
      for (int u = 0; u < gaussedImage.height(); u++) {
        int newIntensity = greyedOriginalImage.pixelColor(u, v).green() -
                           greyedgaussedImage.pixelColor(u, v).green();
        newIntensity = std::max(0, std::min(255, (int)newIntensity));
        maskImage.setPixelColor(
            u, v, QColor(newIntensity, newIntensity, newIntensity));
      }
    }

    for (int v = 0; v < gaussedImage.width(); v++) {
      for (int u = 0; u < gaussedImage.height(); u++) {
        int factor = USMfactorSlider->value() / 30;
        int newIntensity = greyedOriginalImage.pixelColor(u, v).green() +
                           (factor * maskImage.pixelColor(u, v).green());
        newIntensity = std::max(0, std::min(255, (int)newIntensity));
        finalImage.setPixelColor(
            u, v, QColor(newIntensity, newIntensity, newIntensity));
      }
    }
    imageLabel->setPixmap(QPixmap::fromImage(finalImage));
  }
}
