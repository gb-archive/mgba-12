#ifndef QGBA_WINDOW
#define QGBA_WINDOW

#include <QAudioOutput>
#include <QMainWindow>

#include "GameController.h"
#include "Display.h"

#include "ui_Window.h"

namespace QGBA {

class Window : public QMainWindow, Ui::GBAWindow {
Q_OBJECT

public:
	Window(QWidget* parent = 0);

public slots:
	void selectROM();

private slots:
	void setupAudio(GBAAudio*);

private:
	GameController* m_controller;
	Display* m_display;
};

}

#endif
