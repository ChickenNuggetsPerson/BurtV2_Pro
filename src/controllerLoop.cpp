#include "controllerLoop.h"
#include "stateMachine.h"
#include "main.h"

using namespace pros;


// Variables for xButton System
const double Wing_rotationOne = 100;
const double Wing_rotationTwo = 1000;
const double Wing_rotationThree = 2100;
const double Wing_rotationClose = 2490;
const int Wing_maxVel = 10000;

// Wings logic
int getBtnPressed() {
    if (masterController.get_digital(E_CONTROLLER_DIGITAL_B))
        return W_pos1;
    if (masterController.get_digital(E_CONTROLLER_DIGITAL_A))
        return W_pos2;
    if (masterController.get_digital(E_CONTROLLER_DIGITAL_Y))
        return W_pos3;
    if (masterController.get_digital(E_CONTROLLER_DIGITAL_X))
        return W_close;
    if (masterController.get_digital(E_CONTROLLER_DIGITAL_DOWN))
        return W_CataAlign;

    if (masterController.get_digital(E_CONTROLLER_DIGITAL_R1) || masterController.get_digital(E_CONTROLLER_DIGITAL_R2) || masterController.get_digital(E_CONTROLLER_DIGITAL_L1) || masterController.get_digital(E_CONTROLLER_DIGITAL_L2))
        return W_loose;

    return -1;
}
int wingLooseState() {

    int leftDir = 0;
    int rightDir = 0;

    if (masterController.get_digital(E_CONTROLLER_DIGITAL_R1)) 
        rightDir++;
    if (masterController.get_digital(E_CONTROLLER_DIGITAL_R2)) 
        rightDir--;

    if (masterController.get_digital(E_CONTROLLER_DIGITAL_L1)) 
        leftDir++;
    if (masterController.get_digital(E_CONTROLLER_DIGITAL_L2)) 
        leftDir--;

    leftArm_Moter.move_voltage(10000 * leftDir);
    rightArm_Moter.move_voltage(10000 * rightDir);

    if (int btn = getBtnPressed(); btn != -1) { return btn; }

    return W_loose;
}
int wingPos1State() {
    rightArm_Moter.move_absolute(Wing_rotationOne, Wing_maxVel);
    leftArm_Moter.move_absolute(Wing_rotationOne, Wing_maxVel);

    if (int btn = getBtnPressed(); btn != -1) { return btn; }

    return W_pos1;
}
int wingPos2State() {
    rightArm_Moter.move_absolute(Wing_rotationTwo, Wing_maxVel);
    leftArm_Moter.move_absolute(Wing_rotationTwo, Wing_maxVel);

    if (int btn = getBtnPressed(); btn != -1) { return btn; }

    return W_pos2;
}
int wingPos3State() {
    rightArm_Moter.move_absolute(Wing_rotationThree, Wing_maxVel);
    leftArm_Moter.move_absolute(Wing_rotationThree, Wing_maxVel);

    if (int btn = getBtnPressed(); btn != -1) { return btn; }

    return W_pos3;
}
int wingCloseState() {
    rightArm_Moter.move_absolute(Wing_rotationClose, Wing_maxVel);
    leftArm_Moter.move_absolute(Wing_rotationClose, Wing_maxVel);

    if (int btn = getBtnPressed(); btn != -1) { return btn; }

    return W_close;
}
int wingCataAlignState() {
    
    rightArm_Moter.move_absolute(1500, Wing_maxVel);
    leftArm_Moter.move_absolute(W_pos1 + 50, Wing_maxVel);

    if (int btn = getBtnPressed(); btn != -1) { return btn; }

    return W_CataAlign;
}
int wingLeftAutonState() {
    
    rightArm_Moter.move_absolute(W_pos1, Wing_maxVel);
    leftArm_Moter.move_absolute(1500, Wing_maxVel);

    if (int btn = getBtnPressed(); btn != -1) { return btn; }

    return W_LeftAutonPoleTouch;
}

bool cata_autoLaunch = false;
void startCatapult() {
    cata_autoLaunch = true;
}
void stopCatapult() {
    cata_autoLaunch = false;
}
void toggleCata() {
    cata_autoLaunch = !cata_autoLaunch;
}
auto upBtn = ButtonSystem(&masterController, E_CONTROLLER_DIGITAL_UP, false, startCatapult, stopCatapult, 0.00);
auto leftBtn = ButtonSystem(&masterController, E_CONTROLLER_DIGITAL_LEFT, true, toggleCata, toggleCata, 10);
void catapultCheck() {
    upBtn.check();
    leftBtn.check();

    if (cata_autoLaunch) {
        cata_Motor.move_voltage(10000);
    } else {
        cata_Motor.move_voltage(0);
    }
}


StateMachine wingStateMachine = StateMachine();

// The main loop for the controller
// Handles motor movement and button input
__attribute__((noreturn)) void controllerLoop() {

    wingStateMachine.addState(W_loose, wingLooseState);
    wingStateMachine.addState(W_pos1, wingPos1State);
    wingStateMachine.addState(W_pos2, wingPos2State);
    wingStateMachine.addState(W_pos3, wingPos3State);
    wingStateMachine.addState(W_close, wingCloseState);
    wingStateMachine.addState(W_CataAlign, wingCataAlignState);
    wingStateMachine.addState(W_LeftAutonPoleTouch, wingLeftAutonState);
    wingStateMachine.start(W_pos1);

    // Main loop
    while (true) {

        wingStateMachine.iterate();
        catapultCheck();

        delay(20);
    }
}


void warnUser(std::string message) {
    masterController.rumble("..");
    masterController.clear();
    masterController.set_text(1, 1, message);
};