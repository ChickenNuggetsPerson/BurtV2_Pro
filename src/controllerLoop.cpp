#include "controllerLoop.h"
#include "stateMachine.h"
#include "main.h"




// Variables for xButton System
const double Wing_rotationOne = 50;
const double Wing_rotationTwo = 950;
const double Wing_rotationThree = 2000;
const double Wing_rotationClose = 2400;
const int Wing_maxVel = 10000;


bool wingsClosed = false; // Define wings closed variable

// Called when the X button is pressed
void toggleClose() {
    wingsClosed = !wingsClosed;
    if (!wingsClosed) {
        rightArm_Moter.move_absolute(Wing_rotationThree, Wing_maxVel);
        leftArm_Moter.move_absolute(Wing_rotationThree, Wing_maxVel);
    }
}

// X Button press handling system
auto xButton = ButtonSystem(&masterController, pros::controller_digital_e_t::E_CONTROLLER_DIGITAL_X, false, toggleClose);

// Wings logic
void wingsCheck() {
    if (masterController.get_digital(pros::E_CONTROLLER_DIGITAL_Y)) {
        wingsClosed = false;
        rightArm_Moter.move_absolute(Wing_rotationOne, Wing_maxVel);
        leftArm_Moter.move_absolute(Wing_rotationOne, Wing_maxVel);
    }
    if (masterController.get_digital(pros::E_CONTROLLER_DIGITAL_B)) {
        wingsClosed = false;
        rightArm_Moter.move_absolute(Wing_rotationTwo, Wing_maxVel);
        leftArm_Moter.move_absolute(Wing_rotationTwo, Wing_maxVel);
    }
    if (masterController.get_digital(pros::E_CONTROLLER_DIGITAL_A)) {
        wingsClosed = false;
        rightArm_Moter.move_absolute(Wing_rotationThree, Wing_maxVel);
        leftArm_Moter.move_absolute(Wing_rotationThree, Wing_maxVel);
    }
    if (wingsClosed) {
        rightArm_Moter.move_absolute(Wing_rotationClose, Wing_maxVel);
        leftArm_Moter.move_absolute(Wing_rotationClose, Wing_maxVel);
    }

    xButton.check();
}


// The main loop for the controller
// Handles motor movement and button input
void controllerLoop() {

    // Main loop
    while (true) {

        wingsCheck();

        pros::delay(20);
    }
}