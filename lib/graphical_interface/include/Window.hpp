#pragma once

#include <iostream>
#include <vector>

#include <QListWidget>
#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>

#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 800

#define USER_NAME_FONT QFont("Arial", 14)

class Window : public QWidget {
  public:
    Window(const QString &title)
    {
      setWindowTitle(title);
      setMinimumSize(WINDOW_WIDTH, WINDOW_HEIGHT);

      // main layout
      mainLayout = new QHBoxLayout(this);

      // side menu
      sideMenu = new QListWidget(this);
      sideMenu->setFixedWidth(300);
      sideMenu->setFixedHeight(WINDOW_HEIGHT);

      chat = new QListWidget(this);
      chat->setFixedWidth(WINDOW_WIDTH - 300);
      chat->setFixedHeight(WINDOW_HEIGHT);


      mainLayout->addWidget(sideMenu);
      mainLayout->addWidget(chat);
    }

    virtual ~Window() {}

    void addItemToSideMenu(const QString &item)
    {
      QListWidgetItem *newItem = new QListWidgetItem(item);
      newItem->setFont(USER_NAME_FONT);
      sideMenu->addItem(newItem);
    }

    void show()
    {
      QWidget::show();
    }
  private:
    QListWidget *sideMenu;
    QListWidget *chat;

    QHBoxLayout *mainLayout;
    QTextEdit *textEdit;

    std::vector<QWidget*> widgets;
};
