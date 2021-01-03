#include "stdafx.h"
#include "FlowField.h"

FlowField::FlowField()
	: m_Direction{1.f,1.f}
{
}

FlowField::FlowField(Elite::Vector2 direction)
	: m_Direction{direction}
{
}

FlowField::~FlowField()
{
}

Elite::Vector2* FlowField::GetDirection()
{
	return &m_Direction;
}
