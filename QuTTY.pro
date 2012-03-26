#-------------------------------------------------
#
# Project created by QtCreator 2012-01-02T17:45:25
#
#-------------------------------------------------

QT       += core gui network

TARGET = QuTTY
TEMPLATE = app


SOURCES +=  \
    GuiMain.cpp \
    GuiMainWindow.cpp \
    GuiTerminalWindow.cpp \
    GuiSettingsWindow.cpp \
    terminalmisc.cpp \
    QtCommon.cpp \
    QtNet.cpp \
    QtUnicode.cpp \
    QtPageant.cpp \
    QtNoise.cpp \
    QtDlg.cpp \
    QtGss.cpp \
    QtX11.cpp \
    puttysrc/PGSSAPI.C \
    puttysrc/INT64.C \
    puttysrc/ssh.c \
    puttysrc/proxy.c \
    puttysrc/portfwd.c \
    puttysrc/misc.c \
    puttysrc/terminal.c \
    puttysrc/ldisc.c \
    puttysrc/ldiscucs.c \
    puttysrc/timing.c \
    puttysrc/sshbn.c \
    puttysrc/sshpubk.c \
    puttysrc/sshrand.c \
    puttysrc/sshrsa.c \
    puttysrc/sshsh256.c \
    puttysrc/sshsh512.c \
    puttysrc/sshsha.c \
    puttysrc/x11fwd.c \
    puttysrc/tree234.c \
    puttysrc/telnet.c \
    puttysrc/pinger.c \
    puttysrc/sshmd5.c \
    puttysrc/sshaes.c \
    puttysrc/sshdes.c \
    puttysrc/sshzlib.c \
    puttysrc/sshdh.c \
    puttysrc/sshblowf.c \
    puttysrc/ssharcf.c \
    puttysrc/sshcrcda.c \
    puttysrc/sshdss.c \
    puttysrc/cproxy.c \
    puttysrc/logging.c \
    puttysrc/minibidi.c \
    puttysrc/sshgssc.c \
    puttysrc/wcwidth.c \
    puttysrc/sshcrc.c \
    puttysrc/wildcard.c \
    puttysrc/BE_ALL_S.C

HEADERS +=  \
    GuiMainWindow.h \
    GuiTerminalWindow.h \
    GuiSettingsWindow.h \
    QtCommon.h \
    QtLogDbg.h \
    puttysrc/TREE234.H \
    puttysrc/TERMINAL.H \
    puttysrc/SSHGSSC.H \
    puttysrc/SSHGSS.H \
    puttysrc/SSH.H \
    puttysrc/PUTTYPS.H \
    puttysrc/PUTTYMEM.H \
    puttysrc/PUTTY.H \
    puttysrc/PROXY.H \
    puttysrc/PGSSAPI.H \
    puttysrc/NETWORK.H \
    puttysrc/missing.h \
    puttysrc/LDISC.H \
    puttysrc/INT64.H \
    puttysrc/QtStuff.h \
    puttysrc/MISC.H

INCLUDEPATH += tmp/moc/release_shared puttysrc/

#LIBS += -lGdi32 -L C:\\Qt\\4.8.0\\lib

#CONFIG += static