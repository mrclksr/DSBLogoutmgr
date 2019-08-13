/*-
 * Copyright (c) 2016 Marcel Kaiser. All rights reserved.
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

#include <QTimer>
#include <QStyle>
#include <QScreen>
#include <QRect>
#include <QDesktopWidget>

#include "icons.h"
#include "countdown.h"
#include "timerwin.h"
#include "qt-helper/qt-helper.h"

Countdown::Countdown(int hours, int minutes, QWidget *parent)
	: QDialog(parent)
{
	int secondsLeft = hours * 60 * 60 + minutes * 60;

	trayIcon     = 0;
	doShutdown   = false;
	shutdownTime = time(NULL) + secondsLeft;

	QIcon cnclIcon = qh_loadStockIcon(QStyle::SP_DialogCancelButton, 0);
	QIcon okIcon   = qh_loadStockIcon(QStyle::SP_DialogOkButton, 0);
	QIcon chgIcon  = qh_loadIcon(ICONS_TIMER);
	QIcon pic      = qh_loadIcon(ICONS_TIMER);
	tIcon          = pic;

	label		    = new QLabel("");
	timer		    = new QTimer(this);
	trayTimer	    = new QTimer(this);
	QLabel	    *icon   = new QLabel;
	QHBoxLayout *hbox   = new QHBoxLayout;
	QHBoxLayout *btHbox = new QHBoxLayout;
	QVBoxLayout *layout = new QVBoxLayout(this);
	QPushButton *cancel = new QPushButton(cnclIcon, tr("Cancel timer"));
	QPushButton *ok	    = new QPushButton(okIcon, tr("Ok"));
	QPushButton *edit   = new QPushButton(chgIcon, tr("Change timer"));

	setWindowIcon(tIcon);

	hbox->setSpacing(20);
	hbox->addWidget(icon);
	hbox->addWidget(label, 1, Qt::AlignHCenter);

	btHbox->setSpacing(2);
	btHbox->addWidget(edit, 0, Qt::AlignLeft);
	btHbox->addWidget(cancel, 0, Qt::AlignRight);
	btHbox->addWidget(ok, 1, Qt::AlignRight);

	layout->setSpacing(10);
	layout->addLayout(hbox);
	layout->addLayout(btHbox);
	layout->setContentsMargins(15, 15, 15, 15);

	connect(trayTimer, SIGNAL(timeout()), this, SLOT(checkForSysTray()));
	connect(timer, SIGNAL(timeout()), this, SLOT(update()));
	connect(ok, SIGNAL(clicked()), this, SLOT(hideWin()));
	connect(cancel, SIGNAL(clicked()), this, SLOT(reject()));
	connect(edit, SIGNAL(clicked()), this, SLOT(changeTimer()));
	if (secondsLeft <= 60)
		timer->start(1000);
	else
		timer->start(60000);
	trayTimer->start(500);

	connect(QGuiApplication::primaryScreen(),
	    SIGNAL(geometryChanged(const QRect &)), this,
	    SLOT(scrGeomChanged(const QRect &)));
}

void
Countdown::scrGeomChanged(const QRect &g)
{
	Q_UNUSED(g);
	trayTimer->start(500);
}

void Countdown::setLabelText(int hours, int minutes, int seconds)
{
	QString s;

	if (seconds > 0) {
		s = tr("Shutdown in %1 %2");
		s = s.arg(seconds);
		s = s.arg(seconds == 1 ? tr("Second") : tr("Seconds"));
	} else  if (hours > 0) {
		if (minutes > 0) {
			s = tr("Shutdown in %1 %2 %3 %4");
			s = s.arg(hours);
			s = s.arg(hours == 1 ? tr("Hour") : tr("Hours"));
			s = s.arg(minutes);
			s = s.arg(minutes == 1 ? tr("Minute") : tr("Minutes"));
		} else {
			s = tr("Shutdown in %1 %2");
			s = s.arg(hours);
			s = s.arg(hours == 1 ? tr("Hour") : tr("Hours"));
		}
	} else {
		s = tr("Shutdown in %1 %2");
		s = s.arg(minutes);
		s = s.arg(minutes == 1 ? tr("Minute") : tr("Minutes"));
	}
	label->setText(s);
}


void Countdown::keyPressEvent(QKeyEvent *e) {
	if (e->key() == Qt::Key_Escape)
		return;
	QDialog::keyPressEvent(e);
}

void Countdown::hideWin()
{
	setVisible(false);
}

void Countdown::closeEvent(QCloseEvent *event)
{
	setVisible(false);
	event->ignore();
}

void Countdown::checkForSysTray()
{
	static int tries = 60;

	if (trayIcon != 0) {
		tries = 60;
		delete trayIcon;
		trayIcon = 0;
	}
	if (QSystemTrayIcon::isSystemTrayAvailable()) {
		trayTimer->stop();
		createTrayIcon();
	} else if (tries-- <= 0) {
		trayTimer->stop();
		show();
	}
}

void Countdown::createTrayIcon()
{
	if (trayIcon != 0)
		return;
	if (!QSystemTrayIcon::isSystemTrayAvailable())
		return;
	trayIcon = new QSystemTrayIcon(this);
	trayIcon->setIcon(tIcon);
	update();
	trayIcon->show();
	connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
	    this, SLOT(trayClicked(QSystemTrayIcon::ActivationReason)));
}

void Countdown::trayClicked(QSystemTrayIcon::ActivationReason reason)
{
	if (reason == QSystemTrayIcon::Trigger || 
	    reason == QSystemTrayIcon::DoubleClick) {
		if (!isVisible()) {
			show();
			setGeometry(QStyle::alignedRect(Qt::LeftToRight,
			    Qt::AlignCenter, size(),
			    qApp->desktop()->availableGeometry()));
			setVisible(true);
		} else
			setVisible(false);
	}
}

bool Countdown::shutdown()
{
	return (doShutdown);
}

void Countdown::update()
{
	int secondsLeft = (int)(shutdownTime - time(NULL));

	if (secondsLeft <= 60 && secondsLeft > 0) {
		timer->start(1000);
		setLabelText(0, 0, secondsLeft);
		if (trayIcon != 0)
			trayIcon->setToolTip(label->text());
	} else if (secondsLeft > 0) {
		int hours = secondsLeft / (60 * 60);
		int minutes = (secondsLeft - hours * 60 * 60) / 60;
		setLabelText(hours, minutes);
		if (trayIcon != 0)
			trayIcon->setToolTip(label->text());
	}
	if (secondsLeft <= 0) {
		doShutdown = true;
		accept();
	}
}

void Countdown::changeTimer()
{
	int secondsLeft = (int)(shutdownTime - time(NULL));
	int hours   =  secondsLeft / (60 * 60);
	int minutes = (secondsLeft -  hours * 60 * 60) / 60;
	Timerwin tw;

	tw.setHours(hours);
	tw.setMinutes(minutes);

	if (tw.exec() == QDialog::Accepted) {
		shutdownTime = time(NULL) + tw.getHours() * 60 * 60 +
			       tw.getMinutes() * 60;
		update();
	}
	setVisible(false);
}

