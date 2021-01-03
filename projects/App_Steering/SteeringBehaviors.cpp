//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

//Includes
#include "SteeringBehaviors.h"
#include "SteeringAgent.h"

//SEEK
//****
SteeringOutput Seek::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering{};
	
	
	steering.LinearVelocity = (m_Target.Position - pAgent->GetPosition());
	steering.LinearVelocity.Normalize();
	steering.LinearVelocity *= pAgent->GetMaxLinearSpeed();

	if(pAgent->CanRenderBehavior())
	{
		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), steering.LinearVelocity, 5, { 0, 1, 0, 0.5f }, 0.40f);
	}

	return steering;
}

//WANDER (base> SEEK)
//******
SteeringOutput Wander::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering{};
	Elite::Vector2 center = pAgent->GetPosition();
	Elite::Vector2 direction =pAgent->GetDirection();
	center = center+direction* m_Offset;
	float value{};
	bool activateLoop{ true };
	while (activateLoop)
	{
		float randomF = Elite::randomFloat(0, 6.28f);

		Elite::Vector2 dir{ pAgent->GetPosition().x + cos(pAgent->GetRotation() - ToRadians(90)), pAgent->GetPosition().y + sin(pAgent->GetRotation() - ToRadians(90)) };
		Elite::Vector2 toTargetVect{ cos(randomF) * m_Radius + center.x - pAgent->GetPosition().x , sin(randomF) * m_Radius + center.y - pAgent->GetPosition().y };

		m_WanderAngle = acos(Dot(dir, toTargetVect) / (dir.Magnitude() * toTargetVect.Magnitude()));
		value = randomF;
		if (m_AngleChange > m_WanderAngle || 6.28-m_AngleChange > m_WanderAngle)
		{
			activateLoop=false;
		}
	}
	SetTarget(Elite::Vector2(cos(value) * m_Radius + center.x, sin(value) * m_Radius + center.y));

	steering.LinearVelocity = (m_Target.Position - pAgent->GetPosition());
	steering.LinearVelocity.Normalize();
	steering.LinearVelocity *= pAgent->GetMaxLinearSpeed();

	if (pAgent->CanRenderBehavior())
	{
		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), steering.LinearVelocity, 5, { 0, 1, 0, 0.5f }, 0.40f);
		DEBUGRENDERER2D->DrawCircle(center, m_Radius, { 0,0,1,0.5f }, 0.4f);
		DEBUGRENDERER2D->DrawCircle(m_Target.Position, 1.f, { 0,0,1,0.5f }, 0.4f);
	}
	//SAFE_DELETE(pTarget);
	return steering;
}
//Flee
SteeringOutput Flee::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	auto distanceToTarget = Distance(pAgent->GetPosition(), m_Target.Position);
	if (distanceToTarget > m_FleeRadius)
	{
		SteeringOutput steering{};
		steering.IsValid = false;
		return steering;
	}
	SteeringOutput steering{};

	steering.LinearVelocity = (m_Target.Position - pAgent->GetPosition());
	steering.LinearVelocity.Normalize();
	steering.LinearVelocity *= -1*pAgent->GetMaxLinearSpeed();

	if (pAgent->CanRenderBehavior())
	{
		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), steering.LinearVelocity, 5, { 0, 1, 0, 0.5f }, 0.40f);
	}
	return steering;
}
// Arrive
SteeringOutput Arrive::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering{};
	Elite::Vector2 temp = m_Target.Position - pAgent->GetPosition();
	float distance =sqrt(temp.x* temp.x + temp.y * temp.y);
	SetDistanceLeft(distance);
	if (distance < m_SlowRadius)
	{
		steering.LinearVelocity = (m_Target.Position - pAgent->GetPosition());
		steering.LinearVelocity.Normalize();
		steering.LinearVelocity *= (pAgent->GetMaxLinearSpeed()*(distance/m_SlowRadius));
	}
	else
	{
		steering.LinearVelocity = (m_Target.Position - pAgent->GetPosition());
		steering.LinearVelocity.Normalize();
		steering.LinearVelocity *= pAgent->GetMaxLinearSpeed();
	}

	if (pAgent->CanRenderBehavior())
	{
		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), steering.LinearVelocity, 5, { 0, 1, 0, 0.5f }, 0.40f);
	}
	return steering;
}
// Face
SteeringOutput Face::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering{};
	pAgent->GetDirection();
	Elite::Vector2 temp = m_Target.Position - pAgent->GetPosition();
	float temp2 =atan2( pAgent->GetDirection().y-temp.y, pAgent->GetDirection().x-temp.x);
	pAgent->SetAutoOrient(false);
	steering.AngularVelocity = pAgent->GetMaxAngularSpeed();	
	pAgent->SetRotation(temp2-ToRadians(90));
	

	

	if (pAgent->CanRenderBehavior())
	{
		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), steering.LinearVelocity, 5, { 0, 1, 0, 0.5f }, 0.40f);
	}
	return steering;
}
// Pursuit
SteeringOutput Pursuit::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering{};
	Elite::Vector2 newPos = Elite::Vector2{ m_Target.Position.x + cos(m_Target.Orientation) + m_Offset,m_Target.Position.y + sin(m_Target.Orientation) + m_Offset };
	
	//TargetData* newTarget = new TargetData{ newPos };
	SetTarget(TargetData{ newPos });
	Elite::Vector2 temp = m_Target.Position - pAgent->GetPosition();
	float distance = sqrt(temp.x * temp.x + temp.y * temp.y);
	if (distance < m_Offset)
	{
		SetPursuitOffset(distance);
	}
	steering.LinearVelocity = (m_Target.Position - pAgent->GetPosition());
	steering.LinearVelocity.Normalize();
	steering.LinearVelocity *= pAgent->GetMaxLinearSpeed();

	if (pAgent->CanRenderBehavior())
	{
		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), steering.LinearVelocity, 5, { 0, 1, 0, 0.5f }, 0.40f);
	}
	//SAFE_DELETE(newTarget);
	return steering;
}

// Evade
SteeringOutput Evade::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	//SteeringOutput steering{};
	Elite::Vector2 newPos = Elite::Vector2{ m_Target.Position.x + cos(m_Target.Orientation) + m_Offset,m_Target.Position.y + sin(m_Target.Orientation) + m_Offset };
	auto distanceToTarget = Distance(pAgent->GetPosition(), newPos);
	if (distanceToTarget > m_Radius)
	{
		SteeringOutput steering{};
		steering.IsValid = false;
		return steering;
	}
	SteeringOutput steering{};

	steering.LinearVelocity = (m_Target.Position - pAgent->GetPosition());
	steering.LinearVelocity.Normalize();
	steering.LinearVelocity *= -1 * pAgent->GetMaxLinearSpeed();

	if (pAgent->CanRenderBehavior())
	{
		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), steering.LinearVelocity, 5, { 0, 1, 0, 0.5f }, 0.40f);
	}
	return steering;
}