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
#include <QApplication>
#include <QLocale>
#include <QTranslator>

#include "bgwin.h"
#include "mainwin.h"
#include "timerwin.h"
#include "countdown.h"
#include "qt-helper/qt-helper.h"
#include "config.h"
#include "dsbcfg/dsbcfg.h"

static void usage(void);
static void execmd(const char *cmd, QWidget *bgwin);

int
main(int argc, char *argv[])
{
	int	      ch;
	char	      path_lock[PATH_MAX];
	FILE	      *fp;
	dsbcfg_t      *cfg;
	const char    *cmds[5];
	struct passwd *pw;

	QApplication app(argc, argv);
	QTranslator translator;

	if (translator.load(QLocale(), QLatin1String(PROGRAM),
	    QLatin1String("_"), QLatin1String(LOCALE_PATH)))
		app.installTranslator(&translator);

	cfg = dsbcfg_read(PROGRAM, "config", vardefs, CFG_NVARS);
	if (cfg == NULL && errno == ENOENT) {
		cfg = dsbcfg_new(NULL, vardefs, CFG_NVARS);
		if (cfg == NULL)
			qh_errx(0, EXIT_FAILURE, "%s", dsbcfg_strerror());
	} else if (cfg == NULL)
		qh_errx(0, EXIT_FAILURE, "%s", dsbcfg_strerror());

	if ((pw = getpwuid(getuid())) == NULL)
		qh_err(0, EXIT_FAILURE, "getpwuid()");
	/* Check if another instance is already running. */
	(void)snprintf(path_lock, sizeof(path_lock), "%s/%s", pw->pw_dir,
	    PATH_LOCK);
	endpwent();
	if ((fp = fopen(path_lock, "r+")) == NULL) {
		if (errno != ENOENT || (fp = fopen(path_lock, "w+")) == NULL)
			qh_err(0, EXIT_FAILURE, "fopen(%s)", path_lock);
	}
	if (lockf(fileno(fp), F_TLOCK, 0) == -1) {
		if (errno == EWOULDBLOCK)
			exit(EXIT_SUCCESS);
                qh_err(0, EXIT_FAILURE, "lockf()");
	}
	cmds[LOGOUT]   = dsbcfg_getval(cfg, CFG_LOGOUT).string;
	cmds[REBOOT]   = dsbcfg_getval(cfg, CFG_REBOOT).string;
	cmds[SUSPEND]  = dsbcfg_getval(cfg, CFG_SUSPEND).string;
	cmds[SHUTDOWN] = dsbcfg_getval(cfg, CFG_SHUTDOWN).string;

	while ((ch = getopt(argc, argv, "l:r:s:S:")) != -1) {
		switch (ch) {
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
	BgWin   *bg = new BgWin();
	Mainwin *w  = new Mainwin(bg);

	while (w->exec() == QDialog::Accepted) {
		switch (w->getButton()) {
		case LOGOUT:
			execmd(cmds[LOGOUT], bg);
			break;
		case SHUTDOWN:
			execmd(cmds[SHUTDOWN], bg);
			break;
		case SUSPEND:
			execmd(cmds[SUSPEND], bg);
			break;
		case REBOOT:
			execmd(cmds[REBOOT], bg);
			break;
		case TIMER:
			Timerwin *tw = new Timerwin(bg);
			if (tw->exec() == QDialog::Accepted) {
				bg->close();
				Countdown *c = new Countdown(tw->getHours(),
				    tw->getMinutes());
				app.exec();
				if (c->shutdown())
					execmd(cmds[SHUTDOWN], 0);
				return (EXIT_SUCCESS);
			}
			delete(tw);
			break;
		}
	}
	return (EXIT_SUCCESS);
}

static void
execmd(const char *cmd, QWidget *bgwin)
{
	switch (system(cmd)) {
	case -1:
		qh_err(bgwin, EXIT_FAILURE, "system(%s)", cmd);
	case  0:
		exit(EXIT_SUCCESS);
	default:
		qh_errx(bgwin, EXIT_FAILURE, "Couldn't execute \"%s\"", cmd);
	}
}

static void
usage()
{
	(void)fprintf(stderr,
	    "Usage: %s [-l <logout command>] [-r <reboot command>]\n" \
	    "       %-*s [-s <shutdown command>] [-S <suspend command>]\n",
	    PROGRAM, (int)sizeof(PROGRAM) - 1, "");
	exit(EXIT_FAILURE);
}

