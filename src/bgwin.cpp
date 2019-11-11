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

#include <QImage>
#include <QApplication>
#include <QLabel>
#include <QPixmap>
#include <QScreen>
#include <QWindow>
#include <QDesktopWidget>

#include "bgwin.h"

BgWin::BgWin(QWidget *parent) : QMainWindow(parent) {
	QList <QScreen *> screens = QGuiApplication::screens();

	for (int i = 0; i < screens.size(); i++) {
		QRect g = screens.at(i)->geometry();
		QPixmap pixmap = screens.at(i)->grabWindow(0,
		    g.topLeft().rx(), g.topLeft().ry(), g.width(), g.height());
		QImage image = pixmap.toImage();
		for (int y = 0; y < image.height(); y++) {
			for (int x = 0; x < image.width(); x++) {
				QRgb c = image.pixel(x, y);
				image.setPixel(x, y, qRgb(qRed(c) >> 2,
				    qGreen(c) >> 2, qBlue(c) >> 2));
			}
		}
		pixmap = QPixmap::fromImage(image);
		QLabel *content = new QLabel(this);
		content->setPixmap(pixmap);
		content->move(QPoint(g.topLeft().rx(), g.topLeft().ry()));
		if (i == 0)
			setCentralWidget(content);
		content->setWindowFlags(Qt::Window | Qt::FramelessWindowHint |
		    Qt::WindowStaysOnTopHint);
		content->showFullScreen();
	}
}

