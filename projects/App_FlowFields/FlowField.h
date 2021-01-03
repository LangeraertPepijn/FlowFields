#pragma once
class FlowField
{
public:
	FlowField();
	FlowField(Elite::Vector2 direction);
	~FlowField();
	Elite::Vector2* GetDirection();
private:

	Elite::Vector2 m_Direction{};
};

