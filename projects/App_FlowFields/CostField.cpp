#include "stdafx.h"
#include "CostField.h"

CostField::CostField( int width,  int height)
	: m_Height {height}
	, m_Width {width}
	, m_Cost {}
{
	for ( int i{}; i < height * width; i++)
	{
		m_Cost.push_back(0);
	}
}

CostField::~CostField()
{
}

void CostField::SetCostAt(int index,int cost)
{
	if (index<= m_Width*m_Height)
	{

		m_Cost[index] = cost;
	}
	else
	{
		std::cout << "index error" << std::endl;
	}
}

int CostField::GetCostAt(int index)
{
	if (index <= m_Width * m_Height)
	{


		return m_Cost[index] ;
	}
	else
	{
		std::cout << "width or height error" << std::endl;
		return -1.f;
	}
}
