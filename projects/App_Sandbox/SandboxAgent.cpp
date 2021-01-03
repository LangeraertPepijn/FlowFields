#include "stdafx.h"
#include "SandboxAgent.h"

using namespace Elite;

SandboxAgent::SandboxAgent(): BaseAgent()
{
	m_Target = GetPosition();
}

void SandboxAgent::Update(float dt)
{
	float maxSpeed = 50.f;
	Elite::Vector2 toTarget = m_Target - GetPosition();
	toTarget.Normalize();

	toTarget *= maxSpeed;
	SetLinearVelocity(toTarget);

	//Orientation
	const auto angle = Orientation(GetRotation(), GetLinearVelocity());
	m_pRigidBody->SetTransform(Transform(GetPosition(), Elite::Vector2(angle, angle)));
}

void SandboxAgent::Render(float dt)
{
	BaseAgent::Render(dt); //Default Agent Rendering
}

float SandboxAgent::Orientation(float currentOrientation, Elite::Vector2 velocity) const
{
	//Determine angle based on direction
	if (velocity.Magnitude() > 0)
	{
		velocity.Normalize();
		return atan2(-velocity.x, velocity.y);
	}
	return currentOrientation;
}