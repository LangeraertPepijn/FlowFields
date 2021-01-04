#include "stdafx.h"
#include "FlowField.h"

FlowField::FlowField(int width,int height)
	: m_Height {height}
	, m_Width {width}
{
	for (int i = 0; i < width* height; i++)
	{
		m_Directions.push_back(Elite::Vector2 (1,0 ));
		m_Cost.push_back(0);
		m_Values.push_back(0);

	}
}



FlowField::~FlowField()
{
}

Elite::Vector2 FlowField::GetDirectionAt(int index)
{
	return m_Directions[index];
}

void FlowField::SetValueAt(int index, float cost)
{
	if (index <= m_Width * m_Height)
	{

		m_Values[index] = cost;
	}
	else
	{
		std::cout << "width or height error" << std::endl;
	}
}

float FlowField::GetValueAt(int index)
{
	if (index <= m_Width * m_Height)
	{


		return m_Values[index];
	}
	else
	{
		std::cout << "width or height error" << std::endl;
		return -1;
	}
}

int FlowField::GetWidth()
{
	return m_Width;
}

int FlowField::GetHeight()
{
	return m_Height;
}

//(x+heiht*y)
void FlowField::CalculateValues(int index)
{
	for (int i{}; i < m_Height * m_Width; i++)
	{
		m_Values[i] = 1000;
	}


	m_Values[index] = 0;




	//list<int> openList;
	//openList.push_back(index);
	//while (openList.size() > 0)
	//{
	//	int currentIndex = openList.front();
	//	openList.pop_front();

	//	int currentWidth = currentIndex % m_Width;
	//	int currentHeight = currentIndex / m_Height;


	//	std::vector<int> adjacentIndices;
	//	if (currentWidth + 1 < m_Width )
	//		adjacentIndices.push_back(currentWidth + 1 + currentHeight * m_Width);
	//	if (currentWidth - 1 > 0 )
	//		adjacentIndices.push_back(currentWidth - 1 + currentHeight * m_Width);
	//	if (currentHeight + 1 < m_Height)
	//		adjacentIndices.push_back(currentWidth + (currentHeight + 1) * m_Width);
	//	if (currentHeight - 1 > 0)
	//		adjacentIndices.push_back(currentWidth + (currentHeight - 1) * m_Width);
	//	if (currentHeight - 1 > 0 && currentWidth - 1 > 0)
	//		adjacentIndices.push_back(currentWidth - 1 + (currentHeight - 1) * m_Width);
	//	if (currentHeight - 1 > 0 && currentWidth + 1 < m_Width)
	//		adjacentIndices.push_back(currentWidth + 1 + (currentHeight - 1) * m_Width);
	//	if (currentHeight + 1 < m_Height && currentWidth - 1 > 0)
	//		adjacentIndices.push_back(currentWidth - 1 + (currentHeight + 1) * m_Width);
	//	if (currentHeight + 1 < m_Height && currentWidth + 1 < m_Width)
	//		adjacentIndices.push_back(currentWidth + 1 + (currentHeight + 1) * m_Width);

	//	size_t adjacentCount{ adjacentIndices.size() };

	//	for (size_t i{}; i < adjacentCount; i++)
	//	{
	//		int endCost = m_Values[currentIndex] + m_Cost[adjacentIndices[i]];

	//		if (endCost < m_Values[adjacentIndices[i]])
	//		{
	//			if (openList.end() == std::find(openList.begin(), openList.end(), adjacentIndices[i]))
	//			{
	//				openList.push_back(adjacentIndices[i]);
	//			}
	//			m_Values[adjacentIndices[i]] = endCost;
	//		}
	//	}

	//}




}
void FlowField::SetCostAt(int index, int cost)
{
	if (index <= m_Width * m_Height)
	{

		m_Cost[index] = cost;
	}
	else
	{
		std::cout << "index error" << std::endl;
	}
}

void FlowField::SetDirtAt(int index, const Elite::Vector2& dir)
{
	if (index <= m_Width * m_Height)
	{

		m_Directions[index] = dir;
	}
	else
	{
		std::cout << "index error" << std::endl;
	}
}

int FlowField::GetCostAt(int index) const
{
	if (index <= m_Width * m_Height)
	{


		return m_Cost[index];
	}
	else
	{
		std::cout << "width or height error" << std::endl;
		return -1;
	}
}

Elite::Vector2 FlowField::GetDirAt(int index) const
{
	if (index <= m_Width * m_Height)
	{


		return m_Directions[index];
	}
	else
	{
		std::cout << "width or height error" << std::endl;
		return Elite::Vector2{};
	}
}

void FlowField::CalculateDirections() 
{
	for (int i{}; i < m_Width * m_Height; i++)
	{

		int currentWidth = i % m_Width;
		int currentHeight = i / m_Height;


		std::vector<int> adjacentIndices;
		if (currentWidth + 1 < m_Width)
			adjacentIndices.push_back(currentWidth + 1 + currentHeight * m_Width);
		if (currentWidth - 1 > 0)
			adjacentIndices.push_back(currentWidth - 1 + currentHeight * m_Width);
		if (currentHeight + 1 < m_Height)
			adjacentIndices.push_back(currentWidth + (currentHeight + 1) * m_Width);
		if (currentHeight - 1 > 0)
			adjacentIndices.push_back(currentWidth + (currentHeight - 1) * m_Width);
		if (currentHeight - 1 > 0 && currentWidth - 1 > 0)
			adjacentIndices.push_back(currentWidth - 1 + (currentHeight - 1) * m_Width);
		if (currentHeight - 1 > 0 && currentWidth + 1 < m_Width)
			adjacentIndices.push_back(currentWidth + 1 + (currentHeight - 1) * m_Width);
		if (currentHeight + 1 < m_Height && currentWidth - 1 > 0)
			adjacentIndices.push_back(currentWidth - 1 + (currentHeight + 1) * m_Width);
		if (currentHeight + 1 < m_Height && currentWidth + 1 < m_Width)
			adjacentIndices.push_back(currentWidth + 1 + (currentHeight + 1) * m_Width);
		
		
		size_t adjacentCount{ adjacentIndices.size() };
		int index{};
		if (adjacentCount != 0)
		{
			int value = INT_MAX;
			for (size_t c{}; c < adjacentCount; c++)
			{
				if (m_Values[adjacentIndices[c]] < value)
				{
					value = m_Values[adjacentIndices[c]];
					index = adjacentIndices[c];
				}
			}

			if (currentWidth + 1 < m_Width)
				if (index == currentWidth + 1 + currentHeight * m_Width)
				{
					m_Directions[i] = Elite::Vector2( 1,0 );
				}
			if (currentWidth - 1 < 0)
				if(index ==currentWidth - 1 + currentHeight * m_Width)
				{
					m_Directions[i] = Elite::Vector2(-1, 0);
				}
			if (currentHeight + 1 < m_Height)
				if(index == currentWidth + (currentHeight + 1) * m_Width)
				{
					m_Directions[i] = Elite::Vector2(0, 1);
				}
			if (currentHeight - 1 < 0)
				if(index == currentWidth + (currentHeight - 1) * m_Width)
				{
					m_Directions[i] = Elite::Vector2(0, -1);
				}
			if (currentHeight - 1 < 0 && currentWidth - 1 < m_Width)
				if(index == currentWidth - 1 + (currentHeight - 1) * m_Width)
				{
					m_Directions[i] = Elite::Vector2(-1, -1);
				}
			if (currentHeight - 1 < 0 && currentWidth + 1 < m_Width)
				if(index == currentWidth + 1 + (currentHeight - 1) * m_Width)
				{
					m_Directions[i] = Elite::Vector2(1, -1);
				}
			if (currentHeight + 1 < m_Height && currentWidth - 1 < m_Width)
				if(index == currentWidth - 1 + (currentHeight + 1) * m_Width)
				{
					m_Directions[i] = Elite::Vector2(-1, 1);
				}
			if (currentHeight + 1 < m_Height && currentWidth + 1 < m_Width)
				if(index == currentWidth + 1 + (currentHeight + 1) * m_Width)
				{
					m_Directions[i] = Elite::Vector2(1, 1);
				}

		}
	}
}

void FlowField::ResetValues()
{
	for (int i = 0; i < m_Width*m_Height; i++)
	{
		m_Values[i] = 10000000;
	}
}
