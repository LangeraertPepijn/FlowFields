#include "stdafx.h"
#include "FlockingSteeringBehaviors.h"
#include "TheFlock.h"
#include "../SteeringAgent.h"
#include "../SteeringHelpers.h"


//*********************
//SEPARATION (FLOCKING)
Separation::Separation(Flock* pFlock)
	:m_pFlock{pFlock}
{

}

Separation::~Separation()
{
	m_pFlock= nullptr;
}

SteeringOutput Separation::CalculateSteering(float deltaTime, SteeringAgent* pAgent)
{
	SteeringOutput steering{};
	Elite::Vector2 temp{};
	Elite::Vector2 directionVector{};

	for (size_t i{}; i < size_t(m_pFlock->GetNrOfNeighbors()); i++)
	{
		temp = m_pFlock->GetNeighbors()[i]->GetPosition() - pAgent->GetPosition();
		temp.GetNormalized();

	
		directionVector += temp * 1 / Elite::Distance(m_pFlock->GetNeighbors()[i]->GetPosition(), pAgent->GetPosition());
	
		
	}
	directionVector.GetNormalized();
	steering.LinearVelocity = (directionVector*-1);
	steering.LinearVelocity.Normalize();
	steering.LinearVelocity *= pAgent->GetMaxLinearSpeed();

	if (pAgent->CanRenderBehavior())
	{
		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), steering.LinearVelocity, 5, { 0, 1, 0, 0.5f }, 0.40f);
	}

	return steering;
}



//*******************
//COHESION (FLOCKING)

Cohesion::Cohesion(Flock* pFlock)
{
	m_pFlock = pFlock;
}

Cohesion::~Cohesion()
{
	m_pFlock = nullptr;
}

SteeringOutput Cohesion::CalculateSteering(float deltaTime, SteeringAgent* pAgent)
{
	SteeringOutput steering{};

	
	steering.LinearVelocity = ( m_pFlock->GetAverageNeighborPos()- pAgent->GetPosition());
	steering.LinearVelocity.Normalize();
	steering.LinearVelocity *= pAgent->GetMaxLinearSpeed();

	if (pAgent->CanRenderBehavior())
	{
		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), steering.LinearVelocity, 5, { 0, 1, 0, 0.5f }, 0.40f);
	}

	return steering;
}



//*************************
//VELOCITY MATCH (FLOCKING)

Alignment::Alignment(Flock* pFlock)
	:m_pFlock{pFlock}
{
}

Alignment::~Alignment()
{
	m_pFlock = nullptr;
}

SteeringOutput Alignment::CalculateSteering(float deltaTime, SteeringAgent* pAgent)
{
	SteeringOutput steering{};

	
	steering.LinearVelocity = (m_pFlock->GetAverageNeighborVelocity());
	steering.LinearVelocity.Normalize();
	steering.LinearVelocity *= pAgent->GetMaxLinearSpeed();

	if (pAgent->CanRenderBehavior())
	{
		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), steering.LinearVelocity, 5, { 0, 1, 0, 0.5f }, 0.40f);
	}

	return steering;
}
