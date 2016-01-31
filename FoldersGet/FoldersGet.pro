QT       += core gui network widgets
CONFIG   += c++11
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FoldersGet
TEMPLATE = app


SOURCES += main.cpp\
				mtoken.cpp

HEADERS  += main.h \
		mtoken.h
