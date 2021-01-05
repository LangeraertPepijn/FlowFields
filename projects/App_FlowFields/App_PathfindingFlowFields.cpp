//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

//Includes
#include "App_PathfindingFlowFields.h"
#include "framework\EliteAI\EliteGraphs\EliteGraphAlgorithms\EAstar.h"
#include "framework\EliteAI\EliteGraphs\EliteGraphAlgorithms\EBFS.h"


using namespace Elite;

App_PathfindingFlowFields::App_PathfindingFlowFields()
	: m_pColliders{}
{
	for (int i = 0; i < m_NoFlowFields; i++)
	{
		m_FlowFieldsPtrs.push_back(new FlowField(COLUMNS, ROWS));
	}
	m_pCollidersBorder.push_back(new NavigationColliderElement(Elite::Vector2{ 75,-1.15f }, 150, 0.75f));
	m_pCollidersBorder.push_back(new NavigationColliderElement(Elite::Vector2{ 75,150.33f }, 150, 0.75f));
	m_pCollidersBorder.push_back(new NavigationColliderElement(Elite::Vector2{ -1.15f,37.5f }, 0.75f,75));
	m_pCollidersBorder.push_back(new NavigationColliderElement(Elite::Vector2{ 300.33f,37.5f }, 0.75f,75));

	m_Colors.push_back(Elite::Color{ 1,0,0 });
	m_Colors.push_back(Elite::Color{ 0,1,0 });
	m_Colors.push_back(Elite::Color{ 0,0,1 });
	for (int i = 0; i < m_NoAgents; i++)
	{
		m_pBaseAgents.push_back(new BaseAgent{});
		m_pBaseAgents[i]->SetPosition(  Elite::Vector2((m_StartPosition.x+m_SizeCell/2.f)+i%5, (m_StartPosition.y + m_SizeCell / 2.f)+i%2));
		m_pBaseAgents[i]->SetRotation(0);
		int randomIndex = Elite::randomInt(m_NoFlowFields);
		m_pBaseAgents[i]->SetFlowFieldIndex(randomIndex);
		m_pBaseAgents[i]->SetBodyColor(m_Colors[randomIndex]);
	}
}

//Destructor
App_PathfindingFlowFields::~App_PathfindingFlowFields()
{
	SAFE_DELETE(m_pGridGraph);
	for (int i = 0; i < m_NoAgents; i++)
	{
		SAFE_DELETE( m_pBaseAgents[i]);
	}
	for (size_t i = 0; i < m_pColliders.size(); i++)
	{
		SAFE_DELETE(m_pColliders[i]);
	}
	for (size_t i = 0; i < m_pCollidersBorder.size(); i++)
	{
		SAFE_DELETE(m_pCollidersBorder[i]);
	}
	for (size_t i = 0; i < m_NoFlowFields; i++)
	{
		SAFE_DELETE(m_FlowFieldsPtrs[i]);
	}
}

//Functions
void App_PathfindingFlowFields::Start()
{
	//Set Camera
	DEBUGRENDERER2D->GetActiveCamera()->SetZoom(39.0f);
	DEBUGRENDERER2D->GetActiveCamera()->SetCenter(Elite::Vector2(73.0f, 35.0f));
	//DEBUGRENDERER2D->GetActiveCamera()->SetMoveLocked(true);
	//DEBUGRENDERER2D->GetActiveCamera()->SetZoomLocked(true);

	//Create Graph
	MakeGridGraph();

	m_EndPathIdices.push_back(4);
	m_EndPathIdices.push_back(0);
	m_EndPathIdices.push_back(50);


}

void App_PathfindingFlowFields::Update(float deltaTime)
{
	UNREFERENCED_PARAMETER(deltaTime);
	int t{};
	for (int i = 0; i < m_NoAgents; i++)
	{
		//Elite::Vector2  pos = m_SizeCell * m_pGridGraph->GetNodePos(m_pGridGraph->GetNodeFromWorldPos(m_TargetPosition));
		//pos += Elite::Vector2(m_SizeCell / 2.f,m_SizeCell/2.f);
		//

		//bool isNotAtEnd{false};

		//if ((pos.x - 2 + 4 < m_pBaseAgents[i]->GetPosition().x || pos.x - 2 > m_pBaseAgents[i]->GetPosition().x)&& (pos.y - 2 + 4 < m_pBaseAgents[i]->GetPosition().y || pos.y - 2 > m_pBaseAgents[i]->GetPosition().y))
		//	isNotAtEnd = true;
		//if(!isNotAtEnd)
		int flowFieldIndex = m_pBaseAgents[i]->GetFlowFieldIndex();
		int agentIndex = m_pGridGraph->GetNodeFromWorldPos(m_pBaseAgents[i]->GetPosition());
		if(m_EndPathIdices[flowFieldIndex] != agentIndex&& agentIndex>=0)
			m_pBaseAgents[i]->SetLinearVelocity(10.f * m_FlowFieldsPtrs[flowFieldIndex]->GetDirectionAt(m_pGridGraph->GetNodeFromWorldPos(m_pBaseAgents[i]->GetPosition())));

			
		float angle = acos(Dot(Elite::Vector2(0, 1), m_pBaseAgents[i]->GetLinearVelocity()));
		auto desiredOrientation = Elite::GetOrientationFromVelocity(m_pBaseAgents[i]->GetLinearVelocity());
		m_pBaseAgents[i]->SetRotation(desiredOrientation);
		m_pBaseAgents[i]->Update(deltaTime);
	}
	 
	//INPUT
	bool const middleMousePressed = INPUTMANAGER->IsMouseButtonUp(InputMouseButton::eMiddle);
	if (middleMousePressed)
	{
		MouseData mouseData = { INPUTMANAGER->GetMouseData(Elite::InputType::eMouseButton, Elite::InputMouseButton::eMiddle) };
		Elite::Vector2 mousePos = DEBUGRENDERER2D->GetActiveCamera()->ConvertScreenToWorld({ (float)mouseData.X, (float)mouseData.Y });

		//Find closest node to click pos
		int closestNode = m_pGridGraph->GetNodeFromWorldPos(mousePos);

		m_EndPathIdices[m_EndPointToSet] = closestNode;
		m_UpdatePath = true; 
		//if (m_StartSelected)
		//{
		//	m_EndPathIdices[1] = closestNode;
		//	m_UpdatePath = true;
		//}
		//else
		//{
		//	
		//	m_EndPathIdices[0] = closestNode;
		//	m_UpdatePath = true;
		//}
	}

	//GRID INPUT
	bool hasGridChanged = m_GraphEditor.UpdateGraph(m_pGridGraph);
	if (hasGridChanged)
	{
		m_UpdatePath = true;
	}

	//IMGUI
	UpdateImGui();


	//CALCULATEPATH
	//If we have nodes and the target is not the startNode, find a path!
	if (m_UpdatePath )
	{

		for (size_t i = 0; i < m_pColliders.size(); i++)
		{
			SAFE_DELETE(m_pColliders[i]);

		}
		std::cout << "New Path Calculated" << std::endl;
		for (int i{}; i < COLUMNS * ROWS; i++)
		{
			TerrainType terrain = m_pGridGraph->GetNode(i)->GetTerrainType();
			

			if (terrain == TerrainType::Water)
			{
				m_pColliders.push_back(new NavigationColliderElement(Elite::Vector2{ float(m_SizeCell) * m_pGridGraph->GetNodePos(i).x + m_SizeCell / 4.f,float(m_SizeCell) * m_pGridGraph->GetNodePos(i).y + m_SizeCell / 4.f }, float(m_SizeCell / 2.f), float(m_SizeCell / 2.f)));
			}
			for (int c = 0; c < m_NoFlowFields; c++)
			{
				m_FlowFieldsPtrs[c]->SetCostAt(m_pGridGraph->GetNode(i)->GetIndex(), int(terrain));

			}
		}

		//BFS Pathfinding
		//auto pathfinder = BFS<GridTerrainNode, GraphConnection>(m_pGridGraph);

		auto pathfinder = AStar<GridTerrainNode, GraphConnection>(m_pGridGraph,m_pHeuristicFunction);
		auto calc = BFS<GridTerrainNode, GraphConnection>(m_pGridGraph);
		for (int i{}; i < m_NoFlowFields; i++)
		{
			auto endNodeFlowField = m_pGridGraph->GetNode(m_EndPathIdices[i]);
	
			m_vPath = calc.FindPath(endNodeFlowField, endNodeFlowField, m_FlowFieldsPtrs[i]);


		}
		//if (m_EndPathIdices.size() != 0)
		//{
		//	auto startNode = m_pGridGraph->GetNode(m_EndPathIdices[0]);
		//	auto endNode = m_pGridGraph->GetNode(m_EndPathIdices[0]);
		//	m_vPath = pathfinder.FindPath(startNode, endNode, m_FlowFieldsPtrs[0]);
		//}

		m_UpdatePath = false;

	}
}

void App_PathfindingFlowFields::Render(float deltaTime) const
{
	UNREFERENCED_PARAMETER(deltaTime);
	//Render grid
	m_GraphRenderer.RenderGraph(
		m_pGridGraph, 
		m_bDrawGrid, 
		m_bDrawNodeNumbers, 
		m_bDrawConnections, 
		m_bDrawConnectionsCosts
	);
	
	//render agents 
	for (int i = 0; i < m_NoAgents; i++)
	{
		m_pBaseAgents[i]->Render(deltaTime);
		
	}


	//Render start node on top if applicable
	for (size_t i = 0; i < m_EndPathIdices.size(); i++)
	{
		if (m_EndPathIdices[i] != invalid_node_index)
		{
			switch (i)
			{
			case 0:
				m_GraphRenderer.RenderHighlightedGrid(m_pGridGraph, { m_pGridGraph->GetNode(m_EndPathIdices[i]) }, m_Colors[i]);
				break;
			case 1:
				m_GraphRenderer.RenderHighlightedGrid(m_pGridGraph, { m_pGridGraph->GetNode(m_EndPathIdices[i]) }, m_Colors[i]);
				break; 
			case 2:
				m_GraphRenderer.RenderHighlightedGrid(m_pGridGraph, { m_pGridGraph->GetNode(m_EndPathIdices[i]) }, m_Colors[i]);
				break;
			default:
				m_GraphRenderer.RenderHighlightedGrid(m_pGridGraph, { m_pGridGraph->GetNode(m_EndPathIdices[i]) }, START_NODE_COLOR);
				break;
			}
		
		}
	}
	//Render end node on top if applicable
	//if (endPathIdx != invalid_node_index)
	//{
	//	
	//	m_GraphRenderer.RenderHighlightedGrid(m_pGridGraph, { m_pGridGraph->GetNode(endPathIdx) }, END_NODE_COLOR);
	//}
	
	//render path below if applicable
	//if (m_vPath.size() > 0)
	//{
	//	
	//	m_GraphRenderer.RenderHighlightedGrid(m_pGridGraph, m_vPath);
	//}
	int i{};
	if (m_bDrawVectors)
	{

		for (const auto& node : m_pGridGraph->GetAllNodes())
		{

			Elite::Vector2 pos{ float(m_SizeCell) * m_pGridGraph->GetNodePos(node) };
			pos += {m_SizeCell / 2.f, m_SizeCell / 2.f};

			Elite::Vector2 dir = m_FlowFieldsPtrs[m_SelectedEndPos]->GetDirAt(node->GetIndex());
			DEBUGRENDERER2D->DrawDirection(pos, dir, 5.f, m_Colors[m_SelectedEndPos],0.1f);

			i++;
		}
	}
}

void App_PathfindingFlowFields::MakeGridGraph()
{
	m_pGridGraph = new GridGraph<GridTerrainNode, GraphConnection>(
		COLUMNS
		, ROWS
		, m_SizeCell
		, false// directional or not
		, true // allow diagonal connectionsc
		, 1.f //default cost perpendicualr connections
		, 1.5f); // default const for diagonal connections
	//m_pGridGraph->IsolateNode(6);
	m_pGridGraph->GetNode(7)->SetTerrainType(TerrainType::Mud);
	m_pGridGraph->GetNode(61)->SetTerrainType(TerrainType::Mud);
	m_pGridGraph->GetNode(62)->SetTerrainType(TerrainType::Mud);
	m_pGridGraph->GetNode(82)->SetTerrainType(TerrainType::Mud);
	m_pGridGraph->GetNode(83)->SetTerrainType(TerrainType::Mud);
	m_pGridGraph->GetNode(84)->SetTerrainType(TerrainType::Mud);
	m_pGridGraph->GetNode(85)->SetTerrainType(TerrainType::Mud);
	m_pGridGraph->GetNode(29)->SetTerrainType(TerrainType::Mud);
	m_pGridGraph->GetNode(48)->SetTerrainType(TerrainType::Water);
	m_pGridGraph->GetNode(87)->SetTerrainType(TerrainType::Water);
	m_pGridGraph->GetNode(27)->SetTerrainType(TerrainType::Water);
	m_pGridGraph->GetNode(47)->SetTerrainType(TerrainType::Water);
	m_pGridGraph->GetNode(67)->SetTerrainType(TerrainType::Water);
	m_pGridGraph->GetNode(194)->SetTerrainType(TerrainType::Mud);
	m_pGridGraph->GetNode(196)->SetTerrainType(TerrainType::Mud);
	m_pGridGraph->GetNode(134)->SetTerrainType(TerrainType::Mud);
	m_pGridGraph->GetNode(151)->SetTerrainType(TerrainType::Water);
	m_pGridGraph->GetNode(199)->SetTerrainType(TerrainType::Water);
	m_pGridGraph->GetNode(117)->SetTerrainType(TerrainType::Water);
}

void App_PathfindingFlowFields::UpdateImGui()
{
#ifdef PLATFORM_WINDOWS
#pragma region UI
	//UI
	{
		//Setup
		int menuWidth = 115;
		int const width = DEBUGRENDERER2D->GetActiveCamera()->GetWidth();
		int const height = DEBUGRENDERER2D->GetActiveCamera()->GetHeight();
		bool windowActive = true;
		ImGui::SetNextWindowPos(ImVec2((float)width - menuWidth - 10, 10));
		ImGui::SetNextWindowSize(ImVec2((float)menuWidth, (float)height - 20));
		ImGui::Begin("Gameplay Programming", &windowActive, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
		ImGui::PushAllowKeyboardFocus(false);

		//Elements
		ImGui::Text("CONTROLS");
		ImGui::Indent();
		ImGui::Text("LMB: target");
		ImGui::Text("RMB: start");
		ImGui::Unindent();

		/*Spacing*/ImGui::Spacing();ImGui::Separator();ImGui::Spacing();ImGui::Spacing();

		ImGui::Text("STATS");
		ImGui::Indent();
		ImGui::Text("%.3f ms/frame", 1000.0f / ImGui::GetIO().Framerate);
		ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
		ImGui::Unindent();

		/*Spacing*/ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing(); ImGui::Spacing();

		ImGui::Text("Flowfields Pathfinding");
		ImGui::Spacing();

		ImGui::Text("Middle Mouse");
		ImGui::Text("controls");
		std::string buttonText{""};

		if (ImGui::Combo("PositionToChange", &m_EndPointToSet, "Red\0Green\0Blue", 2))
		{

		}

		//if (m_StartSelected)
		//	buttonText += "Node 1";
		//else
		//	buttonText += "Node 0";

		//if (ImGui::Button(buttonText.c_str()))
		//{
		//	m_StartSelected = !m_StartSelected;
		//}

		ImGui::Checkbox("Grid", &m_bDrawGrid);
		ImGui::Checkbox("NodeNumbers", &m_bDrawNodeNumbers);
		ImGui::Checkbox("Connections", &m_bDrawConnections);
		ImGui::Checkbox("Connections Costs", &m_bDrawConnectionsCosts);
		ImGui::Checkbox("Directions", &m_bDrawVectors);
		//if (ImGui::Combo("", &m_SelectedHeuristic, "Manhattan\0Euclidean\0SqrtEuclidean\0Octile\0Chebyshev", 4))
		//{
		//	switch (m_SelectedHeuristic)
		//	{
		//	case 0:
		//		m_pHeuristicFunction = HeuristicFunctions::Manhattan;
		//		break;
		//	case 1:
		//		m_pHeuristicFunction = HeuristicFunctions::Euclidean;
		//		break;
		//	case 2:
		//		m_pHeuristicFunction = HeuristicFunctions::SqrtEuclidean;
		//		break;
		//	case 3:
		//		m_pHeuristicFunction = HeuristicFunctions::Octile;
		//		break;
		//	case 4:
		//		m_pHeuristicFunction = HeuristicFunctions::Chebyshev;
		//		break;
		//	default:
		//		m_pHeuristicFunction = HeuristicFunctions::Chebyshev;
		//		break;
		//	}
		//}
		if (ImGui::Combo("FlowFieldToShow", &m_SelectedEndPos, "Red\0Green\0Blue", 2))
		{
	
		}
		ImGui::Spacing();

		//End
		ImGui::PopAllowKeyboardFocus();
		ImGui::End();
	}
#pragma endregion
#endif
}
