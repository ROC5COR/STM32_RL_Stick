/*
 * Agent.c
 *
 *  Created on: May 20, 2019
 *      Author: roc5
 */
#include "Agent.h"

AgentConfiguration* AgentInit(int obs_space, int action_space){
	AgentConfiguration *agent = (AgentConfiguration*)malloc(sizeof(AgentConfiguration));
	agent->observation_space = obs_space;
	agent->action_space = action_space;
	agent->epsilon = 1.0f;
	agent->epsilon_decay = 0.995f;//original 0.9995f;
	agent->epsilon_minimum = 0.15f; // To get still exploration states
	agent->learning_rate = 0.1f;
	printf("Sizeof int: %d, sizeof float: %d, sizeof float*: %d\n", sizeof(int),sizeof(float),sizeof(float*));

	agent->q_matrix = (float**)malloc(sizeof(float*)*obs_space);
	if(agent->q_matrix == NULL){
		printf("Error while creating q_matrix\n");
		return NULL;
	}
	for(int i = 0; i < obs_space; i++){
		float* tempArray = (float*)malloc(sizeof(float)*action_space);
		if(tempArray == NULL){
			printf("Error while creating sub q_matrix at %d with %d elements\n",i, action_space);
		}
		agent->q_matrix[i] = tempArray;

		for(int k = 0; k < action_space; k++){
			agent->q_matrix[i][k] = 1;
		}
	}

	printf("AgentInit: obs_space:%d, action_space:%d\n",agent->observation_space, agent->action_space);
	return agent;
}

void AgentPrintQMatrix(AgentConfiguration *agent){
	for(int k = 0; k < agent->action_space; k++){
		for(int i = 0; i < agent->observation_space; i++){
			printf(" %d ",(int)(agent->q_matrix[i][k]*1000));
		}
		printf("\n");
	}

}

int AgentAction(AgentConfiguration *agent, int state){
	int choice = -1;
	double randomChoice = (rand() % 100)/100.0;

	int forceRandom = 0;
	if(state > agent->observation_space){
		printf("Error with state %d > observation state: %d\n",state, agent->observation_space);
		forceRandom = 1;
	}

	if(randomChoice < (float)agent->epsilon || forceRandom == 1)
	{	//Exploration
		printf("Agent: Baby\n");
		choice = rand() % (100*agent->action_space - 1);
		choice = (int)floor(choice / 100.0);
	}
	else
	{	//Exploitation
		printf("Agent: Adult\n");

		float *actionAtState = agent->q_matrix[state];
		//AgentPrintQMatrix(agent);
		float maxAction = actionAtState[0];
		int actionIndex = 0;

		printf("Max between: ");
		for(int i=0;i<agent->action_space;i++){
			printf("f%d,", (int)(actionAtState[i]*1000));
			if(actionAtState[i] > maxAction){
				maxAction = actionAtState[i];
				actionIndex = i;
			}
		}
		printf("\n");

		choice = actionIndex; //Index with max value

	}

	return choice;
}

void AgentLearn(AgentConfiguration *agent, int state, int action, int reward, int new_state)
{
	if(state < 0 || state >= agent->observation_space){
		printf("Error state out of observation space ! %d\n",state);
		return;
	}
	if(new_state < 0 || new_state >= agent->observation_space){
		printf("Error new_state out of observation space ! %d\n",new_state);
		return;
	}

	float oldValue = agent->q_matrix[state][action];
	float learningValue = (float)reward + (float)agent->epsilon*(getMaxOfArray(agent->q_matrix[new_state], agent->action_space));
	float newValue = (1.0f - (float)agent->learning_rate)*oldValue + (float)agent->learning_rate*learningValue;
	agent->q_matrix[state][action] = newValue;

	//AgentPrintQMatrix(agent);

	if(agent->epsilon > agent->epsilon_minimum){
		agent->epsilon *= agent->epsilon_decay;
	}
}

float getMaxOfArray(float* array, int len)
{
	if(len <= 0){
		printf("Error array of len 0\n");
		return 0;
	}

	float max = array[0];
	for(int i = 0; i < len; i++){
		if(array[i] > max){
			max = array[i];
		}
	}
	return max;
}
