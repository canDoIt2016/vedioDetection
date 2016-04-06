TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    fhog.cpp \
    kcftracker.cpp \
    runtracker.cpp \
    boxextractor.cpp

INCLUDEPATH += D:/sdk/opencv3/opencv/buildm/include \
INCLUDEPATH += D:/sdk/opencv3/opencv/buildm/include/opencv
INCLUDEPATH += $$PWD



LIBS += -LD:/sdk/opencv3/opencv/buildm/x64/vc12/lib \
	-lopencv_bgsegm300d \
	-lopencv_bioinspired300d \
	-lopencv_calib3d300d \
	-lopencv_ccalib300d \
	-lopencv_core300d \
	-lopencv_datasets300d \
	-lopencv_face300d \
	-lopencv_features2d300d \
	-lopencv_flann300d \
	-lopencv_hal300d \
	-lopencv_highgui300d \
	-lopencv_imgcodecs300d \
	-lopencv_imgproc300d \
	-lopencv_latentsvm300d \
	-lopencv_line_descriptor300d \
	-lopencv_ml300d \
	-lopencv_objdetect300d \
	-lopencv_optflow300d \
	-lopencv_photo300d \
	-lopencv_reg300d \
	-lopencv_rgbd300d \
	-lopencv_saliency300d \
	-lopencv_shape300d \
	-lopencv_stereo300d \
	-lopencv_stitching300d \
	-lopencv_superres300d \
	-lopencv_surface_matching300d \
	-lopencv_text300d \
	-lopencv_tracking300d \
	-lopencv_ts300d \
	-lopencv_video300d \
	-lopencv_videoio300d \
	-lopencv_videostab300d \
	-lopencv_xfeatures2d300d \
	-lopencv_ximgproc300d \
	-lopencv_xobjdetect300d \
	-lopencv_xphoto300d
include(deployment.pri)
qtcAddDeployment()

HEADERS += \
    ffttools.hpp \
    fhog.hpp \
    kcftracker.hpp \
    labdata.hpp \
    recttools.hpp \
    tracker.h \
    dirent.h \
    boxextractor.h

