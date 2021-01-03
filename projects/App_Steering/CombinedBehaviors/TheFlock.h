#pragma once
#include "../SteeringHelpers.h"
#include "FlockingSteeringBehaviors.h"
#include "SpacePartitioning.h"

class ISteeringBehavior;
class SteeringAgent;
class BlendedSteering;
class PrioritySteering;

class Flock
{
public:
	Flock(
		int flockSize = 50, 
		float worldSize = 100.f, 
		SteeringAgent* pAgentToEvade = nullptr, 
		bool trimWorld = false
	);

	~Flock();

	void Update(float deltaT);
	void UpdateAndRenderUI(BlendedSteering* m_pSteering=nullptr);
	void Render(float deltaT);


	void RegisterNeighbors(SteeringAgent* pAgent);
	int GetNrOfNeighbors() const { return m_NrOfNeighbors; }
	const vector<SteeringAgent*>& GetNeighbors() const { return m_Neighbors; }

	void SetBlendedSteering(BlendedSteering* pBlendedSteering);
	void SetPrioritySteering(PrioritySteering* pPrioritySteering);
	void SetSteeringBehaviour();
	Elite::Vector2 GetAverageNeighborPos() const;
	Elite::Vector2 GetAverageNeighborVelocity() const;

private:
	// flock agents
	int m_FlockSize = 0;
	vector<SteeringAgent*> m_Agents;
	vector<Elite::Vector2> m_OldPos;
	// neighborhood agents
	vector<SteeringAgent*> m_Neighbors;
	float m_NeighborhoodRadius = 10.f;
	int m_NrOfNeighbors = 0;

	// evade target
	SteeringAgent* m_pAgentToEvade = nullptr;

	// world info
	bool m_TrimWorld = false;
	float m_WorldSize = 0.f;
	
	// steering Behaviors
	BlendedSteering* m_pBlendedSteering = nullptr;
	PrioritySteering* m_pPrioritySteering = nullptr;

	CellSpace m_CellSpace;
	bool m_UsePartitioning;
	// private functions
	float* GetWeight(ISteeringBehavior* pBehaviour);


private:
	Flock(const Flock& other);
	Flock& operator=(const Flock& other);
};