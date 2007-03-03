#include <QApplication>
#include <QFont>
#include <QPushButton>
#include <QWidget>
#include <QColor>
#include <QPalette>

#include "board.h"
#include "dim.h"
#include "aiboard.h"

using namespace ai;
using namespace luffarschack;
    
int WIDTH = 20;
int HEIGHT = 20;
int BTN_WIDTH = 20;
int BTN_HEIGHT = 20;
int BTN_MARGIN_WIDTH = 0;
int BTN_MARGIN_HEIGHT = 0;
int MARGIN_WIDTH = 10;
int MARGIN_HEIGHT = 10;

class BovoWidget : public QWidget {
public:
  BovoWidget(QWidget *parent = 0);
};

BovoWidget::BovoWidget(QWidget *parent) : QWidget(parent) {
  board b(dim(WIDTH, HEIGHT));
  aiboard p0(WIDTH, HEIGHT);
  aiboard p1(WIDTH, HEIGHT);
  unsigned short int player = 1;
   
  setFixedSize(WIDTH  * (BTN_WIDTH  + BTN_MARGIN_WIDTH ) + 2 * MARGIN_WIDTH  - BTN_MARGIN_WIDTH, 
               HEIGHT * (BTN_HEIGHT + BTN_MARGIN_HEIGHT) + 2 * MARGIN_HEIGHT - BTN_MARGIN_HEIGHT);
  
  QPushButton ***btn = new QPushButton**[WIDTH];
  for (int x = 0; x < WIDTH; ++x) {
    btn[x] = new QPushButton*[HEIGHT];
    for (int y = 0; y < HEIGHT; ++y) {
      btn[x][y] = new QPushButton(tr(" "), this); 
      btn[x][y]->setGeometry(MARGIN_WIDTH  + x * (BTN_WIDTH  + BTN_MARGIN_WIDTH ), 
                             MARGIN_HEIGHT + y * (BTN_HEIGHT + BTN_MARGIN_HEIGHT),
                             BTN_WIDTH,
                             BTN_HEIGHT);
      btn[x][y]->setFont(QFont("Times", 14, QFont::Bold));
    }
  }
  
  QPalette red(Qt::darkRed);
  QPalette blue(Qt::darkBlue);
  
  coord c(-1, -1);
  while (true) {
    player = player%2+1;
    c = p0.move(c);
    btn[c.x][c.y]->setText("X");
    btn[c.x][c.y]->setPalette(blue);
    if (b.setPlayer(c, player)) break;
    player = player%2+1;
    c = p1.move(c);
    btn[c.x][c.y]->setText("O");
    btn[c.x][c.y]->setPalette(red);
    if (b.setPlayer(c, player)) break;
  }
}

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  BovoWidget widget;
  widget.show();
  return app.exec();
}

