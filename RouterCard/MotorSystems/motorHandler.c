/*
 * motorHandler.c
 *
 * Created: 4/7/2018 12:42:06 PM
 *  Author: sjc93
 */ 

#include "motorHandler.h"




void InitMotor(Motor_t* Motor, char address, char statusBit,char mob, int maxRPM, int maxCurrent)
{
	
	Motor->ID = address;
	Motor->Status = statusBit;
	Motor->MOB = mob;
	Motor->MAX_RPM = maxRPM;
	Motor->MAX_CURRENT = maxCurrent;
	//motor status tracking
    MOTORSTATUS = (1<<statusBit); //assume all motors are innocent until proven guilty
	SDO_packet ClearErrors = { Motor->ID, CLEAR_ERRORS};
	SDO_packet SetMotor = { Motor->ID, SET_MOTOR};
	SDO_packet SetPoles = { Motor->ID, SET_POLES};
	SDO_packet MotorPolarity = { Motor->ID, MOTOR_POLARITY};
	SDO_packet SetFeedBack = {Motor->ID, SET_FEEDBACK_ENCODER}; //2410 for encoder feedback, 2378 for hall feedback
	SDO_packet SetSVELFeedBack = {Motor->ID, SET_FEEDBACK_ENCODER};
	SDO_packet SetFeedBackResolution = {Motor->ID, 0x3962, 0x00, 500};
	SDO_packet CurrentLimitPos = {Motor->ID, CURRENT_LIMIT_POS};
	SDO_packet CurrentLimitNeg = {Motor->ID, CURRENT_LIMIT_NEG};
	SDO_packet VelocityLimitPos = {Motor->ID, VELOCITY_LIMIT_POS};
	SDO_packet VelocityLimitNeg = {Motor->ID, VELOCITY_LIMIT_NEG};
	SDO_packet PowerEnable = { Motor->ID, POWER_ENABLE};
	SDO_packet PowerDisable = { Motor->ID, POWER_DISABLE};
	//initialize circular buffer for left motor
	cb_init(&LeftMotor_Buffer, LeftMotor_Buffer_Size, CAN_PACKET_SIZE);
	//prepare RX receiving mob for motor.
	BOOL ret;
	ret=prepare_rx( LEFTMOTOR_MOB, 0x580 + Motor->ID, RECEIVE_MASK, ReceiveMotor3); //all 0s forces comparison
	ASSERT( ret==0);

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
	SendandVerify(CurrentLimitPos,  &Motor->Motor_Buffer, Motor->Status);
	SendandVerify(CurrentLimitNeg,  &Motor->Motor_Buffer, Motor->Status);
	SendandVerify(VelocityLimitPos,  &Motor->Motor_Buffer, Motor->Status);
	SendandVerify(VelocityLimitNeg,  &Motor->Motor_Buffer, Motor->Status);
	//actually critical to get right if you are using encoders
	SendandVerify(MotorPolarity,  &Motor->Motor_Buffer, Motor->Status);
	//re-enable motor controller.
	SendandVerify(PowerEnable,  &Motor->Motor_Buffer, Motor->Status);
}



void clearMotorErrorStatusReg(Motor_t* motor)
{
	//wipe errors. If this is the first attempt at communication it verifies the controller is in the system.
	
	SDO_packet ClearErrorsR	= {motor->ID,	0x3000, 0x00, 0x01};
	
	SendandVerify(ClearErrorsR,	&RightMotor_Buffer, RIGHTMOTOR_STATUS);
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

	SendandVerify(ClearErrors,  &BucketMotor_Buffer, motor->Status);
	SendandVerify(SetMotor,  &BucketMotor_Buffer, motor->Status);
	SendandVerify(SetPoles,  &BucketMotor_Buffer, motor->Status);
	_delay_ms(2); //not sure why this is needed, but was having a problem communicating at start up.
	SendandVerify(CurrentLimitNeg,  &BucketMotor_Buffer, motor->Status);
	SendandVerify(CurrentLimitPos,  &BucketMotor_Buffer, motor->Status);
	SendandVerify(VelocityLimitPos,  &BucketMotor_Buffer, motor->Status);
	SendandVerify(VelocityLimitNeg,  &BucketMotor_Buffer, motor->Status);
	SendandVerify(SetFeedBack,  &BucketMotor_Buffer, motor->Status);
	SendandVerify(SetEncoderResolution,  &BucketMotor_Buffer, motor->Status);
	SendandVerify(PowerEnable,  &BucketMotor_Buffer, motor->Status);
}

void setMotorControlMode(Motor_t *motor, unsigned char mode)
{
	switch(mode)
	{
		case Velocity:
		{
			SDO_packet ModeVel	 = {motor->ID, 		0x3003, 0x00, 0x3};
			SDO_packet VEL_Acc = {motor->ID, 		0x3380, 0x00, ACCEL_CONST}; //value should be between 100k and 10k
			SDO_packet VEL_Dec = {motor->ID, 		0x3381, 0x00, ACCEL_CONST};
			
			SendandVerify(ModeVel,  &motor->Motor_Buffer, BUCKETMOTOR_STATUS);
			SendandVerify(VEL_Acc,  &motor->Motor_Buffer, BUCKETMOTOR_STATUS);
			SendandVerify(VEL_Dec,  &motor->Motor_Buffer, BUCKETMOTOR_STATUS);
			break;
		}
		case Position:
		{
			SDO_packet ModePos = {motor->ID,				0x3003, 0x00, 0x7};
			SDO_packet SetPositionWindow = {motor->ID,		0x373A, 0x00, 1000};
			SDO_packet DesiredVelocity = {motor->ID,		0x3300, 0x0,  0x00};	//Velocity set to zero NEED TO SET THIS!!!!!!
			SDO_packet ResetPosition = {motor->ID,			0x3762, 0x00, 0x00};
			
			SendandVerify(ModePos,  &motor->Motor_Buffer, BUCKETMOTOR_STATUS);
			SendandVerify(DesiredVelocity,  &motor->Motor_Buffer, BUCKETMOTOR_STATUS);
			SendandVerify(SetPositionWindow,  &motor->Motor_Buffer, BUCKETMOTOR_STATUS);
			SendandVerify(ResetPosition,  &motor->Motor_Buffer, BUCKETMOTOR_STATUS);
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
	SDO_packet DesiredVelocity = {motor->ID, 0x3300, 0x0, Vel};
	while(cb_size(&motor->Motor_Buffer)>0)
	{
		cb_pop_front(&motor->Motor_Buffer, temp);
	}
	//write data to motor
	while(!SDOWritePacket(DesiredVelocity));
}

void setMotorCounts(Motor_t* motor, long counts)
{
	SDO_packet Move = {motor->ID, 0x3791, 0x00, counts};
	SendandVerify(Move,  &BucketMotor_Buffer, BUCKETMOTOR_STATUS);
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
	SDO_packet PositiveLimit = {motor->ID, POSITIVE_LIMIT_SWITCH_D0H}; //activates digital input 0 as positive limit switch high active
	SDO_packet NegativeLimit = {motor->ID, NEGATIVE_LIMIT_SWITCH_D0H}; //activates digital input 1 as negative limit switch high active
	SDO_packet HomeLimit = {motor->ID, 0x3056, 0x00, 0x132}; //activates digital input 2 as home limit switch high active
	//SDO_packet Home_Method = {BUCKETMOTORID, 0x37B2, 0x00, 2}; //homes by turning CW to positive limit, should be what we want.
	
	SendandVerify(PositiveLimit,  &BucketMotor_Buffer, motor->Status);
	SendandVerify(NegativeLimit,  &BucketMotor_Buffer, motor->Status);
	SendandVerify(HomeLimit,  &BucketMotor_Buffer, motor->Status);
	//SendandVerify(Home_Method,  &BucketMotor_Buffer, BUCKETMOTOR_STATUS);
}




long getMotorPos(Motor_t *motor)
{
	char temp[8];
	SDO_packet ReadPosition ={motor->ID, 0x396A, 0x00, 0x00};// {RIGHTMOTORID, 0x396A, 0x00, 0x00};

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
	SDO_packet ReadPositionReached ={motor->ID, 0x3002, 0x00, 0x00};// {RIGHTMOTORID, 0x396A, 0x00, 0x00};

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
