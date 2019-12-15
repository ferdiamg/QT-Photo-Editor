#include <QtWidgets>
#ifndef QT_NO_PRINTER
#include <QPrintDialog>
#endif
#include <iostream>
#include <string>
#include <vector>
using namespace std;
#include "akkumulator.h"
#include "imageviewer-qt5.h"
#include "line.h"

void ImageViewer::houghTransform() {
  if (image != NULL && EmagGlobal != NULL) {
    const QPixmap *currentCanny = imageLabel->pixmap();
    QImage currentCannyImage = currentCanny->toImage();

    int Xr = currentCannyImage.width() / 2;
    int Yr = currentCannyImage.height() / 2;
    float m = mHough->value();
    float n = nHough->value();
    int Amin = AminSpinner->value();
    float Dtheta = M_PI / m;
    float Mquad = currentCannyImage.width() * currentCannyImage.width();
    float Nquad = currentCannyImage.height() * currentCannyImage.height();
    float Dr = sqrt(Mquad + Nquad) / n;
    float j0 = n / 2;

    vector<vector<Akkumulator>> accumulatorVector(n, vector<Akkumulator>(m));
    for (int i = 0; i < n; i++) {
      for (int j = 0; j < m; j++) {
        accumulatorVector[i][j].a = 0;
      }
    }

    // Accumulate counts in Accumulator-Map
    for (int v = 0; v < currentCannyImage.width(); v++) {
      for (int u = 0; u < currentCannyImage.height(); u++) {
        if (currentCannyImage.pixelColor(u, v).green() > 0) {
          int x = u - Xr;
          int y = v - Yr;
          for (int i = 0; i < m - 1; i++) {
            float Theta = Dtheta * i;
            float r = x * cos(Theta) + y * sin(Theta);
            int j = j0 + round(r / Dr);
            int curVal = accumulatorVector[i][j].a;
            accumulatorVector[i][j].a = accumulatorVector[i][j].a +
                                        EmagGlobal->pixelColor(u, v).green();

            if (accumulatorVector[i][j].Vmin < v) {
              accumulatorVector[i][j].Vmin = v;
            }
            if (accumulatorVector[i][j].Umin < u) {
              accumulatorVector[i][j].Umin = u;
            }
            if (accumulatorVector[i][j].Umax > u) {
              accumulatorVector[i][j].Umax = u;
            }
            if (accumulatorVector[i][j].Vmax > v) {
              accumulatorVector[i][j].Vmax = v;
            }
          }
        }
      }
    }

    QImage *AmaxWhiteImage = new QImage(m, n, QImage::Format_RGB32);
    for (int v = 0; v < m; v++) {
      for (int u = 0; u < n; u++) {
        AmaxWhiteImage->setPixelColor(u, v, QColor(255, 255, 255));
      }
    }
    vector<vector<int>> Amax(n, vector<int>(m));
    Amax = getCurrentAmax(*AmaxWhiteImage);

    // Bias Kompensation
    for (int i = 0; i < n; i++) {
      for (int j = 0; j < m; j++) {
        accumulatorVector[i][j].a =
            (accumulatorVector[i][j].a / std::max(1, Amax[i][j])) * 10;
      }
    }

    // Create HNF lines from LocalMax points in Map
    // Use combination of Schwellwertoperation aswell as NMS to get relevant
    // lines
    vector<Line> L;
    for (int v = 0; v < m - 1; v++) {
      for (int u = 0; u < n - 1; u++) {
        if (accumulatorVector[u][v].a >= Amin &&
            isLocalMax2(accumulatorVector, u, v)) {
          float Theta = u * Dtheta;
          float r = (v - j0) * Dr;
          int a = accumulatorVector[u][v].a;
          Line *newLine = new Line(Theta, r, a, accumulatorVector[u][v].Umin,
                                   accumulatorVector[u][v].Vmin,
                                   accumulatorVector[u][v].Umax,
                                   accumulatorVector[u][v].Vmax);
          L.push_back(*newLine);
        }
      }
    }

    // Draw all lines with thickness of omega
    float omega = omegaHough->value();
    for (vector<float>::size_type i = 0; i != L.size(); i++) {
      logFile << "Line: ---" << i << "---Umin: " << L.at(i).Umin
              << "Vmin: " << L.at(i).Vmin << "Umax: " << L.at(i).Umax
              << "Vmax: " << L.at(i).Vmax << endl;
      for (int v = 0; v < image->width(); v++) {
        for (int u = 0; u < image->height(); u++) {
          if ((abs((u - Xr) * cos(L.at(i).theta) +
                   (v - Yr) * sin(L.at(i).theta) - L.at(i).r) <= omega)) {
            if (u < L.at(i).Umin) {
              image->setPixelColor(u, v, QColor(255, 0, 0));
            }
            if (v >= L.at(i).Vmin) {
              image->setPixelColor(u, v, QColor(255, 0, 0));
            }
            if (u < L.at(i).Umax) {
              image->setPixelColor(u, v, QColor(255, 0, 0));
            }
            if (v > L.at(i).Vmax) {
              image->setPixelColor(u, v, QColor(255, 0, 0));
            }
          }
        }
      }
    }

    //    // Draw QT lines within bounding box limits
    //    QPainter p(image);
    //    //    p.setRenderHint(QPainter::Antialiasing);
    //    p.setPen(QPen(Qt::green, 2));
    //    for (vector<float>::size_type i = 0; i != L.size(); i++) {
    //      logFile << "Line: ---" << i << "---Umin: " << L.at(i).Umin
    //              << "Vmin: " << L.at(i).Vmin << "Umax: " << L.at(i).Umax
    //              << "Vmax: " << L.at(i).Vmax << endl;
    //      p.drawLine(L.at(i).Umin, L.at(i).Vmin, L.at(i).Umax, L.at(i).Vmax);
    //    }

    renewLogging();
    imageLabel->setPixmap(QPixmap::fromImage(*image));
  }
}

bool ImageViewer::isLocalMax2(vector<vector<Akkumulator>> &accumulatorVector,
                              int u, int v) {
  int q0 = accumulatorVector[u][v].a;
  vector<int> neighbours(8);

  for (int j = u - 1; j <= u + 1; j++) {
    for (int i = v - 1; i <= v + 1; i++) {

      if (u + i >= accumulatorVector[1].size()) {
        neighbours.push_back(0);
      } else if (v + j >= accumulatorVector.size()) {
        neighbours.push_back(0);
      } else if (u + i < 0) {
        neighbours.push_back(0);
      } else if (v + j < 0) {
        neighbours.push_back(0);
      } else if (u + j < 0) {
        neighbours.push_back(0);
      } else {
        neighbours.push_back(accumulatorVector[j][i].a);
      }
    }
  }

  int max = *max_element(neighbours.begin(), neighbours.end());
  if (q0 >= max) {
    return true;
  } else {
    return false;
  }
}

vector<vector<int>> ImageViewer::getCurrentAmax(QImage AmaxWhiteImage) {
  QImage whiteImage = AmaxWhiteImage;

  int Xr = whiteImage.width() / 2;
  int Yr = whiteImage.height() / 2;
  float m = mHough->value();
  float n = nHough->value();
  int Amin = AminSpinner->value();
  float Dtheta = M_PI / m;
  float Mquad = whiteImage.width() * whiteImage.width();
  float Nquad = whiteImage.height() * whiteImage.height();
  float Dr = sqrt(Mquad + Nquad) / n;
  float j0 = n / 2;

  vector<vector<int>> Amax(n, vector<int>(m, 0));

  // Accumulate counts in Accumulator-Map
  for (int v = 0; v < whiteImage.width(); v++) {
    for (int u = 0; u < whiteImage.height(); u++) {
      if (whiteImage.pixelColor(u, v).green() > 0) {
        int x = u - Xr;
        int y = v - Yr;
        for (int i = 0; i < m - 1; i++) {
          float Theta = Dtheta * i;
          float r = x * cos(Theta) + y * sin(Theta);
          int j = j0 + round(r / Dr);
          int curVal = Amax[i][j];
          Amax[i][j] = Amax[i][j] + 1;
        }
      }
    }
  }
  return Amax;
}
