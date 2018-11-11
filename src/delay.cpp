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
#include "delay.h"
#include "qt-helper/qt-helper.h"

Delay::Delay(int seconds, const QString &actionName, const QString &text,
	const QIcon &icon, QWidget *parent) : QDialog(parent)
{
	secondsLeft = seconds; this->text = text;

	label		    = new QLabel("");
	timer		    = new QTimer(this);
	QIcon cnclIcon	    = qh_loadStockIcon(QStyle::SP_DialogCancelButton, 0);
	QHBoxLayout *hbox   = new QHBoxLayout;
	QHBoxLayout *btHbox = new QHBoxLayout;
	QVBoxLayout *layout = new QVBoxLayout(this);
	QPushButton *cancel = new QPushButton(cnclIcon, tr("Cancel"));
	QPushButton *action = new QPushButton(icon, actionName);

	setLabelText(text, seconds);
	setWindowIcon(icon);

	hbox->setSpacing(20);
	hbox->addWidget(label, 1, Qt::AlignHCenter);

	btHbox->setSpacing(2);
	btHbox->addWidget(action, 1, Qt::AlignRight);
	btHbox->addWidget(cancel, 0, Qt::AlignRight);

	layout->setSpacing(10);
	layout->addLayout(hbox);
	layout->addLayout(btHbox);
	layout->setContentsMargins(15, 15, 15, 15);

	connect(timer,  SIGNAL(timeout()), this, SLOT(update()));
	connect(action, SIGNAL(clicked()), this, SLOT(accept()));
	connect(cancel, SIGNAL(clicked()), this, SLOT(reject()));

	setModal(true);
	setWindowFlags(windowFlags() | Qt::Dialog | Qt::FramelessWindowHint |
	    Qt::WindowStaysOnTopHint | Qt::BypassWindowManagerHint);
	show();
	setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter,
	    parent->size(), qApp->desktop()->availableGeometry()));
	timer->start(1000);
}

void Delay::setLabelText(const QString &text, int seconds)
{
	QString s;

	if (seconds > 0) {
		s = text + " %1 %2";
		s = s.arg(seconds);
		s = s.arg(seconds == 1 ? tr("second") : tr("seconds"));
		label->setText(s);
	}
}

void Delay::keyPressEvent(QKeyEvent *e) {
	if (e->key() == Qt::Key_Escape)
		reject();
	QDialog::keyPressEvent(e);
}

void Delay::closeEvent(QCloseEvent *event)
{
	hide();
	event->ignore();
}

void Delay::update()
{
	if (secondsLeft-- > 0)
		setLabelText(text, secondsLeft);
	else if (secondsLeft <= 0)
		accept();
}

