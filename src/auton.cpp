#include "auton.h"
#include "main.h"
#include "misc.h"

// This is where the auton movements are programmed 
int nothingState() { DEBUGLOG("State - ", "Nothing") return auton::nothing; };
int leftState() {
    DEBUGLOG("State - ", "Left")



    return auton::nothing;
};
int rightState() {
    DEBUGLOG("State - ", "Right")




    return auton::nothing;
};
int skillState() {
    DEBUGLOG("State - ", "Skills")




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



}