#include <FL/Fl.H>

#include "controller/game_controller.h"

int main() {
    Fl::lock();

    GameController gameController;
    gameController.run();

    return 0;
}
