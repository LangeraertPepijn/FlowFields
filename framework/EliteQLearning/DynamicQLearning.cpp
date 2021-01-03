#include "stdafx.h"
#include "DynamicQLearning.h"



DynamicQLearning::DynamicQLearning(int nrOfFood, int memorySize, int nrOfInputs, int nrOfOutputs, bool bias)
	:m_MemorySize(memorySize),
	m_NrOfInputs(nrOfInputs),
	m_NrOfOutputs(nrOfOutputs),
	m_UseBias(bias)
{
	

	float startx = Elite::randomInt(100)-50;
	float starty = Elite::randomInt(100)-50;
	float startAngle = Elite::randomFloat(0, M_PI * 2);
	
	// memory is 100.
	m_pQBot = new QBot(startx, starty, M_PI / 3, 2*M_PI , startAngle, 50, m_NrOfInputs, m_NrOfOutputs,m_UseBias);
	

	//Initialization of your application. If you want access to the physics world you will need to store it yourself.
	for (int i = 0; i < nrOfFood; ++i)
	{
		float angle = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX));
		angle *= 2 * M_PI;
		float dist = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX));
		dist *= 200;
		dist += 20;
		Food* f = new Food(dist * cos(angle), dist * sin(angle));
		m_Foodstuff.push_back(f);
	}

}

DynamicQLearning::~DynamicQLearning()
{
	SAFE_DELETE(m_pQBot);
	for (Food* food : m_Foodstuff)
	{
		SAFE_DELETE(food);
	}
}

void DynamicQLearning::Render(float deltaTime)
{
	for (Food* food : m_Foodstuff) {
		food->Render();
	}
	m_pQBot->Render(deltaTime);
}