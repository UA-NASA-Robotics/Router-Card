
//Allowing for inclusion and exclusion of subfunctionality of system with defines

#ifndef CONFIG_H
#define CONFIG_H


#define NEW_MAIN

//THE GREATEST OH SHIT WE ARE SCREWED ATTEMPT AT RECOVERY, AUTONOMY MAKES THE ROBOT RESET
//#define MACROS_RESET_ROUTER

#define DISABLE_CAN_FORWARDING
#define DISABLE_CAN_FORWARDING_RECEIVE
#define DISABLE_CAN_FORWARDING_RECEIVEISR

//#define INIT_MOTOR_CONFIGURATIONS

//#define DISABLE_MOTOR_SYSTEMS

//#define DISABLE_LEFT_MOTOR
//#define DISABLE_RIGHT_MOTOR
// #define DISABLE_ARM_MOTOR
// #define DISABLE_BUCKET_MOTOR
#define DISABLE_PLOW_MOTOR

//This makes the tread motors operate using the CAN enabled motor controllers
//If this is commented out then the motor commands are sent to the bumper boards
#define MOTORS_USING_CAN

//USE CAN CONTROLLER FOR LEFT MOTOR
#define LEFT_MOTOR_USING_CAN

//USE CAN CONTROLLER FOR RIGHT MOTOR
#define RIGHT_MOTOR_USING_CAN

//Reversing the left and right motors using CAN
#define REVERSE_LEFT_RIGHT

//This makes us send packets to the control box when we are in comms safety (when we havent heard back)
// IT will probably cost communications bandwidth so for now its disabled
#define SEND_TO_CONTROL_FOR_COMMS_ESTABLISH


#define USE_BUTTONS_TEST


#endif