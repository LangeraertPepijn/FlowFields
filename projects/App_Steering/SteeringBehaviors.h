/*=============================================================================*/
// Copyright 2017-2018 Elite Engine
// Authors: Matthieu Delaere, Thomas Goussaert
/*=============================================================================*/
// SteeringBehaviors.h: SteeringBehaviors interface and different implementations
/*=============================================================================*/
#ifndef ELITE_STEERINGBEHAVIORS
#define ELITE_STEERINGBEHAVIORS

//-----------------------------------------------------------------
// Includes & Forward Declarations
//-----------------------------------------------------------------
#include "SteeringHelpers.h"
class SteeringAgent;
using namespace Elite;

#pragma region **ISTEERINGBEHAVIOR** (BASE)
class ISteeringBehavior
{
public:
	ISteeringBehavior() = default;
	virtual ~ISteeringBehavior() = default;

	virtual SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) = 0;

	//Seek Functions
	void SetTarget(const TargetData& target) { m_Target = target; }

	template<class T, typename std::enable_if<std::is_base_of<ISteeringBehavior, T>::value>::type* = nullptr>
	T* As()
	{ return static_cast<T*>(this); }

protected:
	TargetData m_Target;
};
#pragma endregion

///////////////////////////////////////
//SEEK
//****
class Seek : public ISteeringBehavior
{
public:
	Seek() = default;
	virtual ~Seek() = default;

	//Seek Behaviour
	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;
	//virtual void SetTarget(const TargetData& pTarget) { m_Target = pTarget; }
protected:
	
	 
};

//////////////////////////
//WANDER
//******
class Wander : public Seek
{
public:
	Wander() = default;
	virtual ~Wander() = default;

	//Wander Behavior
	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;
	void SetWanderOffset(float offset) { m_Offset = offset; }
	void SetWanderRadius(float radius) { m_Radius = radius; }
	void SetMaxAngleChange(float rad) { m_AngleChange = rad; }
protected:
	float m_Offset = 6.f;
	float m_Radius = 4.f;
	float m_AngleChange = ToRadians(45);
	float m_WanderAngle = 0.f;
private:
	//void SetTarget(const TargetData& pTarget)override { m_Target = pTarget; };
};

class Flee :public Seek
{
public:
	Flee() = default;
	virtual ~Flee()=default;
	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;
private:
	float m_FleeRadius = 50.f;
};

class Arrive :public Seek
{
public:
	Arrive() = default;
	virtual ~Arrive() = default;
	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent)override;

	
	void SetDistanceLeft(float distanceLeft) { m_DistanceLeft = distanceLeft; }
	void SetSlowRadius(float slowRadius) { m_SlowRadius = slowRadius; }

protected:
	
	float m_SlowRadius = 4.f;
	float m_DistanceLeft = 0.f;
};

class Face :public Seek
{
public:
	Face() = default;
	virtual ~Face() = default;
	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent)override;

};


class Pursuit :public Seek
{
public:
	Pursuit() = default;
	virtual ~Pursuit() = default;
	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent)override;

	void SetPursuitOffset(float offset) { m_Offset = offset; }

protected:
	float m_Offset = 6.f;

private:
	//void SetTarget(const TargetData& pTarget)override { m_Target = pTarget; };
};
class Evade :public Seek
{
public:
	Evade() = default;
	virtual ~Evade() = default;
	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent)override;

	void SetEvadeOffset(float offset) { m_Offset = offset; }

protected:
	float m_Offset = 10.f;
	float m_Radius = 30.f;


private:
	//void SetTarget(const TargetData& pTarget)override { m_Target = pTarget; };
};
#endif


