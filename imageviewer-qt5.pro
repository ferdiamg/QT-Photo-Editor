QT += widgets
qtHaveModule(printsupport): QT += printsupport

HEADERS       = imageviewer-qt5.h \
  akkumulator.h \
  line.h
SOURCES       = imageviewer-qt5.cpp \
                akkumulator.cpp \
                cannyEdge.cpp \
                generateControlPanels.cpp \
                hough.cpp \
                imageviewer-main-qt5.cpp \
                line.cpp \
                myAlgorithms.cpp

# install
target.path = $$[QT_INSTALL_EXAMPLES]/widgets/widgets/imageviewer
INSTALLS += target

wince*: {
   DEPLOYMENT_PLUGIN += qjpeg qgif
}
