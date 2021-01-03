#include "stdafx.h"
#include "ENavGraph.h"
#include "framework\EliteAI\EliteGraphs\EliteGraphAlgorithms\EAStar.h"

using namespace Elite;

Elite::NavGraph::NavGraph(const Polygon& contourMesh, float playerRadius = 1.0f) :
	Graph2D(false),
	m_pNavMeshPolygon(nullptr)
{
	//Create the navigation mesh (polygon of navigatable area= Contour - Static Shapes)
	m_pNavMeshPolygon = new Polygon(contourMesh); // Create copy on heap

	//Get all shapes from all static rigidbodies with NavigationCollider flag
	auto vShapes = PHYSICSWORLD->GetAllStaticShapesInWorld(PhysicsFlags::NavigationCollider);

	//Store all children
	for (auto shape : vShapes)
	{
		shape.ExpandShape(playerRadius);
		m_pNavMeshPolygon->AddChild(shape);
	}

	//Triangulate
	m_pNavMeshPolygon->Triangulate();

	//Create the actual graph (nodes & connections) from the navigation mesh
	CreateNavigationGraph();
}

Elite::NavGraph::~NavGraph()
{
	delete m_pNavMeshPolygon; 
	m_pNavMeshPolygon = nullptr;
}

int Elite::NavGraph::GetNodeIdxFromLineIdx(int lineIdx) const
{
	auto nodeIt = std::find_if(m_Nodes.begin(), m_Nodes.end(), [lineIdx](const NavGraphNode* n) { return n->GetLineIndex() == lineIdx; });
	if (nodeIt != m_Nodes.end())
	{
		return (*nodeIt)->GetIndex();
	}

	return invalid_node_index;
}

Elite::Polygon* Elite::NavGraph::GetNavMeshPolygon() const
{
	return m_pNavMeshPolygon;
}

void Elite::NavGraph::CreateNavigationGraph()
{
	//1. Go over all the edges of the navigationmesh and create nodes
	

	Elite::Vector2 temp{};
	int counter{};
	std::vector<const Elite::Triangle*> triangles{};
	
	for (Elite::Line* line : m_pNavMeshPolygon->GetLines())
	{
		std::vector<const Elite::Triangle*> triangleHolder{ m_pNavMeshPolygon->GetTrianglesFromLineIndex(line->index) };
		for (size_t i = 0; i < triangleHolder.size(); i++)
		{
			triangles.push_back(triangleHolder[i]);
		}
		if (triangleHolder.size() > 1)
		{
			temp = line->p1 + (line->p2 - line->p1)/2;
			AddNode(new Elite::NavGraphNode(counter, line->index, temp));
			counter++;
		}


	}
	//2. Create connections now that every node is created


	for (size_t i = 0; i < triangles.size(); i++)
	{
		std::vector<Elite::NavGraphNode*> nodes{};
	
		for (unsigned int c : triangles[i]->metaData.IndexLines)
		{
			size_t count{  };
			while (count < m_Nodes.size())
			{
				if (c == m_Nodes[count]->GetLineIndex())
				{
					nodes.push_back(m_Nodes[count]);
					count = m_Nodes.size();

				}
				count++;

			}

		}
		
		if (nodes.size()==3)
		{
			if (IsUniqueConnection(nodes[0]->GetIndex(), nodes[1]->GetIndex()))
			{
				AddConnection(new Elite::GraphConnection2D{ nodes[0]->GetIndex(),nodes[1]->GetIndex() });
				AddConnection(new Elite::GraphConnection2D{ nodes[1]->GetIndex(),nodes[2]->GetIndex() });
				AddConnection(new Elite::GraphConnection2D{ nodes[2]->GetIndex(),nodes[0]->GetIndex() });
			
			}
		}
		else if (nodes.size() == 2)
		{
			if (IsUniqueConnection(nodes[0]->GetIndex(), nodes[1]->GetIndex()))
			{
				AddConnection(new Elite::GraphConnection2D{ nodes[0]->GetIndex(),nodes[1]->GetIndex() });
	
			}
		}
	}

	//3. Set the connections cost to the actual distance
	SetConnectionCostsToDistance();

}

