#include "main.h"


/**
 * Catapult: 5
 * Front Arm: 10
 */


// Global Variables
auton::AutonSystem autonSystem = auton::AutonSystem();

RotationSensor leftEncoder(17);
RotationSensor rightEncoder(18, true);

pros::Controller masterController = pros::Controller(pros::E_CONTROLLER_MASTER);
pros::Motor leftArm_Moter = pros::Motor(-8);
pros::Motor rightArm_Moter = pros::Motor(10);

pros::Motor cata_Motor = pros::Motor(-5);


std::shared_ptr<OdomChassisController> chassis = ChassisControllerBuilder()
    .withMotors({-6, -3}, {9, 2})
    // green gearset, 4 inch wheel diameter, 11.5 inch wheel track
    .withDimensions(AbstractMotor::gearset::green, {{3.25_in, 9_in}, imev5GreenTPR * (36.0 / 60.0)})
    .withSensors(leftEncoder, rightEncoder)
    // specify the tracking wheels diameter (2.75 in), track (7 in), and TPR (360)
    .withOdometry({{3.50_in, 4_in}, 360}, StateMode::CARTESIAN)
    .buildOdometry();

std::shared_ptr<AsyncMotionProfileController> profileController = AsyncMotionProfileControllerBuilder()
    .withLimits({
      0.5, // Maximum linear velocity of the Chassis in m/s
      1.0, // Maximum linear acceleration of the Chassis in m/s/s
      5.0 // Maximum linear jerk of the Chassis in m/s/s/s
    })
    .withOutput(chassis)
    .buildMotionProfileController();


void positionPrint() {
	while (true) {
		pros::delay(500);
		DEBUGLOG("POS: ", chassis->getOdometry()->getState().str(1_tile, 1_deg))
	}
}

void screenPressed() { autonSystem.screenPressed(); }
void renderScreen() {
	while (true) {
		autonSystem.renderScreen();
		pros::delay(100);
	}
}
void initialize() {
	chassis->setDefaultStateMode(okapi::StateMode::CARTESIAN);

	pros::Task positionPrintTask(positionPrint);

	pros::screen::touch_callback(screenPressed, pros::last_touch_e_t::E_TOUCH_PRESSED);
	autonSystem.initialize();
	pros::Task renderScreenTask(renderScreen);


	leftArm_Moter.move_voltage(-10000);
	rightArm_Moter.move_voltage(-10000);

	pros::delay(800);

	leftArm_Moter.move_voltage(0);
	rightArm_Moter.move_voltage(0);

	leftArm_Moter.set_zero_position(0);
	rightArm_Moter.set_zero_position(0);


	pros::Task controlLoop(controllerLoop);
}

void disabled() { autonSystem.disabled(); }
void competition_initialize() { autonSystem.autonInitialize(); }
void autonomous() { autonSystem.started(); }

__attribute__((noreturn)) void opcontrol() {

	while (true) {

		if (autonSystem.isTank()) {
			chassis->getModel()->tank(
				(double)masterController.get_analog(ANALOG_LEFT_Y) / 128.0,
				(double)masterController.get_analog(ANALOG_RIGHT_Y) / 128.0
			);
		} else {
			// Arcade Drive
			chassis->getModel()->arcade(
				(double)masterController.get_analog(ANALOG_LEFT_Y) / 128.0,
				(double)masterController.get_analog(ANALOG_RIGHT_X) / 128.0
			);
		}

		pros::delay(20);
	}
}
