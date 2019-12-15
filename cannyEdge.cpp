#include <QtWidgets>
#ifndef QT_NO_PRINTER
#include <QPrintDialog>
#endif
#include <iostream>
#include <string>
#include <vector>
using namespace std;
#include "imageviewer-qt5.h"

QImage ImageViewer::applygaussFilter2(int sigma) {
  if (image != NULL) {
    QImage imageCopy = image->copy();
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

    return imageCopy;
  }
}

void ImageViewer::cannyEdge() {
  if (image != NULL) {
    QImage newImage = applygaussFilter2(sigmaCannySpin->value());
    QImage greyedGaussImage = greyify(newImage);
    QImage xGradient = greyedGaussImage.copy();
    QImage xGradientResult = greyedGaussImage.copy();
    QImage yGradient = greyedGaussImage.copy();
    QImage yGradientResult = greyedGaussImage.copy();
    QImage Emag = greyedGaussImage.copy();
    EmagGlobal = new QImage(greyedGaussImage.width(), greyedGaussImage.height(),
                            QImage::Format_RGB32);
    QImage Enms = greyedGaussImage.copy();
    QImage EbinMemory = greyedGaussImage.copy();
    QImage *Ebin = &EbinMemory;
    int tlo = tlowSpin->value();
    int thi = thighSpin->value();

    // X-Gradient
    double gradientMatrix[3] = {-0.5, 0, 0.5};
    int gradientMatrixCenter = 3 / 2;

    double matrixSum = 0;
    for (int i = 0; i < 3; i++) {
      matrixSum += abs(gradientMatrix[i]);
    }
    double s = 1.0 / matrixSum;

    // Iterate along X
    for (int v = 0; v < xGradient.width(); v++) {
      for (int u = 0; u < xGradient.height(); u++) {
        int sumColor = 0;
        for (int i = -gradientMatrixCenter; i <= gradientMatrixCenter; i++) {
          int pixelColor;
          if (u + i >= xGradient.width() || u + i < 0) {
            pixelColor = xGradient.pixelColor(u, v).green();
          } else {
            pixelColor = xGradient.pixelColor(u + i, v).green();
          }
          float c = gradientMatrix[i + gradientMatrixCenter];
          sumColor = sumColor + c * pixelColor;
        }
        int newIntensity = (int)round(s * sumColor) + 127;
        newIntensity = std::max(0, std::min(255, newIntensity));
        xGradientResult.setPixelColor(
            u, v, QColor(newIntensity, newIntensity, newIntensity));
      }
    }

    // Iterate along Y
    for (int u = 0; u < yGradient.height(); u++) {
      for (int v = 0; v < yGradient.width(); v++) {
        int sumColor = 0;
        for (int i = -gradientMatrixCenter; i <= gradientMatrixCenter; i++) {
          int pixelColor;
          if (v + i >= yGradient.height() || v + i < 0) {
            pixelColor = yGradient.pixelColor(u, v).green();
          } else {
            pixelColor = yGradient.pixelColor(u, v + i).green();
          }
          float c = gradientMatrix[i + gradientMatrixCenter];
          sumColor = sumColor + c * pixelColor;
        }
        int newIntensity = (int)round(s * sumColor) + 127;
        newIntensity = std::max(0, std::min(255, newIntensity));
        yGradientResult.setPixelColor(
            u, v, QColor(newIntensity, newIntensity, newIntensity));
      }
    }

    for (int u = 0; u < greyedGaussImage.height(); u++) {
      for (int v = 0; v < greyedGaussImage.width(); v++) {
        int xGradientQuad = pow(xGradientResult.pixelColor(u, v).green(), 2);
        int yGradientQuad = pow(yGradientResult.pixelColor(u, v).green(), 2);
        int sum = sqrt(xGradientQuad + yGradientQuad);
        sum = std::max(0, std::min(255, sum));
        Emag.setPixelColor(u, v, QColor(sum, sum, sum));

        // Emag Global Kram
        int xGradientQuad2 =
            pow(xGradientResult.pixelColor(u, v).green() - 127, 2);
        int yGradientQuad2 =
            pow(yGradientResult.pixelColor(u, v).green() - 127, 2);
        int sum2 = sqrt(xGradientQuad2 + yGradientQuad2);
        sum2 = std::max(0, std::min(255, sum2));
        EmagGlobal->setPixelColor(u, v, QColor(sum2, sum2, sum2));

        Enms.setPixelColor(u, v, QColor(0, 0, 0));
        Ebin->setPixelColor(u, v, QColor(0, 0, 0));
      }
    }

    for (int u = 1; u < greyedGaussImage.height() - 2; u++) {
      for (int v = 1; v < greyedGaussImage.width() - 2; v++) {
        int dx = xGradientResult.pixelColor(u, v).green();
        int dy = yGradientResult.pixelColor(u, v).green();
        int s0 = getOrientationSector(dx, dy);
        if (isLocalMax(Emag, u, v, s0, tlo)) {
          int newVal = Emag.pixelColor(u, v).green();
          Enms.setPixelColor(u, v, QColor(newVal, newVal, newVal));
        }
      }
    }
    QImage final;
    for (int u = 1; u < greyedGaussImage.height() - 2; u++) {
      for (int v = 1; v < greyedGaussImage.width() - 2; v++) {
        if (Enms.pixelColor(u, v).green() >= thi &&
            Ebin->pixelColor(u, v).green() == 0) {
          traceAndThreshold(Enms, Ebin, u, v, tlo);
        }
      }
    }
    imageLabel->setPixmap(QPixmap::fromImage(*Ebin));
  }
}

void ImageViewer::traceAndThreshold(QImage Enms, QImage *Ebin, int u0, int v0,
                                    int tlo) {
  Ebin->setPixelColor(u0, v0, QColor(255, 255, 255));
  int uL = max(u0 - 1, 0);
  int uR = min(u0 + 1, 20 - 1);
  int vT = max(v0 - 1, 0);
  int vB = min(v0 + 1, 20 - 1);
  for (int u = uL; u < uR; u++) {
    for (int v = vT; v < vB; v++) {
      if (Enms.pixelColor(u, v).green() >= tlo &&
          Ebin->pixelColor(u, v).green() == 0) {
        traceAndThreshold(Enms, Ebin, u, v, tlo);
      }
    }
  }
  return;
}

int ImageViewer::getOrientationSector(int dx, int dy) {
  int s0;
  double piDiv8Val = M_PI / 8;
  int Dnewx = cos(piDiv8Val) * dx - sin(piDiv8Val) * dy;
  int Dnewy = sin(piDiv8Val) * dx + cos(piDiv8Val) * dy;
  if (Dnewy < 0) {
    Dnewx = -Dnewx;
    Dnewy = -Dnewy;
  }
  if (Dnewx >= 0 && Dnewx >= Dnewy) {
    s0 = 0;
  } else if (Dnewx >= 0 && Dnewx < Dnewy) {
    s0 = 1;
  } else if (Dnewx < 0 && -Dnewx < Dnewy) {
    s0 = 2;
  } else if (Dnewx < 0 && -Dnewx >= Dnewy) {
    s0 = 3;
  }
  return s0;
}

bool ImageViewer::isLocalMax(QImage Emag, int u, int v, int s0, int tlo) {
  int mC = Emag.pixelColor(u, v).green();
  int mL, mR = 0;
  if (mC < tlo) {
    return false;
  } else {
    if (s0 == 0) {
      mL = Emag.pixelColor(u - 1, v).green();
      mR = Emag.pixelColor(u + 1, v).green();
    } else if (s0 == 1) {
      mL = Emag.pixelColor(u - 1, v - 1).green();
      mR = Emag.pixelColor(u + 1, v + 1).green();
    } else if (s0 == 2) {
      mL = Emag.pixelColor(u, v - 1).green();
      mR = Emag.pixelColor(u, v + 1).green();
    } else if (s0 == 3) {
      mL = Emag.pixelColor(u - 1, v + 1).green();
      mR = Emag.pixelColor(u + 1, v - 1).green();
    }
  }
  return (mL <= mC && mC >= mR);
}
