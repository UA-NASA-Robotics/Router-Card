/*
 * motorHandler.c
 *
 * Created: 4/7/2018 12:42:06 PM
 *  Author: sjc93
 */
#include "../Config.h"
#include "motorHandler.h"
#include "Motor.h"
#include "../LEDs.h"
void ReceiveMotor( CAN_packet *p, unsigned char mob);

void InitMotor(Motor_t* Motor, char address, char statusBit,char mob, int maxRPM, int maxCurrent, LimitSwitch_t limitSwitch, bool brushless)
{

	Motor->ID = address;
	Motor->Status = statusBit;
	Motor->MOB = mob;
	Motor->MAX_RPM = maxRPM;
	Motor->MAX_CURRENT = maxCurrent;
	Motor->LimitSwitch = limitSwitch;
	Motor->isBrushless = brushless;
	//motor status tracking
	MOTORSTATUS = MOTORSTATUS|(1<<statusBit); //assume all motors are innocent until proven guilty
	SDO_packet ClearErrors = { Motor->ID, CLEAR_ERRORS};
	SDO_packet SetMotor;
	if(brushless)
	{
		SetMotor = (SDO_packet) {
			Motor->ID,  SET_MOTOR_BRUSHLESS
		};
	} else {
		SetMotor = (SDO_packet) {
			Motor->ID,  SET_MOTOR_BRUSHED
		};
	}
	SDO_packet SetPoles = { Motor->ID, SET_POLES};
	SDO_packet MotorPolarity = { Motor->ID, MOTOR_POLARITY};
	SDO_packet SetFeedBack;
	if(brushless == true)
	{
		SetFeedBack = (SDO_packet) {
			Motor->ID,  SET_FEEDBACK_ENCODER
		}; //2410 for encoder feedback, 2378 for hall feedback

	} else {
		SetFeedBack = (SDO_packet) {
			Motor->ID,  SET_FEEDBACK_EMF
		}; //274 for emf feedback

	}
	SDO_packet SetSVELFeedBack = {Motor->ID, SET_SVEL_FEEDBACK_ENCODER};
	SDO_packet SetFeedBackResolution = {Motor->ID, SET_FEEDBACK_RESOLUTION};
	SDO_packet CurrentLimitPos = {Motor->ID, CURRENT_LIMIT_POS, Motor->MAX_CURRENT};
	SDO_packet CurrentLimitNeg = {Motor->ID, CURRENT_LIMIT_NEG, Motor->MAX_CURRENT};
	SDO_packet VelocityLimitPos = {Motor->ID, VELOCITY_LIMIT_POS, Motor->MAX_RPM};
	SDO_packet VelocityLimitNeg = {Motor->ID, VELOCITY_LIMIT_NEG, Motor->MAX_RPM};
	SDO_packet PowerEnable = { Motor->ID, POWER_ENABLE};
	SDO_packet PowerDisable = { Motor->ID, POWER_DISABLE};

	//initialize circular buffer for motor
	cb_init(&Motor->Motor_Buffer, motorBuffer[mob], Motor_Buffer_Size, CAN_PACKET_SIZE);
	//prepare RX receiving mob for motor.
	bool ret;
	ret=prepare_rx( Motor->MOB, 0x580 + Motor->ID, RECEIVE_MASK, ReceiveMotor); //all 0s forces comparison
	ASSERT( ret==0);

#ifdef INIT_MOTOR_CONFIGURATIONS
	//wipe errors. This first attempt at communication also verifies the controller is in the system.
	SendandVerify(ClearErrors,  &Motor->Motor_Buffer, Motor->Status);
	//disable controller so some commands actually work (Feed back commands)
	SendandVerify(PowerDisable,  &Motor->Motor_Buffer, Motor->Status);
	//set resolution of encoder (500 count encoder * 4 for how the controllers work)
	SendandVerify(SetFeedBack,  &Motor->Motor_Buffer, Motor->Status);
	SendandVerify(SetSVELFeedBack,  &Motor->Motor_Buffer, Motor->Status);
	SendandVerify(SetFeedBackResolution,  &Motor->Motor_Buffer, Motor->Status);
	//configure motor, number of magnet poles and motor type.
	SendandVerify(SetMotor,  &Motor->Motor_Buffer, Motor->Status);
	SendandVerify(SetPoles,  &Motor->Motor_Buffer, Motor->Status);
	//set limits on current on velocity to protect motor
	_delay_ms(2); //not sure why this is needed, but was having a problem communicating at start up.
	if(limitSwitch.Neg!=0 && limitSwitch.Pos!=0) {
		SetMotorLimit(Motor);
	}
	SendandVerify(CurrentLimitPos,  &Motor->Motor_Buffer, Motor->Status);
	SendandVerify(CurrentLimitNeg,  &Motor->Motor_Buffer, Motor->Status);
	SendandVerify(VelocityLimitPos,  &Motor->Motor_Buffer, Motor->Status);
	SendandVerify(VelocityLimitNeg,  &Motor->Motor_Buffer, Motor->Status);
	//actually critical to get right if you are using encoders
	SendandVerify(MotorPolarity,  &Motor->Motor_Buffer, Motor->Status);
	//re-enable motor controller.
	SendandVerify(PowerEnable,  &Motor->Motor_Buffer, Motor->Status);
#endif

}
void InitMotor_BG75(Motor_t* Motor, char address, char statusBit,char mob, int maxRPM, int maxCurrent, LimitSwitch_t limitSwitch)
{

	Motor->ID = address;
	Motor->Status = statusBit;
	Motor->MOB = mob;
	Motor->MAX_RPM = maxRPM;
	Motor->MAX_CURRENT = maxCurrent;
	Motor->LimitSwitch = limitSwitch;
	//motor status tracking
	MOTORSTATUS = MOTORSTATUS|(1<<statusBit); //assume all motors are innocent until proven guilty
	SDO_packet ClearErrors = { Motor->ID, CLEAR_ERRORS};
	SDO_packet SetMotor = { Motor->ID, SET_MOTOR_BRUSHLESS};
	SDO_packet SetPoles = { Motor->ID, SET_POLES_BG75};
	SDO_packet MotorPolarity = { Motor->ID, MOTOR_POLARITY_bg75};
	SDO_packet SetFeedBack = {Motor->ID, SET_FEEDBACK_ENCODER}; //2410 for encoder feedback, 2378 for hall feedback
	//SDO_packet SetSVELFeedBack = {Motor->ID, SET_SVEL_FEEDBACK_ENCODER};
	SDO_packet SetFeedBackResolution = {Motor->ID, SET_FEEDBACK_RESOLUTION_BG75};
	SDO_packet CurrentLimitPos = {Motor->ID, CURRENT_LIMIT_POS, Motor->MAX_CURRENT};
	SDO_packet CurrentLimitNeg = {Motor->ID, CURRENT_LIMIT_NEG, Motor->MAX_CURRENT};
	SDO_packet VelocityLimitPos = {Motor->ID, VELOCITY_LIMIT_POS, Motor->MAX_RPM};
	SDO_packet VelocityLimitNeg = {Motor->ID, VELOCITY_LIMIT_NEG, Motor->MAX_RPM};
	SDO_packet PowerEnable = { Motor->ID, POWER_ENABLE};
	SDO_packet PowerDisable = { Motor->ID, POWER_DISABLE};

	//initialize circular buffer for left motor
	cb_init(&Motor->Motor_Buffer,motorBuffer[mob], Motor_Buffer_Size, CAN_PACKET_SIZE);
	//prepare RX receiving mob for motor.
	bool ret;
	ret=prepare_rx( Motor->MOB, 0x580 + Motor->ID, RECEIVE_MASK, ReceiveMotor); //all 0s forces comparison
	ASSERT( ret==0);

#ifdef INIT_MOTOR_CONFIGURATIONS
	if(limitSwitch.Neg!=0 && limitSwitch.Pos!=0) {
		SetMotorLimit(Motor);
	}
	//wipe errors. This first attempt at communication also verifies the controller is in the system.
	SendandVerify(ClearErrors,  &Motor->Motor_Buffer, Motor->Status);
	//SendandVerify(PowerDisable,  &Motor->Motor_Buffer, Motor->Status);

	//configure motor, number of magnet poles and motor type.
	SendandVerify(SetMotor,  &Motor->Motor_Buffer, Motor->Status);
	SendandVerify(SetPoles,  &Motor->Motor_Buffer, Motor->Status);
	//set limits on current on velocity to protect motor
	_delay_ms(2); //not sure why this is needed, but was having a problem communicating at start up.
	//disable controller so some commands actually work (Feed back commands)

	SendandVerify(CurrentLimitPos,  &Motor->Motor_Buffer, Motor->Status);
	SendandVerify(CurrentLimitNeg,  &Motor->Motor_Buffer, Motor->Status);
	SendandVerify(VelocityLimitPos,  &Motor->Motor_Buffer, Motor->Status);
	SendandVerify(VelocityLimitNeg,  &Motor->Motor_Buffer, Motor->Status);
	//actually critical to get right if you are using encoders
	SendandVerify(MotorPolarity,  &Motor->Motor_Buffer, Motor->Status);
	//re-enable motor controller.
	//set resolution of encoder (500 count encoder * 4 for how the controllers work)
	SendandVerify(SetFeedBack,  &Motor->Motor_Buffer, Motor->Status);
	//SendandVerify(SetSVELFeedBack,  &Motor->Motor_Buffer, Motor->Status);
	SendandVerify(SetFeedBackResolution,  &Motor->Motor_Buffer, Motor->Status);
	//re-enable motor controller.
	SendandVerify(PowerEnable,  &Motor->Motor_Buffer, Motor->Status);
#endif

}



void clearMotorErrorStatusReg(Motor_t* motor)
{
	//wipe errors. If this is the first attempt at communication it verifies the controller is in the system.

	SDO_packet ClearErrorsR = {motor->ID, 0x3000, 0x00, 0x01};

	SendandVerify(ClearErrorsR, &motor->Motor_Buffer, motor->Status);
}

void ReEstablishComms(Motor_t* motor)
{
	SDO_packet ClearErrors = {motor->ID, 0x3000, 0x00, 0x01};
	SDO_packet SetMotor = {motor->ID, 0x3900, 0x00, 1};
	SDO_packet SetPoles = {motor->ID, 0x3910, 0x00, 8};
	SDO_packet CurrentLimitPos = {motor->ID, 0x3221, 0, 10000}; //10A
	SDO_packet CurrentLimitNeg = {motor->ID, 0x3223, 0, 10000}; //10A
	SDO_packet VelocityLimitPos = {motor->ID, 0x3321, 0x00, MAXRPM};
	SDO_packet VelocityLimitNeg = {motor->ID, 0x3323, 0x00, MAXRPM};
	//SDO_packet MotorPolarity = {BUCKETMOTORID, 0x3911, 0x00, 0x01};
	SDO_packet SetFeedBack = {motor->ID, 0x3350, 0x00, 2410}; //encoder feedback
	SDO_packet SetEncoderResolution = {motor->ID, 0x3962, 0x00, 4096}; //bg75pi has a resolution of 4096 counts/rev
	SDO_packet PowerEnable = {motor->ID, 0x3004, 0x00, 0x01};

	MOTORSTATUS |= (1<<motor->Status);

	SendandVerify(ClearErrors,  &motor->Motor_Buffer, motor->Status);
	SendandVerify(SetMotor,  &motor->Motor_Buffer, motor->Status);
	SendandVerify(SetPoles,  &motor->Motor_Buffer, motor->Status);
	_delay_ms(2); //not sure why this is needed, but was having a problem communicating at start up.
	SendandVerify(CurrentLimitNeg,  &motor->Motor_Buffer, motor->Status);
	SendandVerify(CurrentLimitPos,  &motor->Motor_Buffer, motor->Status);
	SendandVerify(VelocityLimitPos,  &motor->Motor_Buffer, motor->Status);
	SendandVerify(VelocityLimitNeg,  &motor->Motor_Buffer, motor->Status);
	SendandVerify(SetFeedBack,  &motor->Motor_Buffer, motor->Status);
	SendandVerify(SetEncoderResolution,  &motor->Motor_Buffer, motor->Status);
	SendandVerify(PowerEnable,  &motor->Motor_Buffer, motor->Status);
}

void ReceiveMotor( CAN_packet *p, unsigned char mob) // interrupt callback
{
	switch (mob)
	{
	case LEFTMOTOR_MOB:
	{
		cb_push_back(&LeftMotor.Motor_Buffer, p->data);
		break;
	}
	case RIGHTMOTOR_MOB:
	{
		cb_push_back(&RightMotor.Motor_Buffer, p->data);
		break;
	}
	case ARMMOTOR_MOB:
	{
		cb_push_back(&ArmMotor.Motor_Buffer, p->data);
		break;
	}
	case BUCKETMOTOR_MOB:
	{
		cb_push_back(&BucketMotor.Motor_Buffer, p->data);
		break;
	}
	case PLOWMOTOR_MOB:
	{
		cb_push_back(&PlowMotor.Motor_Buffer, p->data);
		break;
	}



	}

}


//*******************************************************
//------------------------Setters------------------------
//*******************************************************
void setMotorControlMode(Motor_t *motor, unsigned char mode)
{
	switch(mode)
	{
	case Velocity:
	{
		SDO_packet ModeVel   = {motor->ID,    0x3003, 0x00, 0x3};
		SDO_packet VEL_Acc = {motor->ID,    0x3380, 0x00, (motor->isBrushless ? ACCEL_CONST: ACCEL_CONST_BRUSH)}; //value should be between 100k and 10k
		SDO_packet VEL_Dec = {motor->ID,    0x3381, 0x00, (motor->isBrushless ? ACCEL_CONST: ACCEL_CONST_BRUSH)};

		SendandVerify(ModeVel,  &motor->Motor_Buffer, motor->Status);
		SendandVerify(VEL_Acc,  &motor->Motor_Buffer, motor->Status);
		SendandVerify(VEL_Dec,  &motor->Motor_Buffer, motor->Status);
		break;
	}
	case Position:
	{
		SDO_packet ModePos = {motor->ID,        0x3003, 0x00, 0x7};
		SDO_packet SetPositionWindow = {motor->ID,    0x373A, 0x00, 1000};
		SDO_packet DesiredVelocity = {motor->ID,    0x3300, 0x0,  0x00};  //Velocity set to zero NEED TO SET THIS!!!!!!
		SDO_packet ResetPosition = {motor->ID,      0x3762, 0x00, 0x00};

		SendandVerify(ModePos,  &motor->Motor_Buffer, motor->Status);
		SendandVerify(DesiredVelocity,  &motor->Motor_Buffer, motor->Status);
		SendandVerify(SetPositionWindow,  &motor->Motor_Buffer, motor->Status);
		SendandVerify(ResetPosition,  &motor->Motor_Buffer, motor->Status);
		break;
	}
	}
}
//void setMotorPos(Motor_t *motor, int pos)
//{
//
//
//}
void setMotorVel(Motor_t* motor, int Vel)
{
	if (Vel > MAXRPM)
	{
		Vel = MAXRPM;
	}
	if(Vel < -MAXRPM)
	{
		Vel = -MAXRPM;
	}

	SDO_packet DesiredVelocity = {motor->ID, 0x3300, 0x0, Vel};

	SendandVerify(DesiredVelocity,  &motor->Motor_Buffer, motor->Status);
}
void setMotorVelNoSafetyComms(Motor_t *motor, int Vel)
{
	if (Vel > MAXRPM)
	{
		Vel = MAXRPM;
	}
	if(Vel < -MAXRPM)
	{
		Vel = -MAXRPM;
	}
	char temp[8];
	SDO_packet Continue = {motor->ID, 0x3000, 0x00, 4};
	SDO_packet DesiredVelocity = {motor->ID, 0x3300, 0x0, Vel};
	while(cb_size(&motor->Motor_Buffer)>0)
	{
		cb_pop_front(&motor->Motor_Buffer, temp);
	}
	//write data to motor
	while(!SDOWritePacket(Continue));
	while(!SDOWritePacket(DesiredVelocity));
}
void setMotorCurNoSafetyComms(Motor_t *motor, int Cur)
{
	setMotorVelNoSafetyComms(motor, Cur);
//  if(abs(Cur) <  3000 && abs(Cur) >  30)
//    Cur = (Cur/abs(Cur))*3000;
	if (Cur > MAXCURRENTARM)
	{
		Cur = MAXCURRENTARM;
	}
	if(Cur < -MAXCURRENTARM)
	{
		Cur = -MAXCURRENTARM;
	}
	char temp[8];
	SDO_packet Continue = {motor->ID, 0x3000, 0x00, 4};
	SDO_packet DesiredVelocity = {motor->ID, 0x3200, 0x0, Cur};
	while(cb_size(&motor->Motor_Buffer)>0)
	{
		cb_pop_front(&motor->Motor_Buffer, temp);
	}
	//write data to motor
	while(!SDOWritePacket(Continue));
	while(!SDOWritePacket(DesiredVelocity));
}
void setMotorCounts(Motor_t* motor, long counts)
{
	SDO_packet Move = {motor->ID, 0x3791, 0x00, counts};
	SendandVerify(Move,  &motor->Motor_Buffer, motor->Status);
}
void setMotorCountsNoSafetyComms(Motor_t *motor, long counts)
{
	SDO_packet Move = {motor->ID, 0x3791, 0x00, counts};
	char temp[8];
	while(cb_size(&motor->Motor_Buffer)>0)
	{
		cb_pop_front(&motor->Motor_Buffer, temp);
	}
	//write data to motor
	while(!SDOWritePacket(Move));
}
void SetMotorLimit(Motor_t *motor)
{
	SDO_packet PositiveLimit = {motor->ID,POSITIVE_LIMIT_SWITCH, motor->LimitSwitch.Pos}; //activates digital input 0 as positive limit switch high active
	SDO_packet NegativeLimit = {motor->ID, NEGATIVE_LIMIT_SWITCH, motor->LimitSwitch.Neg}; //activates digital input 1 as negative limit switch high active

	SDO_packet HomeLimit = {motor->ID, 0x3056, 0x00, 0x132}; //activates digital input 2 as home limit switch high active
	//SDO_packet Home_Method = {BUCKETMOTORID, 0x37B2, 0x00, 2}; //homes by turning CW to positive limit, should be what we want.

	SendandVerify(PositiveLimit,  &motor->Motor_Buffer, motor->Status);
	SendandVerify(NegativeLimit,  &motor->Motor_Buffer, motor->Status);
	if(motor->LimitSwitch.Home != 0)
		SendandVerify(HomeLimit,  &motor->Motor_Buffer, motor->Status);
	//SendandVerify(Home_Method,  &BucketMotor_Buffer, BUCKETMOTOR_STATUS);
}

//*********************************************************************************
//------------------------------------<Getters>------------------------------------
//*********************************************************************************
long getMotorPos(Motor_t *motor)
{
	char temp[8];
	SDO_packet ReadPosition = {motor->ID, 0x396A, 0x00, 0x00}; // {RIGHTMOTORID, 0x396A, 0x00, 0x00};

	if(ReadandVerify(ReadPosition, &motor->Motor_Buffer, motor->Status, temp))
	{
		long result = ArrayToLong(temp);
		return (result/4); //returns encoder counts
	}
	else
	{
		return 0;
	}
}
char getMotorVoltage(Motor_t *motor)
{
	char temp[8];
	SDO_packet ReadVoltage = {motor->ID, MOTOR_VOLTAGE};

	if(ReadandVerify(ReadVoltage, &motor->Motor_Buffer, motor->Status, temp))
	{
		long result = ArrayToLong(temp);
		return (result/1000 +1); //get voltage in mV from controller, add 1v to compensate for suspected diode drop to input.
	}
	else
	{
		return 0;
	}
}
char getMotorTemp(Motor_t *motor)
{
	char temp[8];
	SDO_packet ReadTemperature = {motor->ID, MOTOR_TEMPERATURE};

	if(ReadandVerify(ReadTemperature, &motor->Motor_Buffer, motor->Status, temp))
	{
		long result = ArrayToLong(temp);
		return result/10;
	}
	else
	{
		return 0;
	}
}
bool getMotorPosReached(Motor_t *motor)
{
	char temp[8];
	SDO_packet ReadPositionReached = {motor->ID, 0x3002, 0x00, 0x00}; // {RIGHTMOTORID, 0x396A, 0x00, 0x00};

	if(ReadandVerify(ReadPositionReached, &motor->Motor_Buffer, motor->Status, temp))
	{
		//long result = ArrayToLong(temp);
		return (temp[4]&0x10); //returns position Reached
	}
	else
	{
		return 0;
	}
}
//bool getMotorStatus(Motor_t *motor)
//{
//
//}


char getMotorDigital(Motor_t *motor)
{
	char temp[8];
	SDO_packet ReadPort0 = {motor->ID, 0x3120, 0x00, 0x00};

	ReadandVerify(ReadPort0, &motor->Motor_Buffer, motor->Status, temp);
	long result = ArrayToLong(temp);
	return result;
}
