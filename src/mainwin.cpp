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
#include <QStyle>
#include <QDesktopWidget>

#include "icons.h"
#include "mainwin.h"
#include "bgwin.h"
#include "qt-helper/qt-helper.h"

#define PB_STYLE "padding: 2px; text-align: left;"

Mainwin::Mainwin(QWidget *parent) : QDialog(parent)
{
	QIcon pic	    = qh_loadIcon(ICONS_SHUTDOWN);
	QIcon icon_logout   = qh_loadIcon(ICONS_LOGOUT);
	QIcon icon_shutdown = qh_loadIcon(ICONS_SHUTDOWN);
	QIcon icon_suspend  = qh_loadIcon(ICONS_SUSPEND);
	QIcon icon_timer    = qh_loadIcon(ICONS_TIMER);
	QIcon icon_reboot   = qh_loadIcon(ICONS_REBOOT);
	QIcon icon_cancel   = qh_loadStockIcon(QStyle::SP_DialogCancelButton,
					       NULL);
	QLabel	    *icon	 = new QLabel;
	QHBoxLayout *hbox	 = new QHBoxLayout;
	QVBoxLayout *vbox	 = new QVBoxLayout;
	QVBoxLayout *layout	 = new QVBoxLayout(this);
	QPushButton *pb_timer	 = new QPushButton(icon_timer,  tr("Timer"));
	QPushButton *pb_logout	 = new QPushButton(icon_logout, tr("Logout"));
	QPushButton *pb_reboot	 = new QPushButton(icon_reboot,
						   tr("Reboot system"));
	QPushButton *pb_shutdown = new QPushButton(icon_shutdown,
						   tr("Shutdown system"));
	QPushButton *pb_suspend  = new QPushButton(icon_suspend,
						   tr("Suspend system"));
	QPushButton *pb_cancel   = new QPushButton(icon_cancel, tr("Cancel"));

	pb_timer->setStyleSheet(PB_STYLE);
	pb_logout->setStyleSheet(PB_STYLE);
	pb_reboot->setStyleSheet(PB_STYLE);
	pb_shutdown->setStyleSheet(PB_STYLE);
	pb_timer->setStyleSheet(PB_STYLE);
	pb_cancel->setStyleSheet(PB_STYLE);
	pb_suspend->setStyleSheet(PB_STYLE);

	icon->setPixmap(pic.pixmap(96));
	hbox->addWidget(icon);

	vbox->addWidget(pb_logout);
	vbox->addWidget(pb_reboot);
	vbox->addWidget(pb_shutdown);
	vbox->addWidget(pb_suspend);
	vbox->addWidget(pb_timer);
	vbox->addWidget(pb_cancel);
	hbox->addSpacing(20);
	hbox->addLayout(vbox);
	layout->addLayout(hbox);

	connect(pb_timer,    SIGNAL(clicked()), this, SLOT(timerClicked()));
	connect(pb_logout,   SIGNAL(clicked()), this, SLOT(logoutClicked()));
	connect(pb_reboot,   SIGNAL(clicked()), this, SLOT(rebootClicked()));
	connect(pb_shutdown, SIGNAL(clicked()), this, SLOT(shutdownClicked()));
	connect(pb_suspend,  SIGNAL(clicked()), this, SLOT(suspendClicked()));
	connect(pb_cancel,   SIGNAL(clicked()), this, SLOT(reject()));
	setModal(true);
	setWindowFlags(windowFlags() | Qt::Dialog | Qt::FramelessWindowHint |
	    Qt::WindowStaysOnTopHint);
	show();
	setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter,
	    size(), qApp->desktop()->availableGeometry()));
}

void Mainwin::logoutClicked()
{
	button = LOGOUT;
	accept();
}

void Mainwin::rebootClicked()
{
	button = REBOOT;
	accept();
}

void Mainwin::shutdownClicked()
{
	button = SHUTDOWN;
	accept();
}

void Mainwin::suspendClicked()
{
	button = SUSPEND;
	accept();
}

void Mainwin::timerClicked()
{
	button = TIMER;
	accept();
}

int Mainwin::getButton()
{
	return (button);
}

