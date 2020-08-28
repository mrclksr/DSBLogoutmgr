/*-
 * Copyright (c) 2017 Marcel Kaiser. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <unistd.h>
#include <errno.h>
#include <pwd.h>
#include <err.h>
#include <QApplication>
#include <QLocale>
#include <QTranslator>

#include "bgwin.h"
#include "mainwin.h"
#include "timerwin.h"
#include "countdown.h"
#include "qt-helper/qt-helper.h"
#include "config.h"

#define PATH_FIFO "." PROGRAM ".fifo"

static void usage(void);
static void execmd(const char *cmd);
static void cleanup(void);

static char path_fifo[PATH_MAX];

int
main(int argc, char *argv[])
{
	int	      bt, ch, fifo;
	dsbcfg_t      *cfg;
	const char    *cmds[6];
	struct passwd *pw;

	QApplication app(argc, argv);
	QTranslator translator;

	if (translator.load(QLocale(), QLatin1String(PROGRAM),
	    QLatin1String("_"), QLatin1String(LOCALE_PATH)))
		app.installTranslator(&translator);

	/* Set application name and RESOURCE_NAME env to set WM_CLASS */
	QApplication::setApplicationName(PROGRAM);
	(void)qputenv("RESOURCE_NAME", PROGRAM);

	cfg = dsbcfg_read(PROGRAM, "config", vardefs, CFG_NVARS);
	if (cfg == NULL && errno == ENOENT) {
		cfg = dsbcfg_new(NULL, vardefs, CFG_NVARS);
		if (cfg == NULL)
			qh_errx(0, EXIT_FAILURE, "%s", dsbcfg_strerror());
	} else if (cfg == NULL)
		qh_errx(0, EXIT_FAILURE, "%s", dsbcfg_strerror());

	cmds[LOCK]     = dsbcfg_getval(cfg, CFG_LOCK).string;
	cmds[LOGOUT]   = dsbcfg_getval(cfg, CFG_LOGOUT).string;
	cmds[REBOOT]   = dsbcfg_getval(cfg, CFG_REBOOT).string;
	cmds[SUSPEND]  = dsbcfg_getval(cfg, CFG_SUSPEND).string;
	cmds[SHUTDOWN] = dsbcfg_getval(cfg, CFG_SHUTDOWN).string;

	while ((ch = getopt(argc, argv, "l:r:s:S:L:")) != -1) {
		switch (ch) {
		case 'L':
			cmds[LOCK] = optarg;
			break;
		case 'l':
			cmds[LOGOUT] = optarg;
			break;
		case 'r':
			cmds[REBOOT] = optarg;
			break;
		case 's':
			cmds[SHUTDOWN] = optarg;
			break;
		case 'S':
			cmds[SUSPEND] = optarg;
			break;
		case '?':
		default :
			usage();
		}
	}
	if ((pw = getpwuid(getuid())) == NULL)
		qh_err(0, EXIT_FAILURE, "getpwuid()");
	endpwent();
		(void)snprintf(path_fifo, sizeof(path_fifo), "%s/%s", pw->pw_dir,
	    PATH_FIFO);
	endpwent();
	if (mkfifo(path_fifo, S_IWUSR | S_IRUSR) == -1 && errno != EEXIST)
		err(EXIT_FAILURE, "mkfifo()");
	(void)chmod(path_fifo, S_IRUSR | S_IWUSR);
	if ((fifo = open(path_fifo, O_WRONLY | O_NONBLOCK)) != -1) {
		(void)write(fifo, "x", 1);
		exit(EXIT_SUCCESS);
	} else {
		/*
		 * Open the FIFO for reading and writing to prevent select()
		 * from returning with a value > 0 if the last writer has
		 * closed its end.
		 */
		(void)close(fifo);
		if ((fifo = open(path_fifo, O_RDWR | O_NONBLOCK)) == -1)
			err(1, "open(%s)", path_fifo);
	}
	(void)atexit(cleanup);

	BgWin   *bg = new BgWin();
	Mainwin *w  = new Mainwin(bg);

	while (w->exec() == QDialog::Accepted) {
		bt = w->getButton();
		if (bt != TIMER)
			delete(bg);
		switch (bt) {
		case LOCK:
			execmd(cmds[LOCK]);
			break;
		case LOGOUT:
			execmd(cmds[LOGOUT]);
			break;
		case SHUTDOWN:
			execmd(cmds[SHUTDOWN]);
			break;
		case SUSPEND:
			execmd(cmds[SUSPEND]);
			break;
		case REBOOT:
			execmd(cmds[REBOOT]);
			break;
		case TIMER:
			Timerwin *tw = new Timerwin(bg);
			if (tw->exec() == QDialog::Accepted) {
				int hours   = tw->getHours();
				int minutes = tw->getMinutes();
				delete(bg);
				Countdown *c = new Countdown(hours, minutes,
							     fifo);
				app.exec();
				if (c->shutdown())
					execmd(cmds[SHUTDOWN]);
				return (EXIT_SUCCESS);
			} else
				delete(tw);
			break;
		}
	}
	return (EXIT_SUCCESS);
}

static void
execmd(const char *cmd)
{
	int error;

	if ((error = system(cmd)) == -1) {
		qh_err(0, EXIT_FAILURE, "system(%s)", cmd);
	} else if (error == 0) {
		exit(EXIT_SUCCESS);
	} else if ((error >> 8) == 127) {
		qh_errx(0, EXIT_FAILURE,
		    QObject::tr("Command '%s' not found").toLocal8Bit().data(),
		    cmd);
	}
	qh_errx(0, EXIT_FAILURE,
	    QObject::tr("Command '%s' exited with code %d").toLocal8Bit().data(),
	    cmd, (error >> 8));
}

static void
cleanup()
{
	(void)unlink(path_fifo);
}

static void
usage()
{
	(void)fprintf(stderr,
	    "Usage: %s [-L <lock command>] [-S <suspend command>]\n" \
	    "       %-*s [-l <logout command>] [-r <reboot command>]\n" \
	    "       %-*s [-s <shutdown command>]\n", PROGRAM,
	    (int)sizeof(PROGRAM) - 1, "", (int)sizeof(PROGRAM) - 1, "");
	exit(EXIT_FAILURE);
}

