/*
 * Agent.h
 *
 *  Created on: May 20, 2019
 *      Author: roc5
 */

#ifndef INC_AGENT_H_
#define INC_AGENT_H_

#include <time.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
	int observation_space;
	int action_space;
	float epsilon;
	float epsilon_decay;
	float epsilon_minimum;
	float learning_rate;
	float **q_matrix;
} AgentConfiguration;

AgentConfiguration* AgentInit(int obs_space, int action_space);
void AgentPrintQMatrix(AgentConfiguration *agent);
int AgentAction(AgentConfiguration *agent, int state);
void AgentLearn(AgentConfiguration *agent, int state, int action, int reward, int new_state);
float getMaxOfArray(float* array, int len);

#endif /* INC_AGENT_H_ */
