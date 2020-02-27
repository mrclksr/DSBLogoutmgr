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

#include <QDialog>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QSpinBox>
#include <QLabel>
#include <QIcon>
#include <QPixmap>
#include <QTimer>
#include <QCloseEvent>
#include <QSystemTrayIcon>
#include <time.h>

class Countdown : public QDialog
{
	Q_OBJECT
public:
	Countdown(int hours, int minutes, int fifo, QWidget *parent = 0);
	bool shutdown();
private:
	void setLabelText(int hours, int minutes, int seconds = 0);
	void createTrayIcon();
private slots:
	void update();
	void hideWin();
	void changeTimer();
	void handleFIFO(int fifo);
	void trayClicked(QSystemTrayIcon::ActivationReason reason);
	void closeEvent(QCloseEvent *event);
	void keyPressEvent(QKeyEvent *e);
	void checkForSysTray();
	void scrGeomChanged(const QRect &);
private:
	bool   doShutdown;
	time_t shutdownTime;
	QIcon  tIcon;
	QTimer *timer;
	QTimer *trayTimer;
	QLabel *label;
	QSystemTrayIcon *trayIcon;
};

