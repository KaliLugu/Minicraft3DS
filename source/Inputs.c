#include "Inputs.h"

Inputs localInputs;

void toggleKey(Key *key, sInt mask) {
    key->down = inputIsPressed(mask);
    key->clicked = inputWasClicked(mask);
}

void tickKeys(Inputs *inputs) {
    inputs->k_touchX = inputGetTouchX();
    inputs->k_touchY = inputGetTouchY();
    toggleKey(&(inputs->k_up), localInputs.k_up.input);
    toggleKey(&(inputs->k_down), localInputs.k_down.input);
    toggleKey(&(inputs->k_left), localInputs.k_left.input);
    toggleKey(&(inputs->k_right), localInputs.k_right.input);
    toggleKey(&(inputs->k_pause), localInputs.k_pause.input);
    toggleKey(&(inputs->k_attack), localInputs.k_attack.input);
    toggleKey(&(inputs->k_menu), localInputs.k_menu.input);
    toggleKey(&(inputs->k_accept), localInputs.k_accept.input);
    toggleKey(&(inputs->k_decline), localInputs.k_decline.input);
    toggleKey(&(inputs->k_delete), localInputs.k_delete.input);
    toggleKey(&(inputs->k_menuNext), localInputs.k_menuNext.input);
    toggleKey(&(inputs->k_menuPrev), localInputs.k_menuPrev.input);
}

void resetKeys(Inputs *inputs) {
    inputs->k_touchX = -1;
    inputs->k_touchY = -1;

    toggleKey(&(inputs->k_up), 0);
    toggleKey(&(inputs->k_down), 0);
    toggleKey(&(inputs->k_left), 0);
    toggleKey(&(inputs->k_right), 0);
    toggleKey(&(inputs->k_pause), 0);
    toggleKey(&(inputs->k_attack), 0);
    toggleKey(&(inputs->k_menu), 0);
    toggleKey(&(inputs->k_accept), 0);
    toggleKey(&(inputs->k_decline), 0);
    toggleKey(&(inputs->k_delete), 0);
    toggleKey(&(inputs->k_menuNext), 0);
    toggleKey(&(inputs->k_menuPrev), 0);
}

void resetClicked(Inputs *inputs) {
    inputs->k_up.clicked = false;
    inputs->k_down.clicked = false;
    inputs->k_left.clicked = false;
    inputs->k_right.clicked = false;
    inputs->k_pause.clicked = false;
    inputs->k_attack.clicked = false;
    inputs->k_menu.clicked = false;
    inputs->k_accept.clicked = false;
    inputs->k_decline.clicked = false;
    inputs->k_delete.clicked = false;
    inputs->k_menuNext.clicked = false;
    inputs->k_menuPrev.clicked = false;
}
