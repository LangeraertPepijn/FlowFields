#pragma once

#include "framework/EliteMath/FMatrix.h"
#include "QBot.h"
class DynamicQLearning
{
public:
	DynamicQLearning(int nrOfFood, int memorySize, int nrOfInputs, int nrOfOutputs, bool bias );
	~DynamicQLearning();
	void Update(float deltaTime) {
		m_pQBot->Update(m_Foodstuff,deltaTime);
		for (Food* food : m_Foodstuff) {
			food->Update();
		}
	}
	void Render(float deltaTime);
private:
	static const int MEMORY_SIZE = 200;
	
	int m_MemorySize;
	int m_NrOfInputs;
	int m_NrOfOutputs;
	bool m_UseBias;

	// currentIndex stores the information at the current time.
	// instead of swapping or copying matrices, manipulate the currentIndex to
	// go back in time.
	int currentIndex = 0;
	QBot* m_pQBot{ 0 };

	// environment
	vector<Food*> m_Foodstuff;
};

