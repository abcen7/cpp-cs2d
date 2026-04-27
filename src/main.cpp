#include <FL/Fl.H>
#include <FL/Fl_Shared_Image.H>

#include "controller/game_controller.h"

int main() {
    Fl::lock();
    fl_register_images();

    GameController gameController;
    gameController.run();

    return 0;
}
