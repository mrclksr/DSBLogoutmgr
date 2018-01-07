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
#include "timerwin.h"
#include "qt-helper/qt-helper.h"

Timerwin::Timerwin(QWidget *parent) : QDialog(parent)
{
	QIcon pic      = qh_loadIcon(ICONS_TIMER);
	QIcon okIcon   = qh_loadStockIcon(QStyle::SP_DialogOkButton, NULL);
	QIcon cnclIcon = qh_loadStockIcon(QStyle::SP_DialogCancelButton, NULL);

	hourSb		     = new QSpinBox;
	minSb		     = new QSpinBox;
	QLabel	    *icon    = new QLabel;
	QLabel	    *label   = new QLabel(QString(tr("Set shutdown time " \
					  "in hours and/or minutes:")));
	QHBoxLayout *hbox    = new QHBoxLayout;
	QHBoxLayout *btHbox  = new QHBoxLayout;
	QVBoxLayout *vbox    = new QVBoxLayout;
	QHBoxLayout *hLayout = new QHBoxLayout;
	QVBoxLayout *vLayout = new QVBoxLayout(this);
	QPushButton *ok	     = new QPushButton(okIcon, tr("&Ok"));
	QPushButton *cancel  = new QPushButton(cnclIcon, tr("&Cancel"));

	icon->setPixmap(pic.pixmap(96));
	hourSb->setSuffix(tr(" Hours"));
	minSb->setSuffix(tr(" Minutes"));
	minSb->setMinimum(-1);
	minSb->setMaximum(60);
	minSb->setValue(minutes);
	hourSb->setValue(hours);

	hbox->addWidget(hourSb);
	hbox->addWidget(minSb);

	btHbox->addWidget(ok, 1, Qt::AlignRight);
	btHbox->addWidget(cancel, 0, Qt::AlignRight);

	vbox->addSpacing(0);
	vbox->addWidget(label, 1, Qt::AlignHCenter);
	vbox->addLayout(hbox);

	hLayout->addWidget(icon);
	hLayout->addLayout(vbox);

	vLayout->setSpacing(10);
	vLayout->addLayout(hLayout);
	vLayout->addLayout(btHbox);

	connect(ok,     SIGNAL(clicked()), this, SLOT(setTimer()));
	connect(cancel, SIGNAL(clicked()), this, SLOT(reject()));
	connect(minSb,  SIGNAL(valueChanged(int)), this, SLOT(checkValue()));
	connect(hourSb, SIGNAL(valueChanged(int)), this, SLOT(checkValue()));
	setModal(true);
	setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
	show();
	setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter,
	    size(), qApp->desktop()->availableGeometry()));
}

void Timerwin::setTimer()
{
	hours = hourSb->value();
	minutes = minSb->value();
	accept();
}

int Timerwin::getHours()
{
	return (hours);
}

int Timerwin::getMinutes()
{
	return (minutes);
}

void Timerwin::setHours(int hours)
{
	hourSb->setValue(hours);
}

void Timerwin::setMinutes(int minutes)
{
	minSb->setValue(minutes);
}

void Timerwin::checkValue()
{
	if (hourSb->value() == 0 && minSb->value() <= 0) {
		minSb->setValue(1);
	} else if (minSb->value() == 60) {
		hourSb->setValue(hourSb->value() + 1);
		minSb->setValue(0);
	} else if (minSb->value() == -1) {
		if (hourSb->value() > 0) {
			hourSb->setValue(hourSb->value() - 1);
			minSb->setValue(59);
		} else
			minSb->setValue(1);
	}
}

