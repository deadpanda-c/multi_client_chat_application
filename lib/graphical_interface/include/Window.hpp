#pragma once

#include <iostream>

#include <QApplication>
#include <QWidget>

#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 800

class Window : public QWidget {
  public:
    Window(const QString &title)
    {
      setWindowTitle(title);
      setMinimumSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    }
    ~Window();

    void show()
    {
      QWidget::show();
    }
  private:




};
