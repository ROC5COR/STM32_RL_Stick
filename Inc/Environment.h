/*
 * Environment.h
 *
 *  Created on: May 20, 2019
 *      Author: roc5
 */
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stm32f7xx.h>
#include "ADXL345_Interface.h"

#ifndef INC_ENVIRONMENT_H_
#define INC_ENVIRONMENT_H_


#define NB_AVERAGE 10 //Nb of average sample to smooth value
#define NB_STATE 20
#define DELAY_BETWEEN_PHASES 4
#define ACCELERO_VALUE_MIN -250
#define ACCELERO_VALUE_MAX 250

typedef struct {
	I2C_HandleTypeDef *i2C_Handle;
	int observation_space;
	int action_space;
} EnvironmentConfiguration;

typedef struct {
	int state;
	int reward;
	int done;
} EnvReturn;

EnvironmentConfiguration* EnvironmentInit(I2C_HandleTypeDef *accelero);
int getIntAverage(int* tab, int size);
EnvReturn EnvMakeAction(EnvironmentConfiguration *env, int action);
EnvReturn EnvReset(EnvironmentConfiguration *env);
int getState(EnvironmentConfiguration *env);
void stepperForward();


#endif /* INC_ENVIRONMENT_H_ */
