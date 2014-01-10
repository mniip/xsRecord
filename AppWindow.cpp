#include <QtGui/QLineEdit>
#include <QtCore/QTimer>
#include <QtGui/QApplication>
#include <QtGui/QLabel>
#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QFileDialog>

#include "X.h"
#include "Queue.h"
#include "AppWindow.h"

#include <stdio.h>
#include <unistd.h>

void AppWindow::startStop()
{
	if(recording)
	{
		StartStop->setText(QString::fromUtf8("\xE2\x97\x8F"));
		recording = 0;
		Queue::Stop();
	}
	else
	{
		StartStop->setText(QString::fromUtf8("\xE2\x97\xBE"));
		recording = 1;
		setWindowState(Qt::WindowMinimized);
		Queue::Start(Output->text().toUtf8().constData());
		int fps = Fps->value();
		unsigned long int sleep = 1000000/fps;
		while(recording)
		{
			usleep(sleep);
			qApp->processEvents();
			int width = selection[2] - selection[0];
			int height = selection[3] - selection[1];
			Queue::Push(X::Pull(selection[0], selection[1], width, height));
		}
	}
}

void AppWindow::selectRange()
{
	selection = X::SelectRange();
}

void AppWindow::selectWindow()
{
	selection = X::SelectWindow();
}

void AppWindow::screenshot()
{
	Queue::Start(Output->text().toUtf8().constData());
	int width = selection[2] - selection[0];
	int height = selection[3] - selection[1];
	Queue::Push(X::Pull(selection[0], selection[1], width, height));
	Queue::Stop();
}

void AppWindow::browse()
{
	Output->setText(QFileDialog::getExistingDirectory(this, tr("Select output directory"), Output->text(), QFileDialog::ShowDirsOnly));
}

void AppWindow::update()
{
	int width = selection[2] - selection[0];
	int height = selection[3] - selection[1];
	QImage q(width, height, QImage::Format_RGB32);
	XImage *image = X::Pull(selection[0], selection[1], width, height);
	memcpy(q.bits(), image->data, image->bytes_per_line*image->height);
	X::Destroy(image);
	Preview->setPixmap(QPixmap::fromImage(q.scaled(Preview->width(), Preview->height(), Qt::KeepAspectRatio, Qt::SmoothTransformation)));
}

AppWindow::AppWindow(QWidget *parent):
	QWidget(parent),
	recording(0)
{
	X::Init();
	selection.push_back(0);
	selection.push_back(0);
	selection.push_back(X::Width);
	selection.push_back(X::Height);

	Preview = new QLabel;
	Preview->setAlignment(Qt::AlignCenter);
	Preview->setMargin(-30);
	Preview->setStyleSheet("background: black;");

	QLabel *previewLabel = new QLabel(tr("Preview:"));

	QVBoxLayout *previewL = new QVBoxLayout;
	previewL->addWidget(previewLabel, 0);
	previewL->addWidget(Preview, 1);

	QPushButton *selectRange = new QPushButton(tr("Select range"));
	connect(selectRange, SIGNAL(clicked()), this, SLOT(selectRange()));

	QPushButton *selectWindow = new QPushButton(tr("Select window"));
	connect(selectWindow, SIGNAL(clicked()), this, SLOT(selectWindow()));

	StartStop = new QPushButton(QString::fromUtf8("\xE2\x97\x8F"));
	connect(StartStop, SIGNAL(clicked()), this, SLOT(startStop()));
	StartStop->setMaximumWidth(24);

	QPushButton *screenshot = new QPushButton(tr("Screenshot"));
	connect(screenshot, SIGNAL(clicked()), this, SLOT(screenshot()));

	QHBoxLayout *screenshotL = new QHBoxLayout;
	screenshotL->addWidget(StartStop);
	screenshotL->addWidget(screenshot);

	QLabel *outputLabel = new QLabel(tr("Output directory:"));

	Output = new QLineEdit(tr("./"));

	QPushButton *browse = new QPushButton(tr("..."));
	connect(browse, SIGNAL(clicked()), this, SLOT(browse()));
	browse->setMaximumWidth(24);

	QHBoxLayout *outputL = new QHBoxLayout;
	outputL->addWidget(Output);
	outputL->addWidget(browse);

	QLabel *fpsLabel = new QLabel(tr("FPS:"));
	
	Fps = new QSpinBox();
	Fps->setMaximum(120);
	Fps->setMinimum(1);
	Fps->setValue(25);

	QVBoxLayout *controlsL = new QVBoxLayout;
	controlsL->addWidget(selectRange);
	controlsL->addWidget(selectWindow);
	controlsL->addLayout(screenshotL);
	controlsL->addWidget(outputLabel);
	controlsL->addLayout(outputL);
	controlsL->addWidget(fpsLabel);
	controlsL->addWidget(Fps);
	controlsL->addStretch();

	QHBoxLayout *mainL = new QHBoxLayout;
	mainL->addLayout(previewL, 0);
	mainL->addLayout(controlsL, 1);
	mainL->setStretch(0, 1);
	mainL->setStretch(1, 0);

	QTimer *timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(update()));
	timer->start(40);

	setLayout(mainL);
	setMinimumWidth(300);
	setWindowTitle(tr("xsRecord"));
}
