#pragma once
#include <stack>

namespace Elite
{
	enum class Eulerianity
	{
		notEulerian,
		semiEulerian,
		eulerian,
	};

	template <class T_NodeType, class T_ConnectionType>
	class EulerianPath
	{
	public:

		EulerianPath(IGraph<T_NodeType, T_ConnectionType>* pGraph);

		Eulerianity IsEulerian() const;
		vector<T_NodeType*> FindPath(Eulerianity& eulerianity) const;

	private:
		void VisitAllNodesDFS(int startIdx, vector<bool>& visited) const;
		bool IsConnected() const;

		IGraph<T_NodeType, T_ConnectionType>* m_pGraph;
	};

	template<class T_NodeType, class T_ConnectionType>
	inline EulerianPath<T_NodeType, T_ConnectionType>::EulerianPath(IGraph<T_NodeType, T_ConnectionType>* pGraph)
		: m_pGraph(pGraph)
	{
	}

	template<class T_NodeType, class T_ConnectionType>
	inline Eulerianity EulerianPath<T_NodeType, T_ConnectionType>::IsEulerian() const
	{
		// If the graph is not connected, there can be no Eulerian Trail
		if (IsConnected() == false)
			return Eulerianity::notEulerian;

		// Count nodes with odd degree 
		int nrOfNodes = m_pGraph->GetNrOfNodes();
		int oddCount = 0;
		for (int i{}; i < nrOfNodes; i++)
		{
			if (m_pGraph->IsNodeValid(i) && (m_pGraph->GetNodeConnections(i).size() & 1))
				oddCount++;
		}

		// A connected graph with more than 2 nodes with an odd degree (an odd amount of connections) is not Eulerian
		if (oddCount > 2)
			return Eulerianity::notEulerian;

		// A connected graph with exactly 2 nodes with an odd degree is Semi-Eulerian (an Euler trail can be made, but only starting and ending in these 2 nodes)
		else if (oddCount == 2 && nrOfNodes != 2)
			return Eulerianity::semiEulerian;

		// A connected graph with no odd nodes is Eulerian
		return Eulerianity::eulerian;

	}

	template<class T_NodeType, class T_ConnectionType>
	inline vector<T_NodeType*> EulerianPath<T_NodeType, T_ConnectionType>::FindPath(Eulerianity& eulerianity) const
	{
		// Get a copy of the graph because this algorithm involves removing edges
		auto graphCopy = m_pGraph->Clone();
		int nrOfNodes = graphCopy->GetNrOfNodes();
		auto path = vector<T_NodeType*>();
		stack<int> queue{};
		int counterUneven{};
		int indexUneven{};
		int indexEven{};
		bool start{true};
		// algorithm...
		
		for (int i = 0; i < nrOfNodes; i++)
		{
			if (graphCopy->GetNodeConnections(i).size() % 2 ==1)
			{
				counterUneven++;
				if (counterUneven > 2)
				{
					return vector<T_NodeType*>();
				}
				indexUneven = i;
			}
		}
		if (counterUneven == 0)
		{
			while(start||queue.size()!=0)
			{
				start = false;
				if (graphCopy->GetNodeConnections(indexEven).size() == 0)
				{
					path.push_back(m_pGraph->GetNode(indexEven));
					queue.pop();
					if (queue.size() > 0)
					{
						indexEven = queue.top();

					}
				}
				else
				{
					queue.push(indexEven);
					std::list<T_ConnectionType*> temp = graphCopy->GetNodeConnections(indexEven);
					T_ConnectionType* test = *temp.begin();
					indexEven =test->GetTo();
					graphCopy->RemoveConnection(*temp.begin());
				}
			}
		}
		else
		{
			while (start||queue.size() != 0)
			{
				start = false;
				if (graphCopy->GetNodeConnections(indexUneven).size() == 0)
				{
					path.push_back(m_pGraph->GetNode(indexUneven));
					queue.pop();
					if (queue.size() > 0)
					{
						indexUneven = queue.top();

					}
				}
				else
				{
					queue.push(indexUneven);
					std::list<T_ConnectionType*> temp = graphCopy->GetNodeConnections(indexUneven);
					T_ConnectionType* test = *temp.begin();
					indexUneven = test->GetTo();
					graphCopy->RemoveConnection(*temp.begin());
				}
			}
		}
		return path;
	}

	template<class T_NodeType, class T_ConnectionType>
	inline void EulerianPath<T_NodeType, T_ConnectionType>::VisitAllNodesDFS(int startIdx, vector<bool>& visited) const
	{
		// mark the visited node
		visited[startIdx] = true;
		// recursively visit any valid connected nodes that were not visited before
		for (T_ConnectionType* connection : m_pGraph->GetNodeConnections(startIdx))
		{
			if (m_pGraph->IsNodeValid(connection->GetTo()) && !visited[connection->GetTo()])
				VisitAllNodesDFS(connection->GetTo(), visited);
		}
	}

	template<class T_NodeType, class T_ConnectionType>
	inline bool EulerianPath<T_NodeType, T_ConnectionType>::IsConnected() const
	{
		int nrOfNodes = m_pGraph->GetNrOfNodes();
		vector<bool> visited(nrOfNodes,false);
		
		// find a valid starting node that has connections
		int connectedIdx = invalid_node_index;
		for (int i = 0; i < nrOfNodes; i++)
		{
			if (m_pGraph->IsNodeValid(i) && m_pGraph->GetNodeConnections(i).size() != 0)
			{
				connectedIdx = i;
				break;
			}
			else
			{
				return false;
			}
		}

		// if no valid node could be found, return false
		if (connectedIdx == invalid_node_index)
			return false;

		// start a depth-first-search traversal from a node that has connections
		VisitAllNodesDFS(connectedIdx, visited);

		// if a node was never visited, this graph is not connected
		for (int i{}; i < nrOfNodes; i++)
		{
			if (m_pGraph->IsNodeValid(i) && visited[i] == false)
				return false;
		}

		return true;
	}

}