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
#include <QDesktopWidget>

#include "icons.h"
#include "countdown.h"
#include "qt-helper/qt-helper.h"

Countdown::Countdown(int hours, int minutes, QWidget *parent)
	: QDialog(parent)
{
	int secondsLeft = hours * 60 * 60 + minutes * 60;

	doShutdown   = false;
	shutdownTime = time(NULL) + secondsLeft;

	QIcon cnclIcon = qh_loadStockIcon(QStyle::SP_DialogCancelButton, NULL);
	QIcon okIcon   = qh_loadStockIcon(QStyle::SP_DialogOkButton, NULL);
	QIcon pic      = qh_loadIcon(ICONS_TIMER);
	QIcon tIcon    = pic;

	label		    = new QLabel("");
	timer		    = new QTimer(this);
	trayIcon	    = new QSystemTrayIcon(tIcon, this);
	QLabel	    *icon   = new QLabel;
	QHBoxLayout *hbox   = new QHBoxLayout;
	QHBoxLayout *btHbox = new QHBoxLayout;
	QVBoxLayout *layout = new QVBoxLayout(this);
	QPushButton *cancel = new QPushButton(cnclIcon, tr("Cancel timer"));
	QPushButton *ok	    = new QPushButton(okIcon, tr("Ok"));

	setLabelText(hours, minutes);
	trayIcon->setToolTip(label->text());
	icon->setPixmap(pic.pixmap(72));
	setWindowIcon(tIcon);

	hbox->setSpacing(20);
	hbox->addWidget(icon);
	hbox->addWidget(label, 1, Qt::AlignHCenter);

	btHbox->setSpacing(2);
	btHbox->addWidget(ok, 1, Qt::AlignRight);
	btHbox->addWidget(cancel, 0, Qt::AlignRight);

	layout->addLayout(hbox);
	layout->addLayout(btHbox);

	connect(trayIcon,
	    SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this,
	    SLOT(trayClicked(QSystemTrayIcon::ActivationReason)));
	connect(timer, SIGNAL(timeout()), this, SLOT(update()));
	connect(ok, SIGNAL(clicked()), this, SLOT(hideWin()));
	connect(cancel, SIGNAL(clicked()), this, SLOT(reject()));

	if (secondsLeft <= 60)
		timer->start(1000);
	else
		timer->start(60000);
	trayIcon->show();
	show();
	setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter,
	    size(), qApp->desktop()->availableGeometry()));
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

void Countdown::trayClicked(QSystemTrayIcon::ActivationReason reason)
{
	if (reason == QSystemTrayIcon::Trigger || 
	    reason == QSystemTrayIcon::DoubleClick) {
		if (!isVisible())
			setVisible(true);
		else
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
		trayIcon->setToolTip(label->text());
	} else if (secondsLeft > 0) {
		int hours = secondsLeft / (60 * 60);
		int minutes = (secondsLeft - hours * 60 * 60) / 60;
		setLabelText(hours, minutes);
		trayIcon->setToolTip(label->text());
	}
	if (secondsLeft <= 0) {
		doShutdown = true;
		accept();
	}
}

