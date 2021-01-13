#ifndef ASTAR_APPLICATION_H
#define ASTAR_APPLICATION_H
//-----------------------------------------------------------------
// Includes & Forward Declarations
//-----------------------------------------------------------------
#include "framework/EliteInterfaces/EIApp.h"
#include "framework\EliteAI\EliteGraphs\EGridGraph.h"
#include "framework\EliteAI\EliteGraphs\EliteGraphUtilities\EGraphEditor.h"
#include "framework\EliteAI\EliteGraphs\EliteGraphUtilities\EGraphRenderer.h"
#include "projects/App_FlowFields/FlowField.h"
#include "projects/Shared/BaseAgent.h"
#include "projects/Shared/NavigationColliderElement.h"



//-----------------------------------------------------------------
// Application
//-----------------------------------------------------------------
class App_PathfindingFlowFields final : public IApp
{
public:
	//Constructor & Destructor
	App_PathfindingFlowFields() ;
	virtual ~App_PathfindingFlowFields();

	//App Functions
	void Start() override;
	void Update(float deltaTime) override;
	void Render(float deltaTime) const override;

private:
	//Datamembers
	const bool ALLOW_DIAGONAL_MOVEMENT = true;
	Elite::Vector2 m_SpawnPosition = Elite::ZeroVector2;
	Elite::Vector2 m_TargetPosition = Elite::ZeroVector2;

	//Grid datamembers
	static const int COLUMNS = 20;
	static const int ROWS = 10;
	unsigned int m_SizeCell = 15;
	Elite::GridGraph<Elite::GridTerrainNode, Elite::GraphConnection>* m_pGridGraph;


	//Pathfinding datamembers
	std::vector<int> m_StartPathIdices{};
	std::vector<int> m_EndPathIdices{};
	int m_EndPointToSet {};
	int m_StartPointToSet {};
	std::vector<Elite::GridTerrainNode*> m_vPath;
	bool m_UpdatePath = true;

	//Editor and Visualisation
	Elite::EGraphEditor m_GraphEditor{};
	Elite::EGraphRenderer m_GraphRenderer{};

	//Debug rendering information
	bool m_bDrawGrid = true;
	bool m_bDrawVectors = true;
	bool m_bDrawNodeNumbers = false;
	bool m_bDrawConnections = false;
	bool m_bDrawConnectionsCosts = false;
	bool m_SpawnSelected = false;
	int m_SelectedHeuristic = 4;
	int m_SelectedEndPos = 0;
	Elite::Heuristic m_pHeuristicFunction = Elite::HeuristicFunctions::Chebyshev;

	//Functions
	void MakeGridGraph();
	void UpdateImGui();

	//FlowFieldMembers
	std::vector<BaseAgent* >m_pBaseAgents;
	std::vector<NavigationColliderElement* >m_pColliders;
	std::vector<NavigationColliderElement* >m_pCollidersBorder;
	const static int m_NoAgents{50};
	const static int m_NoFlowFields{3};
	std::vector<FlowField*> m_pFlowFields;
	std::vector<Elite::Color> m_Colors;


	//AgentReset
	float m_Time{0};
	float m_ResetTime{0};
	const float m_ResetTimeValue{0.3f};
	const int m_TimeForTotReset{25};
	int m_NextAgentIdx{0};
	
	//C++ make the class non-copyable
	App_PathfindingFlowFields(const App_PathfindingFlowFields&) = delete;
	App_PathfindingFlowFields& operator=(const App_PathfindingFlowFields&) = delete;
};
#endif