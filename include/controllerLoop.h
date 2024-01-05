#pragma once

#include "stateMachine.h"
#include <string>


enum WingStates {
    W_loose, W_pos1, W_pos2, W_pos3, W_close, W_CataAlign, W_LeftAutonPoleTouch
};
extern StateMachine wingStateMachine;

void startCatapult();
void stopCatapult();

void controllerLoop();
void warnUser(std::string message);