#ifndef QBOT_H
#define QBOT_H

#include "framework\EliteMath\EVector2.h"
#include "Food.h"
#include "framework/EliteMath/FMatrix.h"
//#include "Generation.h"

class QBot
{
private:
	Elite::Vector2 m_Location;
	Elite::Vector2 m_StartLocation;
	float m_Angle;
	float m_FOV;
	float m_MaxDistance = 50.0f;
	float m_SFOV;
	Color m_AliveColor;
	Color m_DeadColor;
	float m_Speed = 30.0f;
	float m_Health{ 100.0f };
	bool m_Alive = true;
	float m_Age{ 0.0f };

	// fitness members
	float m_TimeOfDeath = 0;
	int m_FoodEaten = 0;

	vector<Food*> m_Visible;

	int m_NrOfInputs;
	bool m_UseBias;
	int m_NrOfOutputs;
	int m_MemorySize;

	// currentIndex stores the information at the current time.
	// instead of swapping or copying matrices, manipulate the currentIndex to
	// go back in time. currentIndex is updated at the start of the update method
	// so that the render method also has the correct currentIndex. But make sure
	// the matrices at 0 are also filled in, otherwise problems.
	int currentIndex{ -1 };
	FMatrix* m_StateMatrixMemoryArr;
	FMatrix* m_ActionMatrixMemoryArr;
	FMatrix m_BotBrain;
	FMatrix m_DeltaBotBrain;
	FMatrix m_SAngle;

	// Q-factors, enable usage for different learning parameters for positive or for negative reinforcement.
	float m_NegativeQ{-0.2f};
	float m_NegativeQClose{ -0.1f };
	float m_PositiveQ{ 0.1f };
	int m_CameCloseCounter{ 0 };

public:
	QBot(float x,
		float y,
		float fov,
		float sFov,
		float angle,
		int memorySize,
		int nrInputs,
		int nrOutputs,
		bool useBias) :
		m_Location(x, y), m_StartLocation(x,y),m_FOV(fov), m_SFOV(sFov), m_Angle(angle),
		m_AliveColor(0.1f, 0.5f, .5f),
		m_DeadColor(.75f, 0.1f, .2f),
		m_NrOfInputs(nrInputs),
		m_NrOfOutputs(nrOutputs),
		m_MemorySize(memorySize),
		m_UseBias(useBias),
		m_BotBrain(nrInputs + (useBias?1:0), nrOutputs),
		m_DeltaBotBrain(nrInputs+(useBias?1:0),nrOutputs),
		m_SAngle(1, nrOutputs)
	{
		float start = -m_SFOV / 2;
		float step = m_SFOV / (nrOutputs - 1);
		for (int i = 0; i < nrOutputs; ++i)
		{
			float value = start + i * step;
			m_SAngle.Set(0, i, value);
		}

		m_ActionMatrixMemoryArr = new FMatrix[m_MemorySize]();
		m_StateMatrixMemoryArr = new FMatrix[m_MemorySize]();

		for (int i = 0; i < m_MemorySize; ++i)
		{
			m_StateMatrixMemoryArr[i].Resize(1, m_NrOfInputs + (m_UseBias ? 1 : 0));
			m_ActionMatrixMemoryArr[i].Resize(1, m_NrOfOutputs);
		}
		m_BotBrain.Randomize(-1.0f, 2.0f);
		if (m_UseBias) {
			m_BotBrain.SetRowAll(m_NrOfInputs,-10.0f);
		}
		
		m_BotBrain.Print();
	}
	~QBot()
	{
		delete[]m_ActionMatrixMemoryArr;
		delete[]m_StateMatrixMemoryArr;
	}
	void Update(vector<Food*>& food, float deltaTime);
	void Render(float deltaTime);
	bool nearMiss(vector<Food*>& food);

	inline auto AngleBetween(const Vector2& v1, const Vector2& v2) {
		double x = v1.Dot(v2);
		double y = v1.Cross(v2);
		return atan2(y, x);
	}


	bool IsAlive() {
		return m_Alive;
	}

	/*void Reset(Generation* gen) {
		m_Health = 100;
		m_TimeOfDeath = 0;
		m_Alive = true;
		m_FoodEaten = 0;

		m_Location.x = gen->Random(-50, 50);
		m_Location.y = gen->Random(-50, 50);
		m_Angle = gen->Random(0, M_PI * 2);
	}*/

	float CalculateFitness() const{
		return m_FoodEaten + m_TimeOfDeath;
	}

	

	

	/*void MutateMatrix(Generation* gen, FMatrix& matrix, float mutationRate, float mutationAmplitude) {
		for (int c = 0; c < matrix.GetNrOfColumns(); ++c) {
			for (int r = 0; r < matrix.GetNrOfRows(); ++r) {
				if (gen->Random(0, 1) < mutationRate) {
					float update = gen->Random(-mutationAmplitude, mutationAmplitude);
					float currentVal = matrix.Get(r, c);
					matrix.Set(r, c, currentVal + update);
				}
			}
		}
	}*/

	void Reinforcement(float factor,int memory);

	float CalculateInverseDistance(float realDist) {
		// version 1 
		//return m_MaxDistance - realDist;
		// version 2
		float nDist = realDist / m_MaxDistance;
		float invDistSquared = m_MaxDistance / (1 + nDist * nDist);
		return invDistSquared;
	}
};

#endif