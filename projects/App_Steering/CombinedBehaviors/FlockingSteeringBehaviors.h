#pragma once
#include "../SteeringBehaviors.h"

class Flock;
//SEPARATION - FLOCKING
//*********************
class Separation : public ISteeringBehavior
{
public:
	Separation(Flock* pFlock);
	virtual ~Separation();
	
	SteeringOutput CalculateSteering(float deltaTime, SteeringAgent* pAgent)override;
private:
	Flock* m_pFlock = nullptr;
	float m_Distance{10.f};


};

//COHESION - FLOCKING
//*******************
class Cohesion: public ISteeringBehavior
{
public:
	Cohesion(Flock* pFlock);
	virtual ~Cohesion();
	
	SteeringOutput CalculateSteering(float deltaTime, SteeringAgent* pAgent)override ;
private:
	Flock* m_pFlock = nullptr;
	
	
};

//VELOCITY MATCH - FLOCKING
//************************
class Alignment : public ISteeringBehavior
{
public:
	Alignment(Flock* pFlock);
	virtual ~Alignment();

	SteeringOutput CalculateSteering(float deltaTime, SteeringAgent* pAgent)override;
private:
	Flock* m_pFlock = nullptr;


};