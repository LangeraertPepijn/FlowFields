#include "stdafx.h"
#include "SpacePartitioning.h"
#include "projects\App_Steering\SteeringAgent.h"

// --- Cell ---
// ------------
Cell::Cell(float left, float bottom, float width, float height)
{
	boundingBox.bottomLeft = { left, bottom };
	boundingBox.width = width;
	boundingBox.height = height;
}

std::vector<Elite::Vector2> Cell::GetRectPoints() const
{
	auto left = boundingBox.bottomLeft.x;
	auto bottom = boundingBox.bottomLeft.y;
	auto width = boundingBox.width;
	auto height = boundingBox.height;

	std::vector<Elite::Vector2> rectPoints =
	{
		{ left , bottom  },
		{ left , bottom + height  },
		{ left + width , bottom + height },
		{ left + width , bottom  },
	};

	return rectPoints;
}

// --- Partitioned Space ---
// -------------------------
CellSpace::CellSpace(float width, float height, int rows, int cols, int maxEntities)
	: m_SpaceWidth(width)
	, m_SpaceHeight(height)
	, m_NrOfRows(rows)
	, m_NrOfCols(cols)
	, m_Neighbors(maxEntities)
	, m_NrOfNeighbors(0)
	, m_RenderOn(true)
{
	m_CellHeight = m_SpaceHeight / m_NrOfRows;
	m_CellWidth = m_SpaceWidth / m_NrOfCols;
	for (int i{}; i < m_NrOfCols ; i++)
	{
		for(int j{};j  < m_NrOfRows; j++)
		{
			Elite::Vector2 point{ m_CellWidth * j,m_CellHeight * i };
			m_Cells.push_back(Cell{ point.x,point.y,m_CellWidth,m_CellHeight });
		}


	}
}

void CellSpace::AddAgent(SteeringAgent* agent)
{
	int place{ PositionToIndex(agent->GetPosition()) };
	if (place < int(m_Cells.size()))
	{
		m_Cells[place].agents.push_back(agent);
	}

}

void CellSpace::UpdateAgentCell(SteeringAgent* agent, const Elite::Vector2& oldPos)
{
	Elite::Vector2 test =agent->GetPosition();
	int place{ PositionToIndex(agent->GetPosition()) };
	int oldPlace{ PositionToIndex(oldPos) };
	if (place == oldPlace)
	{
		return;
	}
	m_Cells[oldPlace].agents.remove(agent);

	m_Cells[place].agents.push_back(agent);
	

}

void CellSpace::RegisterNeighbors(const Elite::Vector2& pos, float queryRadius)
{
	m_NrOfNeighbors = 0;
	m_Neighbors.clear();
	for (Cell cell : m_Cells)
	{
		Elite::Rect temp{ Elite::Vector2{pos.x - queryRadius,pos.y - queryRadius},queryRadius*2,queryRadius*2 };
		
		if (Elite::IsOverlapping(cell.boundingBox, temp))
		{
			for ( SteeringAgent* agent : cell.agents)
			{
				if (Elite::Distance(agent->GetPosition(), pos)<queryRadius&& Elite::Distance(agent->GetPosition(), pos)!=0)
				{
					m_Neighbors.push_back(agent);
					m_NrOfNeighbors++;
				}
			}
		}
	}
	
	
}

void CellSpace::RenderCells() const
{
	if (m_RenderOn)
	{
		for (int i{ 0 }; i < m_NrOfCols*m_NrOfRows;i++)
		{
			std::string temp{};
	
			
			temp =std::to_string(int(m_Cells[i].agents.size()));

			
			Elite::Vector2 points[4]{ {m_CellWidth * ((i) % m_NrOfCols),m_CellHeight * ((i) / m_NrOfCols)},
				{m_CellWidth * ((i % m_NrOfCols)+1),m_CellHeight * ((i) / m_NrOfCols)},
				{m_CellWidth*((i%m_NrOfCols)+1),m_CellHeight*((i /m_NrOfCols)+1)},
				{m_CellWidth * ((i) % m_NrOfCols),m_CellHeight * ((i / m_NrOfCols)+1)} };
			DEBUGRENDERER2D->DrawPolygon(&points[0], 4, { 1,0,0,1 }, 0.4f);
			DEBUGRENDERER2D->DrawString(points[3], temp.c_str());

		}

	}
}

void CellSpace::ChangeRenderActivity(bool on)
{
	m_RenderOn = on;
}

bool& CellSpace::GetRenderActivity()
{
	return m_RenderOn;
}



int CellSpace::PositionToIndex(const Elite::Vector2 pos) const
{

	int tempWidthCount{};
	int tempHeightCount{};
	int i{0};
	while (pos.x > m_CellWidth*i)
	{
		++i;
	}
	tempWidthCount = i - 1;
	i = 0;
	while (pos.y > m_CellHeight * i)
	{
		++i;
	}

	tempHeightCount = i - 1;
	int temp{ m_NrOfCols * tempHeightCount + tempWidthCount };
	temp=temp% (m_NrOfCols* m_NrOfRows);
	if(temp<0)
	{
		temp += m_NrOfCols * m_NrOfRows;
	}
	return temp;
}