PROGRAM = dsblogoutmgr

isEmpty(PREFIX) {
	PREFIX = /usr/local
}

isEmpty(DATADIR) {
	DATADIR = $${PREFIX}/share/$${PROGRAM}
}

QT	    += widgets
TEMPLATE     = app
TARGET	     = $${PROGRAM}
LANGUAGES    = de
PATH_LOCK    = .$${PROGRAM}.lock
INCLUDEPATH += . lib
DEFINES     += PROGRAM=\\\"$${PROGRAM}\\\" LOCALE_PATH=\\\"$${DATADIR}\\\"
DEFINES	    += PATH_LOCK=\\\"$${PATH_LOCK}\\\"
INSTALLS     = target locales
QMAKE_POST_LINK = $(STRIP) $(TARGET)
QMAKE_EXTRA_TARGETS += distclean cleanqm readme

HEADERS	    += lib/config.h \
	       src/bgwin.h \
	       src/mainwin.h \
	       src/timerwin.h \
	       src/countdown.h \
	       lib/dsbcfg/dsbcfg.h \
	       lib/qt-helper/qt-helper.h
SOURCES	    += lib/config.c \
	       src/bgwin.cpp \
	       src/main.cpp \
	       src/mainwin.cpp \
	       src/countdown.cpp \
	       src/timerwin.cpp \
	       lib/dsbcfg/dsbcfg.c \
	       lib/qt-helper/qt-helper.cpp

locales.path = $${DATADIR}

target.path  = $${PREFIX}/bin
target.files = $${PROGRAM}

readme.target = readme
readme.files = readme.mdoc
readme.commands = mandoc -mdoc readme.mdoc | perl -e \'foreach (<STDIN>) { \
		\$$_ =~ s/(.)\x08\1/\$$1/g; \$$_ =~ s/_\x08(.)/\$$1/g; \
		print \$$_ \
	}\' | sed '1,1d' > README

qtPrepareTool(LRELEASE, lrelease)
for(a, LANGUAGES) {
	in = locale/$${PROGRAM}_$${a}.ts
	out = locale/$${PROGRAM}_$${a}.qm
	locales.files += $$out
	cmd = $$LRELEASE $$in -qm $$out
	system($$cmd)
}

cleanqm.commands  = rm -f $${locales.files}
distclean.depends = cleanqm

