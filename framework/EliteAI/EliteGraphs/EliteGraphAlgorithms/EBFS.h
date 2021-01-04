#pragma once
#include "projects/App_FlowFields/FlowField.h"
namespace Elite 
{
	template <class T_NodeType, class T_ConnectionType>
	class BFS
	{
	public:
		BFS(IGraph<T_NodeType, T_ConnectionType>* pGraph);

		std::vector<T_NodeType*> FindPath(T_NodeType* pStartNode, T_NodeType* pDestinationNode,FlowField& flowField);
	private:
		IGraph<T_NodeType, T_ConnectionType>* m_pGraph;
	};

	template <class T_NodeType, class T_ConnectionType>
	BFS<T_NodeType, T_ConnectionType>::BFS(IGraph<T_NodeType, T_ConnectionType>* pGraph)
		: m_pGraph(pGraph)
	{
	}

	template <class T_NodeType, class T_ConnectionType>
	std::vector<T_NodeType*> BFS<T_NodeType, T_ConnectionType>::FindPath(T_NodeType* pStartNode, T_NodeType* pDestinationNode, FlowField& flowField)
	{
		// here we will calculate our path using bfs
		std::list<T_NodeType*> openList;
		std::list<T_NodeType*> closedList;
		//std::map<T_NodeType*,T_NodeType*> closedList;

		flowField.ResetValues();

		//openList.push(pStartNode);
		openList.push_back(pDestinationNode);
		flowField.SetValueAt(pDestinationNode->GetIndex(), 0);
	
		while (!openList.empty())
		{
			T_NodeType* currentNode = openList.front();
			openList.pop_front();
			int currentIndex = currentNode->GetIndex();
			//if (currentNode == pDestinationNode)
			//{
			//	break;
			//}

			for (auto con : m_pGraph->GetNodeConnections(currentNode->GetIndex()))
			{
				int test = con->GetTo();

				float endCost = flowField.GetValueAt(currentIndex) + flowField.GetCostAt(con->GetTo());
				T_NodeType* nextNode = m_pGraph->GetNode(con->GetTo());
				if (std::find(closedList.begin(),closedList.end(),nextNode) == closedList.end())
				{
					if (std::find(openList.begin(), openList.end(), nextNode) == openList.end())
					{
						openList.push_back(nextNode);
					}
					if(flowField.GetValueAt(con->GetTo())> endCost)
						flowField.SetValueAt(con->GetTo(),endCost);
				}
			}
			closedList.push_back(currentNode);
		}

		//track back
		vector<T_NodeType*> path;
		for (int i{}; i < m_pGraph->GetNrOfNodes(); i++)
		{
			float value{FLT_MAX};
			int index{};
			for (auto con : m_pGraph->GetNodeConnections(i))
			{
				float temp = flowField.GetValueAt(con->GetTo());
				//check for diagonal
				if (con->GetTo() != i + 1 && con->GetTo() != i - 1 && con->GetTo() != i + flowField.GetWidth() && con->GetTo() != i - flowField.GetWidth())
					temp += 0.5f;
				if (temp < value)
				{
					value = temp;
					index = con->GetTo();
				}
			}
			flowField.SetDirtAt(i ,m_pGraph->GetNodePos(index)- m_pGraph->GetNodePos(i));
		}


		//if (std::find(pDestinationNode)==closedList.end())
		//	return path;
		//T_NodeType* currentNode = pDestinationNode;
		//while (currentNode!= pStartNode)
		//{
		//	path.push_back(currentNode);
		//	currentNode = closedList[currentNode];
		//}

		//path.push_back(pStartNode);
		//std::reverse(path.begin(), path.end());

		return path ;
	}
}

