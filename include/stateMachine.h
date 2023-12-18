#pragma once

#include <unordered_map>
#include "api.h"

class ButtonSystem {
    public:
        ButtonSystem(pros::Controller* controllerPtr, pros::controller_digital_e_t button, bool isToggle, void(*pressCB)() = nullptr, void(*releaseCB)() = nullptr) {
            this->controllerPtr = controllerPtr;
            this->buttonID = button;
            this->isToggle = isToggle;
            this->pressCB = pressCB;
            this->releaseCB = releaseCB;
        }
        void check() {
            bool currentState = controllerPtr->get_digital(buttonID);
            
            if (pros::c::millis() < lastPress + pressTimout) { return; } 

            if (isToggle) {


                if (currentState && !prevState) {
                    pressed = !pressed;
                    runCB();
                } // Pressed
                if (!currentState && prevState) {

                } // Released
                

            } else {


                if (currentState && !prevState) {
                    pressed = true;
                    runCB();
                } // Pressed
                if (!currentState && prevState) {
                    pressed = false;
                    runCB();
                } // Released
                

            }

            if (prevState != currentState) {
                lastPress = pros::c::millis();
            }
            prevState = currentState;
        }
        inline bool getStatus() { return pressed; }

    private:
        pros::Controller* controllerPtr;
        pros::controller_digital_e_t buttonID;
        bool isToggle;

        bool pressed = false;
        bool prevState = false;

        double pressTimout = 200; // In Ms
        double lastPress = 0;

        void(*pressCB)();
        void(*releaseCB)();

        void runCB() {
            if (pressed) {
                if (pressCB == nullptr) { return; }
                this->pressCB();
            } else {
                if (releaseCB == nullptr) { return; }
                this->releaseCB();
            }
        }
};
class StateMachine {
    public: 

        StateMachine() {};

        void setDefaultState(int(*cb)()) {
            this->stateStorage[0] = cb;
        }

        void addState(int stateID, int(*cb)()) {
            if (stateID == 0) { return; }
            this->stateStorage[stateID] = cb;
        }

        void iterate() {
            if (!running) { return; }
            currentState = this->stateStorage[currentState]();
        }

        int getState() {
            return this->currentState;
        }

        void start() {
            this->running = true;
            this->currentState = 0;
        }
        void stop() {
            this->running = false;
        }

    private:

        std::unordered_map<int, int(*)()> stateStorage = std::unordered_map<int, int(*)()>();
        int currentState = -1; // -1 for default, other numbers for array

        bool running = false;
};