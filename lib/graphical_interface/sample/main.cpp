#include <iostream>
#include <QApplication>
#include "Window.hpp"

int main(int ac, char **av)
{
  QApplication app(ac, av);
  Window window("My Window");

  window.show();
  return app.exec();
}
