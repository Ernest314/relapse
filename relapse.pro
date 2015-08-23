#--------------------------------------------------#
# Project created by QtCreator 2015-08-20T02:56:07 #
#--------------------------------------------------#

CONFIG	+= c++11 c++14

QT		+= core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET	= relapse
TEMPLATE = app

SOURCES += main.cpp \
	ui/mainwindow.cpp

HEADERS += \
	ui/mainwindow.h

FORMS	+= \
	ui/mainwindow.ui

RESOURCES += \
	metadata.qrc \
	icons.qrc



# Pre-/Post- build packaging and stuff

!exists( version.txt ) {
	error( "No version.txt file found." )
}

QMAKE_POST_LINK += $$PWD/build-num-updater.exe -f $$PWD/version.txt



# Old version incrementer that ran even if nothing was changed

#versionTarget.target = $$PWD/version.txt
#versionTarget.depends = FORCE
#versionTarget.commands = $$PWD/build-num-updater.exe -f $$PWD/version.txt

#PRE_TARGETDEPS += $$PWD/version.txt
#QMAKE_EXTRA_TARGETS += versionTarget
