#include "auton.h"
#include "main.h"
#include "misc.h"

// This is where the auton movements are programmed 
int nothingState() { DEBUGLOG("State - ", "Nothing") return auton::nothing; };
int leftState() {
    DEBUGLOG("State - ", "Left")
    
    // Dp nothing

    return auton::nothing;
};
int rightState() {
    DEBUGLOG("State - ", "Right")

    chassis->setState({0_tile, 0_tile, 0_deg});
    
    wingStateMachine.setState(W_close);
    pros::delay(700);

    chassis->driveToPoint({0_tile, 2_tile});
    chassis->waitUntilSettled();

    chassis->turnToAngle(90_deg);
    chassis->waitUntilSettled();

    chassis->moveDistance(10_in);
    
    wingStateMachine.setState(W_pos3);
    pros::delay(500);
    
    chassis->moveDistance(-1_tile);

    // chassis->driveToPoint({2.5_tile, -2_tile});

    // wingStateMachine.setState(W_CataAlign);


    return auton::nothing;
};
int skillState() {
    DEBUGLOG("State - ", "Skills")

    chassis->setState({0_tile, 0_tile, 0_deg});

    chassis->driveToPoint({-0.5_tile, 0.5_tile});
    chassis->turnToPoint({-1_tile, 0_tile});
    chassis->moveDistance(0.6_tile);

    startCatapult();
    pros::delay(55*1000);
    stopCatapult();

    // chassis->moveDistance(-10_in);

    // chassis->driveToPoint({0_tile, 0.4_tile});

    // chassis->setState({0_tile, 0_tile, 135_deg});
    // chassis->driveToPoint({3_tile, 0.2_tile});
    // chassis->driveToPoint({3_tile, 1_tile});
    // chassis->driveToPoint({2.5_tile, 2.5_tile});
    
    // chassis->turnToAngle(90_deg);

    // wingStateMachine.setState(W_pos2);
    // pros::delay(500);

    // chassis->moveDistance(2_tile);
    
    // chassis->moveDistance(-2_tile);

    return auton::nothing;
}

void auton::AutonSystem::initialize() {
    DEBUGLOG("Auton - Initializing")

    addMode(auton::nothing, "Nothing", nothingState);
    addMode(auton::left, "Left", leftState);
    addMode(auton::right, "Right", rightState);
    addMode(auton::skill, "Skills", skillState);

    if (readFile("/usd/lastMode.txt") > 100) { // SD Card is being weird
        // TODO: Make warning system
        DEBUGLOG("SD Card being weird")
        mode = nothing;
    } else {
        mode = readFile("/usd/lastMode.txt");
    }

    if (readFile("/usd/isTank.txt") > 100) {
        DEBUGLOG("SD Card being weird")
        isTankDrive = true;
    } else {
        isTankDrive = readFile("/usd/isTank.txt");
    }

    this->stateMachine.start(mode); // Set the nothing state as default
}
void auton::AutonSystem::autonInitialize() {
    DEBUGLOG("Auton - Match Initialize")
} // Do nothing for now
void auton::AutonSystem::disabled() { 
    DEBUGLOG("Auton - Disabled")
    this->stateMachine.setState(auton::nothing); 
}
void auton::AutonSystem::started() {
    DEBUGLOG("Auton - Started")
    this->stateMachine.setState(mode);
    this->stateMachine.iterate();

}

const int screenWidth = 480;
const int screenHeight = 240;
void auton::AutonSystem::screenPressed() {
    auto status =  pros::screen::touch_status();
    int xPos = status.x;
    int yPos = status.y;
    // DEBUGLOG("Screen Press: ( X: ", xPos, ", Y: ", yPos, " )")


    // Check for auton button
    auto states = this->stateMachine.getStates();
    int stateAmt = states.size();
    if (xPos > 380) {
        int btnHeight = (screenHeight - 10) / stateAmt;
        mode = (yPos - 5) / btnHeight;
        writeFile("/usd/lastMode.txt", mode);
    }

    if (xPos < 100 && yPos > screenHeight - ((screenHeight - 10) / stateAmt) * 2) {
        int btnHeight = (screenHeight - 10) / stateAmt;
        isTankDrive = (yPos - 5) / btnHeight != 3;
        writeFile("/usd/isTank.txt", isTankDrive);
    }

    updateScreen = true;

}

void auton::AutonSystem::renderScreen() {
    if (!updateScreen && initialRenders < 0) { return; }

    updateScreen = false;
    initialRenders--;

    auto states = this->stateMachine.getStates();
    int stateAmt = states.size();
    
    // 480 x 240

    pros::screen::erase();
    pros::screen::set_pen(COLOR_WHITE);
    pros::screen::draw_rect(5, 5, 475, 235);
    pros::screen::print(pros::E_TEXT_MEDIUM, 7, 7, "BURT V2 (pros)");

    // Draw auton Buttons
    int btnHeight = (screenHeight - 10) / stateAmt;
    for (int i = 0; i < stateAmt; i++) {
        if (i == mode) {
            pros::screen::set_pen(COLOR_WHITE);
            pros::screen::draw_rect(380, 5 + ( btnHeight * i), 475, 5 + ( btnHeight * (i+1)));
            pros::screen::set_pen(COLOR_GREEN);
            pros::screen::print(pros::E_TEXT_MEDIUM_CENTER, 390, 5 + ( btnHeight * i) + (btnHeight/2), stateNames[i].c_str());
        } else {
            pros::screen::set_pen(COLOR_WHITE);
            pros::screen::draw_rect(380, 5 + ( btnHeight * i), 475, 5 + ( btnHeight * (i+1)));
            pros::screen::print(pros::E_TEXT_MEDIUM_CENTER, 390, 5 + ( btnHeight * i) + (btnHeight/2), stateNames[i].c_str());
        }
    }

    // Draw Drive Mode Button
    pros::screen::draw_rect(5, screenHeight - 5, 100, screenHeight - 5 - btnHeight);
    pros::screen::draw_rect(5, screenHeight - 5 - btnHeight, 100, screenHeight - 5 - btnHeight * 2);

    pros::screen::set_pen(isTankDrive? COLOR_WHITE : COLOR_GREEN);
    pros::screen::print(pros::E_TEXT_MEDIUM_CENTER, 15, screenHeight - 5 - (btnHeight/2), "Arcade");
    pros::screen::set_pen(isTankDrive? COLOR_GREEN : COLOR_WHITE);
    pros::screen::print(pros::E_TEXT_MEDIUM_CENTER, 15, screenHeight - 5 - (btnHeight/2) - btnHeight, "Tank");



}