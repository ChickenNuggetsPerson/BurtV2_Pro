#pragma once

#include <unordered_map>
#include "api.h"


// The ButtonSystem Class is used for setting callbacks to buttons on the controller
// This is used since PROS does not have a callback system for its controller
class ButtonSystem {
    using Callback = std::function<void()>;
    public:
        /**
         * Initialized the button system
         * 
         * \param conPtr A pointer to the PROS controller.
         * \param btn The button to listen to.
         * \param isToggle Set the button mode. True = Toggle Mode, False = Press Mode.
         * \param pressCB The callback to be called when the state changes to true. Set to nullptr if you want nothing to happen
         * \param releaseCB The callback to be called when the state changes to false. Set to nullptr if you want nothing to happen
         * \param pressTimout Optional. Press timout is the minimum time in miliseconds between button state changes. It help reduces "ghost" button presses.
         * 
         */
        ButtonSystem(
            pros::Controller* conPtr,
            pros::controller_digital_e_t btn,
            bool isToggle,
            Callback const& pressCB = nullptr,
            Callback const& releaseCB = nullptr,
            double pressTimout = 200 
        ) : controllerPtr(conPtr), buttonID(btn), isToggle(isToggle), pressCB(pressCB), releaseCB(releaseCB), pressTimout(pressTimout) {}; 
        
        // This function needs to be ran continually to keep checking for button presses
        // Run this in a while(true) loop
        void check() {
            bool currentState = controllerPtr->get_digital(buttonID);
            
            if (pros::c::millis() < lastPress + pressTimout) { return; } 

            if (isToggle) { // Is toggle mode

                if (currentState && !prevState) {
                    // Pressed
                    pressed = !pressed;
                    runCB();
                }
                if (!currentState && prevState) {
                    // Released
                } 
                
            } else { // Is press mode

                if (currentState && !prevState) {
                    // Pressed
                    pressed = true;
                    runCB();
                }
                if (!currentState && prevState) {
                    // Released
                    pressed = false;
                    runCB();
                }
                
            }

            if (prevState != currentState) {
                lastPress = pros::c::millis();
            }
            prevState = currentState;
        }

        // Gets the pressed status of the button
        inline bool getStatus() const  { return pressed; }

    private:
        pros::Controller* controllerPtr;
        pros::controller_digital_e_t buttonID;
        bool isToggle;

        bool pressed = false;
        bool prevState = false;

        Callback pressCB;
        Callback releaseCB;

        double pressTimout; // In Ms
        double lastPress = 0;


        // Runs the specified callback based on the state of the system
        void runCB() const {
            if (pressed) {
                if (pressCB == nullptr) { return; }
                this->pressCB();
            } else {
                if (releaseCB == nullptr) { return; }
                this->releaseCB();
            }
        }
};


// A state machine where state ids are defined by integers.
// It is recomended to be used along with enums to help make reading states easier
class StateMachine {
    using Callback = std::function<int()>;
    public: 

        StateMachine() = default;

        /**
         * Adds a state to the state machine
         * 
         * \param stateID The integer id of the state
         * \param cb The callback to the state
         * 
         */
        void addState(int stateID, Callback const& cb) {
            this->stateStorage[stateID] = cb;
        }

        // Iterates the machine
        // Run this in a while(true) loop to have the machine continually run
        void iterate() {
            if (!running || currentState == -1) { return; }
            // Run the state and store the result to currentState variable
            currentState = this->stateStorage[currentState](); 
        }

        // Returns the current state
        int getState() const {
            return this->currentState;
        }

        // Prepares the state machine along with sets the starting state
        void start(int startState) {
            this->running = true;
            this->currentState = startState;
        }

        // Stops the state machine
        void stop() {
            this->running = false;
        }

    private:

        std::unordered_map<int, Callback> stateStorage = std::unordered_map<int, Callback>();
        int currentState = -1; // -1 for default, other numbers for array
        bool running = false;
};