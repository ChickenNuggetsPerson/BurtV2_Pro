#pragma once
#include <unordered_map>
#include <string>
#include "stateMachine.h"


namespace auton {

    enum AutonMode {
        nothing, left, right, skill
    };

    class AutonSystem {
        using Callback = std::function<int()>;
        public:
            AutonSystem() {};

            void initialize();
            void autonInitialize();
            void started();
            void disabled();
            void screenPressed();

            void renderScreen();

            inline void addMode(int modeId, std::string modeName, Callback const& cb) { 
                this->stateMachine.addState(modeId, cb); 
                this->stateNames[modeId] = modeName;
            }

            inline bool isTank() const { return isTankDrive; }


        private:
            int mode = nothing;
            StateMachine stateMachine = StateMachine();      
            std::unordered_map<int, std::string> stateNames = std::unordered_map<int, std::string>();
            
            bool updateScreen = true;
            int initialRenders = 50;

            bool isTankDrive = true;
    };

};