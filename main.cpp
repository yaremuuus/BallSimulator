#include <QApplication>
#include "BallSimulator.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    BasketGame game;
    game.resize(500, 600);
    game.show();

    return app.exec();
}