#include "main.h"


// Global Variables
const RotationSensor leftEncoder(11);
const RotationSensor rightEncoder(12);

auton::mode autonMode = auton::mode::none;
bool autonHasRun = false;

const std::shared_ptr<OdomChassisController> chassis = ChassisControllerBuilder()
    .withMotors({1, 2}, {3, 4})
    // green gearset, 4 inch wheel diameter, 11.5 inch wheel track
    .withDimensions(AbstractMotor::gearset::green, {{4_in, 11.5_in}, imev5GreenTPR})
    .withSensors(leftEncoder, rightEncoder)
    // specify the tracking wheels diameter (2.75 in), track (7 in), and TPR (360)
    .withOdometry({{3.75_in, 7_in}, quadEncoderTPR}, StateMode::CARTESIAN)
    .buildOdometry();

const std::shared_ptr<AsyncMotionProfileController> profileController = AsyncMotionProfileControllerBuilder()
    .withLimits({
      1.0, // Maximum linear velocity of the Chassis in m/s
      2.0, // Maximum linear acceleration of the Chassis in m/s/s
      10.0 // Maximum linear jerk of the Chassis in m/s/s/s
    })
    .withOutput(chassis)
    .buildMotionProfileController();

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void leftBtnPressed() { autonMode = auton::mode::left; }
void skillsBtnPressed() { autonMode = auton::mode::skills; }
void rightBtnPressed() { autonMode = auton::mode::right; }

void renderScreen() {
	pros::screen::erase();
}


void screenPressed() {
	auto status =  pros::screen::touch_status();
	int xPos = status.x;
	int yPos = status.y;

	// Check buttons

	renderScreen();
}

void initialize() {
	pros::screen::touch_callback(screenPressed, pros::last_touch_e_t::E_TOUCH_PRESSED);

	renderScreen();
}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {}

/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize() {}

/**
 * Runs the user autonomous code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the autonomous
 * mode. Alternatively, this function may be called in initialize or opcontrol
 * for non-competition testing purposes.
 *
 * If the robot is disabled or communications is lost, the autonomous task
 * will be stopped. Re-enabling the robot will restart the task, not re-start it
 * from where it left off.
 */
void autonomous() {
	if (autonHasRun) { return; }

	autonHasRun = true;
	
	switch (autonMode) {
		case auton::mode::left:
			profileController->generatePath({
					{0_ft, 0_ft, 0_deg}, 
					{3_ft, 0_ft, 0_deg},
					{3_ft, 3_ft, 0_deg}
				},
				"A" // Profile name
			);
			profileController->setTarget("A");
			break;

		case auton::mode::right:
			profileController->generatePath({
					{0_ft, 0_ft, 0_deg}, 
					{3_ft, 0_ft, 0_deg},
					{3_ft, 3_ft, 0_deg}
				},
				"A" // Profile name
			);
			profileController->setTarget("A");
			break;

		case auton::mode::skills:
			profileController->generatePath({
					{0_ft, 0_ft, 0_deg}, 
				},
				"A" // Profile name
			);
			profileController->setTarget("A");
			break;
	
	default:
		break;
	}

	profileController->waitUntilSettled();
}

/**
 * Runs the operator control code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the operator
 * control mode.
 *
 * If no competition control is connected, this function will run immediately
 * following initialize().
 *
 * If the robot is disabled or communications is lost, the
 * operator control task will be stopped. Re-enabling the robot will restart the
 * task, not resume it from where it left off.
 */

void opcontrol() {
	pros::Controller master(pros::E_CONTROLLER_MASTER);

	while (true) {

		chassis->getModel()->tank(
			master.get_analog(ANALOG_LEFT_Y),
			master.get_analog(ANALOG_RIGHT_Y)
		);

		pros::delay(20);
	}
}
