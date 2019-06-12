/*
 * Environment.c
 *
 *  Created on: May 20, 2019
 *      Author: roc5
 */
#include <stdint.h>
#include "Environment.h"


// ---------- Helper functions ----------
//Values for the accelerometer
int value0 = 0;
int value1 = 0;
//int acceleroValue = 0;

//Values for the data averaging
int cptAverage = 0; // Do not touch used by average function
int averageArray[NB_AVERAGE] = {0};
int iteration = 0;

int getIntAverage(int* tab, int size)
{
	int output = 0;
	for(int i = 0; i < size; i++)
	{
		output += tab[i];
	}
	output /= size;
	return output;
}

int getState(EnvironmentConfiguration *env)
{
	//value0 = 0;
	//value1 = 0;

	int realValue = 0;

	//for(int i = 0; i < NB_AVERAGE; i++){
		uint8_t rawValue0 = SingleByteRead(env->i2C_Handle, ADXL345_DATAY0_REG);
		uint8_t rawValue1 = SingleByteRead(env->i2C_Handle, ADXL345_DATAY1_REG);
		realValue += (((int)rawValue1) << 8) | rawValue0;
		//value0 += rawValue0;
		//value1 += rawValue1;
	//}

	//printf("RealValue: %d\n", realValue);
	//realValue = realValue/NB_AVERAGE;

	//value0 /= NB_AVERAGE;
	//value1 /= NB_AVERAGE;

	int acceleroValue = 0;
	if(realValue < 32000){
		acceleroValue = realValue;
		//printf("+ : Value %d\n",acceleroValue);
	} else if(realValue >= 32000){
		acceleroValue = realValue - 65535;
		//printf("- : Value %d\n",acceleroValue);
	} else {
		printf("I: Value %d , %d\n",value1,value0);
	}

	//Set edges
	if(acceleroValue < ACCELERO_VALUE_MIN){
		acceleroValue = ACCELERO_VALUE_MIN;
	}
	if(acceleroValue > ACCELERO_VALUE_MAX){
		acceleroValue = ACCELERO_VALUE_MAX;
	}

	int range = ACCELERO_VALUE_MAX - ACCELERO_VALUE_MIN; //Range of data
	int step = range/NB_STATE;

	//floor instead of round avoid making bad array addresses
	int state = (int)roundf((float)(acceleroValue + abs(ACCELERO_VALUE_MIN))/(float)step);

	if(state == NB_STATE){
		state -= 1;
	}

	printf("State: %d\n",state);
	//acceleroValue = acceleroValue/(NB_STATE/2);

	return state;
}

void stepperForward()
{
	//for (int i=0; i<512*nb_tour; i++)   // for 360 deg rotation

	HAL_GPIO_WritePin (GPIOB, GPIO_PIN_15, GPIO_PIN_SET);   // IN1 HIGH
	HAL_GPIO_WritePin (GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
	HAL_GPIO_WritePin (GPIOA, GPIO_PIN_12, GPIO_PIN_RESET);
	HAL_GPIO_WritePin (GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);

	HAL_Delay (DELAY_BETWEEN_PHASES);

	HAL_GPIO_WritePin (GPIOB, GPIO_PIN_15, GPIO_PIN_RESET);
	HAL_GPIO_WritePin (GPIOB, GPIO_PIN_14, GPIO_PIN_SET);   // IN2 HIGH
	HAL_GPIO_WritePin (GPIOA, GPIO_PIN_12, GPIO_PIN_RESET);
	HAL_GPIO_WritePin (GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);

	HAL_Delay (DELAY_BETWEEN_PHASES);

	HAL_GPIO_WritePin (GPIOB, GPIO_PIN_15, GPIO_PIN_RESET);
	HAL_GPIO_WritePin (GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
	HAL_GPIO_WritePin (GPIOA, GPIO_PIN_12, GPIO_PIN_SET);   // IN3 HIGH
	HAL_GPIO_WritePin (GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);

	HAL_Delay (DELAY_BETWEEN_PHASES);

	HAL_GPIO_WritePin (GPIOB, GPIO_PIN_15, GPIO_PIN_RESET);
	HAL_GPIO_WritePin (GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
	HAL_GPIO_WritePin (GPIOA, GPIO_PIN_12, GPIO_PIN_RESET);
	HAL_GPIO_WritePin (GPIOA, GPIO_PIN_11, GPIO_PIN_SET);    // IN4 HIGH
	HAL_Delay (DELAY_BETWEEN_PHASES);
}

void stepperBackward()
{
	//for (int i=0; i<512*nb_tour; i++)   // for 360 deg rotation

	HAL_GPIO_WritePin (GPIOB, GPIO_PIN_15, GPIO_PIN_RESET);
	HAL_GPIO_WritePin (GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
	HAL_GPIO_WritePin (GPIOA, GPIO_PIN_12, GPIO_PIN_RESET);
	HAL_GPIO_WritePin (GPIOA, GPIO_PIN_11, GPIO_PIN_SET);    // IN4 HIGH
	HAL_Delay (DELAY_BETWEEN_PHASES);

	HAL_GPIO_WritePin (GPIOB, GPIO_PIN_15, GPIO_PIN_RESET);
	HAL_GPIO_WritePin (GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
	HAL_GPIO_WritePin (GPIOA, GPIO_PIN_12, GPIO_PIN_SET);   // IN3 HIGH
	HAL_GPIO_WritePin (GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);
	HAL_Delay (DELAY_BETWEEN_PHASES);

	HAL_GPIO_WritePin (GPIOB, GPIO_PIN_15, GPIO_PIN_RESET);
	HAL_GPIO_WritePin (GPIOB, GPIO_PIN_14, GPIO_PIN_SET);   // IN2 HIGH
	HAL_GPIO_WritePin (GPIOA, GPIO_PIN_12, GPIO_PIN_RESET);
	HAL_GPIO_WritePin (GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);
	HAL_Delay (DELAY_BETWEEN_PHASES);


	HAL_GPIO_WritePin (GPIOB, GPIO_PIN_15, GPIO_PIN_SET);   // IN1 HIGH
	HAL_GPIO_WritePin (GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
	HAL_GPIO_WritePin (GPIOA, GPIO_PIN_12, GPIO_PIN_RESET);
	HAL_GPIO_WritePin (GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);
	HAL_Delay (DELAY_BETWEEN_PHASES);
}

// ---------- Environment functions ----------

EnvironmentConfiguration* EnvironmentInit(I2C_HandleTypeDef *accelero)
{
	EnvironmentConfiguration *env = (EnvironmentConfiguration*)malloc(sizeof(EnvironmentConfiguration));
	env->i2C_Handle = accelero;
	env->observation_space = NB_STATE;
	env->action_space = 7;
	return env;
}

EnvReturn EnvMakeAction(EnvironmentConfiguration *env, int action)
{
	if(action == 0)
	{
		stepperForward();
		stepperForward();
	}
	else if(action == 1)
	{
		stepperBackward();
		stepperBackward();
	}
	else if(action == 2){
		stepperForward();
		stepperForward();
		stepperForward();
		stepperForward();
	}
	else if(action == 3){
		stepperBackward();
		stepperBackward();
		stepperBackward();
		stepperBackward();
	}
	else if(action == 4){
		//Just do nothing
		HAL_GPIO_WritePin (GPIOB, GPIO_PIN_15, GPIO_PIN_RESET);   // IN1 HIGH
		HAL_GPIO_WritePin (GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
		HAL_GPIO_WritePin (GPIOA, GPIO_PIN_12, GPIO_PIN_RESET);
		HAL_GPIO_WritePin (GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);
	}
	else if(action == 5){
		stepperForward();
		stepperForward();
		stepperForward();
		stepperForward();
		stepperForward();
		stepperForward();
		stepperForward();
		stepperForward();
	}
	else if(action == 6){
		stepperBackward();
		stepperBackward();
		stepperBackward();
		stepperBackward();
		stepperBackward();
		stepperBackward();
		stepperBackward();
		stepperBackward();
	}
	else
	{
		printf("Invalid action %d\n",action);
	}
	HAL_Delay(100);//Delay to wait to be in the right state beore measurement
	int new_state = getState(env);
	int reward = NB_STATE/2 - powf(abs(new_state - NB_STATE/2),2) - iteration;
	iteration += 1;

	EnvReturn ret;
	ret.state = new_state;
	ret.reward = reward;
	ret.done = 0;
	return ret;
}

EnvReturn EnvReset(EnvironmentConfiguration *env)
{
	iteration = 0;

	EnvReturn ret;
	ret.state = getState(env);
	ret.reward = 0;
	ret.done = 0;
	return ret;
}
