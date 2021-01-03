#include "stdafx.h"
#include "App_AgarioGame.h"
#include "StatesAndTransitions.h"
#include "../Shared/Agario/AgarioFood.h"
#include "../Shared/Agario/AgarioAgent.h"
#include "../Shared/Agario/AgarioContactListener.h"

using namespace Elite;
App_AgarioGame::App_AgarioGame()
{
}

App_AgarioGame::~App_AgarioGame()
{
	for (auto& f : m_pFoodVec)
	{
		SAFE_DELETE(f);
	}
	m_pFoodVec.clear();

	for (auto& a : m_pAgentVec)
	{
		SAFE_DELETE(a);
	}
	m_pAgentVec.clear();

	SAFE_DELETE(m_pContactListener);
	for (auto& s : m_pStates)
	{
		SAFE_DELETE(s);
	}

	for (auto& t : m_pTransitions)
	{
		SAFE_DELETE(t);
	}
	SAFE_DELETE(m_pUberAgent);

}

void App_AgarioGame::Start()
{
	//Create common state
	WanderState* pWanderState = new WanderState{};
	SeekFoodState* pSeekFoodState = new SeekFoodState{};
	EvadeState* pEvadeState = new EvadeState{};
	PursuitState* pPursuitState = new PursuitState{};
	m_pStates.push_back(pWanderState);
	m_pStates.push_back(pSeekFoodState);
	m_pStates.push_back(pEvadeState);
	m_pStates.push_back(pPursuitState);
	//Create food items
	m_pFoodVec.reserve(m_AmountOfFood);
	for (int i = 0; i < m_AmountOfFood; i++)
	{
		Elite::Vector2 randomPos = randomVector2(-m_TrimWorldSize, m_TrimWorldSize);
		m_pFoodVec.push_back(new AgarioFood(randomPos));
	}

	//Create dumb agents
	m_pAgentVec.reserve(m_AmountOfAgents);
	for (int i = 0; i < m_AmountOfAgents; i++)
	{
		Elite::Vector2 randomPos = randomVector2(-m_TrimWorldSize, m_TrimWorldSize);
		AgarioAgent* newAgent = new AgarioAgent(randomPos);
		//Need to add a dumb FiniteStateMachine

		Blackboard* pBlackBoard = new Blackboard{};
		pBlackBoard->AddData("Agent", newAgent);
		FiniteStateMachine* pDumbFSM = new FiniteStateMachine{pWanderState,pBlackBoard};
		newAgent->SetDecisionMaking(pDumbFSM);
		m_pAgentVec.push_back(newAgent);
	}

	//Creating the world contact listener that informs us of collisions
	m_pContactListener = new AgarioContactListener();

	//Create Uber Agent
	Elite::Vector2 randomPos = randomVector2(-m_TrimWorldSize, m_TrimWorldSize);
	Color customColor = Color{ randomFloat(), randomFloat(), randomFloat() };
	m_pUberAgent = new AgarioAgent(randomPos, customColor);
	//Need to add a Smart FiniteStateMachine
	Blackboard* pBlackBoard = new Blackboard{};
	pBlackBoard->AddData("Agent", m_pUberAgent);
	pBlackBoard->AddData("FoodVec", &m_pFoodVec);
	pBlackBoard->AddData("PosToSeek", Vector2{0,0});
	pBlackBoard->AddData("EnemyToFlee", Vector2{ 0,0 });
	pBlackBoard->AddData("EnemyVec", &m_pAgentVec);
	pBlackBoard->AddData("AgentToChase", Vector2{ 0,0 });
	
	//creat finite stat machin with default stat
	FiniteStateMachine* pSmartFSM = new FiniteStateMachine{ pWanderState,pBlackBoard };
	//create possible transitions
	HasFoodCloseby* hasFoodCloseby= new HasFoodCloseby();
	HasReachedFood*  hasReachedFood = new HasReachedFood();
	BigEnemyIsClose* bigEnemyIsClose = new BigEnemyIsClose();
	HasFled*  hasFled = new HasFled();
	SmalEnemyIsClose* smalEnemyIsClose = new SmalEnemyIsClose();
	HasKilled* hasKilled = new HasKilled();
	m_pTransitions.push_back(hasFoodCloseby);
	m_pTransitions.push_back(hasReachedFood);
	m_pTransitions.push_back(bigEnemyIsClose);
	m_pTransitions.push_back(hasFled);
	m_pTransitions.push_back(smalEnemyIsClose);
	m_pTransitions.push_back(hasKilled);

	pSmartFSM->AddTransition(pWanderState, pSeekFoodState, hasFoodCloseby);
	pSmartFSM->AddTransition(pSeekFoodState, pWanderState, hasReachedFood);
	pSmartFSM->AddTransition(pSeekFoodState, pEvadeState, bigEnemyIsClose);
	pSmartFSM->AddTransition(pWanderState, pEvadeState, bigEnemyIsClose);
	pSmartFSM->AddTransition(pPursuitState, pEvadeState, bigEnemyIsClose);
	pSmartFSM->AddTransition(pEvadeState, pWanderState, hasFled);
	pSmartFSM->AddTransition(pWanderState, pPursuitState, smalEnemyIsClose);
	pSmartFSM->AddTransition(pPursuitState, pSeekFoodState, hasFoodCloseby);
	pSmartFSM->AddTransition(pPursuitState, pWanderState, hasKilled);

	

	

	m_pUberAgent->SetDecisionMaking(pSmartFSM);

}

void App_AgarioGame::Update(float deltaTime)
{
	UpdateImGui();

	//Check if agent is still alive
	if (m_pUberAgent->CanBeDestroyed())
	{
		m_GameOver = true;
		return;
	}
	//Update the custom agent
	m_pUberAgent->Update(deltaTime);
	m_pUberAgent->TrimToWorld(m_TrimWorldSize);

	//Update the other agents and food
	UpdateAgarioEntities(m_pFoodVec, deltaTime);
	UpdateAgarioEntities(m_pAgentVec, deltaTime);

	
	//Check if we need to spawn new food
	m_TimeSinceLastFoodSpawn += deltaTime;
	if (m_TimeSinceLastFoodSpawn > m_FoodSpawnDelay)
	{
		m_TimeSinceLastFoodSpawn = 0.f;
		m_pFoodVec.push_back(new AgarioFood(randomVector2(-m_TrimWorldSize, m_TrimWorldSize)));
	}
	
}

void App_AgarioGame::Render(float deltaTime) const
{
	std::vector<Elite::Vector2> points =
	{
		{ -m_TrimWorldSize, m_TrimWorldSize },
		{ m_TrimWorldSize, m_TrimWorldSize },
		{ m_TrimWorldSize, -m_TrimWorldSize },
		{ -m_TrimWorldSize, -m_TrimWorldSize }
	};
	DEBUGRENDERER2D->DrawPolygon(&points[0], 4, { 1,0,0,1 }, 0.4f);

	for (AgarioFood* f : m_pFoodVec)
	{
		f->Render(deltaTime);
	}

	for (AgarioAgent* a : m_pAgentVec)
	{
		a->Render(deltaTime);
	}

	m_pUberAgent->Render(deltaTime);
}

void App_AgarioGame::UpdateImGui()
{
	//------- UI --------
#ifdef PLATFORM_WINDOWS
#pragma region UI
	{
		//Setup
		int menuWidth = 150;
		int const width = DEBUGRENDERER2D->GetActiveCamera()->GetWidth();
		int const height = DEBUGRENDERER2D->GetActiveCamera()->GetHeight();
		bool windowActive = true;
		ImGui::SetNextWindowPos(ImVec2((float)width - menuWidth - 10, 10));
		ImGui::SetNextWindowSize(ImVec2((float)menuWidth, (float)height - 90));
		ImGui::Begin("Agario", &windowActive, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
		ImGui::PushAllowKeyboardFocus(false);
		ImGui::SetWindowFocus();
		ImGui::PushItemWidth(70);
		//Elements
		ImGui::Text("CONTROLS");
		ImGui::Indent();
		ImGui::Unindent();

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
		ImGui::Spacing();

		ImGui::Text("STATS");
		ImGui::Indent();
		ImGui::Text("%.3f ms/frame", 1000.0f / ImGui::GetIO().Framerate);
		ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
		ImGui::Unindent();

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
		ImGui::Spacing();
		
		ImGui::Text("Agent Info");
		ImGui::Text("Radius: %.1f",m_pUberAgent->GetRadius());
		ImGui::Text("Survive Time: %.1f", TIMER->GetTotal());
		
		//End
		ImGui::PopAllowKeyboardFocus();
		ImGui::End();
	}
	if(m_GameOver)
	{
		//Setup
		int menuWidth = 300;
		int menuHeight = 100;
		int const width = DEBUGRENDERER2D->GetActiveCamera()->GetWidth();
		int const height = DEBUGRENDERER2D->GetActiveCamera()->GetHeight();
		bool windowActive = true;
		ImGui::SetNextWindowPos(ImVec2(width/2.0f- menuWidth, height/2.0f - menuHeight));
		ImGui::SetNextWindowSize(ImVec2((float)menuWidth, (float)menuHeight));
		ImGui::Begin("Game Over", &windowActive, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
		ImGui::Text("Final Agent Info");
		ImGui::Text("Radius: %.1f", m_pUberAgent->GetRadius());
		ImGui::Text("Survive Time: %.1f", TIMER->GetTotal());
		ImGui::End();
	}
#pragma endregion
#endif

}
