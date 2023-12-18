#include "main.h"


/**
 * Catapult: 5
 * Front Arm: 10
 */


// Global Variables
const RotationSensor leftEncoder(17);
const RotationSensor rightEncoder(18);

pros::Controller masterController = pros::Controller(pros::E_CONTROLLER_MASTER);
pros::Motor leftArm_Moter = pros::Motor(-8);
pros::Motor rightArm_Moter = pros::Motor(10);


const std::shared_ptr<OdomChassisController> chassis = ChassisControllerBuilder()
    .withMotors({-6, -3}, {9, 2})
    // green gearset, 4 inch wheel diameter, 11.5 inch wheel track
    .withDimensions(AbstractMotor::gearset::green, {{3.25_in, 9_in}, imev5GreenTPR * (36.0 / 60.0)})
    .withSensors(leftEncoder, rightEncoder)
    // specify the tracking wheels diameter (2.75 in), track (7 in), and TPR (360)
    .withOdometry({{3.50_in, 4_in}, 360}, StateMode::CARTESIAN)
    .buildOdometry();

const std::shared_ptr<AsyncMotionProfileController> profileController = AsyncMotionProfileControllerBuilder()
    .withLimits({
      0.5, // Maximum linear velocity of the Chassis in m/s
      1.0, // Maximum linear acceleration of the Chassis in m/s/s
      5.0 // Maximum linear jerk of the Chassis in m/s/s/s
    })
    .withOutput(chassis)
    .buildMotionProfileController();


/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
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
	pros::Task controlLoop(controllerLoop);

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

	std::shared_ptr<AsyncPositionController<double, double>> frontArm = AsyncPosControllerBuilder().withMotor(10).build();
	frontArm->setTarget(-1000);
	frontArm->waitUntilSettled();

	chassis->setState({0_in, 0_in, 0_deg});



	// Right Side AUTON
	chassis->driveToPoint({0_tile, 1_tile});
	chassis->waitUntilSettled();
	chassis->driveToPoint({1_tile, 0_tile});
	chassis->waitUntilSettled();
	chassis->turnToAngle(0_deg);
	chassis->waitUntilSettled();
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

	while (true) {

		chassis->getModel()->tank(
			(double)masterController.get_analog(ANALOG_LEFT_Y) / 128.0,
			(double)masterController.get_analog(ANALOG_RIGHT_Y) / 128.0
		);

		pros::delay(20);
	}
}
