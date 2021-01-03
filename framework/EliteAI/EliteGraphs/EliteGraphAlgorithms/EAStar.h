#pragma once

namespace Elite
{
	template <class T_NodeType, class T_ConnectionType>
	class AStar
	{
	public:
		AStar(IGraph<T_NodeType, T_ConnectionType>* pGraph, Heuristic hFunction);

		// stores the optimal connection to a node and its total costs related to the start and end node of the path
		struct NodeRecord
		{
			T_NodeType* pNode = nullptr;
			T_ConnectionType* pConnection = nullptr;
			float costSoFar = 0.f; // accumulated g-costs of all the connections leading up to this one
			float estimatedTotalCost = 0.f; // f-cost (= costSoFar + h-cost)

			bool operator==(const NodeRecord& other) const
			{
				return pNode == other.pNode
					&& pConnection == other.pConnection
					&& costSoFar == other.costSoFar
					&& estimatedTotalCost == other.estimatedTotalCost;
			};

			bool operator<(const NodeRecord& other) const
			{
				return estimatedTotalCost < other.estimatedTotalCost;
			};
		};

		std::vector<T_NodeType*> FindPath(T_NodeType* pStartNode, T_NodeType* pDestinationNode);

	private:
		float GetHeuristicCost(T_NodeType* pStartNode, T_NodeType* pEndNode) const;

		IGraph<T_NodeType, T_ConnectionType>* m_pGraph;
		Heuristic m_HeuristicFunction;
	};

	template <class T_NodeType, class T_ConnectionType>
	AStar<T_NodeType, T_ConnectionType>::AStar(IGraph<T_NodeType, T_ConnectionType>* pGraph, Heuristic hFunction)
		: m_pGraph(pGraph)
		, m_HeuristicFunction(hFunction)
	{
	}

	template <class T_NodeType, class T_ConnectionType>
	std::vector<T_NodeType*> AStar<T_NodeType, T_ConnectionType>::FindPath(T_NodeType* pStartNode, T_NodeType* pGoalNode)
	{
		// here we will calculate our path using astar
		vector<T_NodeType*>path;
		vector<NodeRecord> openList;
		vector<NodeRecord> closedList;
		NodeRecord currentRecord;

		NodeRecord startRecord{ pStartNode,nullptr,0,GetHeuristicCost(pStartNode,pGoalNode) };
		openList.push_back(startRecord);
		while (openList.size() != 0)
		{
			float tempCost{ FLT_MAX };
			for (size_t i = 0; i < openList.size(); i++)
			{
				if (openList[i].estimatedTotalCost <= tempCost)
				{
					currentRecord = openList[i];
					tempCost = openList[i].estimatedTotalCost;
				}
			}
			if (currentRecord.pNode == pGoalNode)
			{
				break;
			}
			else
			{
				list<T_ConnectionType* > temp = m_pGraph->GetNodeConnections(currentRecord.pNode->GetIndex());
				for (auto connection : temp)
				{
					bool check1{};
					bool check2{};
					T_NodeType* pNode = m_pGraph->GetNode(connection->GetTo());
					float cost = currentRecord.costSoFar + connection->GetCost();
					for (size_t c = 0; c < closedList.size(); c++)
					{
						if (closedList[c].pNode ==pNode )
						{
							if (closedList[c].costSoFar <= cost )
							{
								check1 = true;
							}
							else
							{
								closedList.erase(closedList.begin() + c);
							}
						}
					}
					for (size_t c = 0; c < openList.size(); c++)
					{
						if (openList[c].pNode == pNode)
						{
							if (openList[c].costSoFar <= cost )
							{
								check2 = true;
							}
							else
							{
								openList.erase(openList.begin() + c);
							}
						}
					}
					
					NodeRecord newNodeRecord{ pNode,connection,cost,cost + GetHeuristicCost(pNode,pGoalNode) };
					if(!check1&&!check2)
						openList.push_back(newNodeRecord);
				}
			}
			for (size_t i = 0; i < openList.size(); i++)
			{
				if (openList[i] == currentRecord)
				{
					openList.erase(openList.begin() + i);
					closedList.push_back(currentRecord);

				}
			}

		}
		bool hasPath{ false };
		for (const NodeRecord& record : openList)
		{
			if (record.pNode->GetIndex() == pGoalNode->GetIndex())
			{
				hasPath = true;
			}
		}
		if (!hasPath)
		{
			return path;
		}
		closedList.push_back(currentRecord);
		closedList.erase(closedList.begin());
		bool work {true};
		while (work)
		{
			T_NodeType* pNode = m_pGraph->GetNode(currentRecord.pConnection->GetFrom());
			size_t pathSize = path.size();
			for (size_t i = 0; i < closedList.size(); i++)
			{
				if (closedList[i].pNode == pNode)
				{
					path.push_back(currentRecord.pNode);
					currentRecord = closedList[i];
					closedList.erase(closedList.begin() + i);
					i = closedList.size();
				}
			}
			if (pathSize == path.size())
			{
				path.push_back(currentRecord.pNode);
				work = false;
			}
		
			
		}
		path.push_back(pStartNode);
		reverse(path.begin(), path.end());
		return path;
	}

	template <class T_NodeType, class T_ConnectionType>
	float Elite::AStar<T_NodeType, T_ConnectionType>::GetHeuristicCost(T_NodeType* pStartNode, T_NodeType* pEndNode) const
	{
		Vector2 toDestination = m_pGraph->GetNodePos(pEndNode) - m_pGraph->GetNodePos(pStartNode);
		return m_HeuristicFunction(abs(toDestination.x), abs(toDestination.y));
	}
}