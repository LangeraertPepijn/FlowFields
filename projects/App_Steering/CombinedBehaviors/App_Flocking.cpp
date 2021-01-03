//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

//Includes
#include "App_Flocking.h"
#include "../SteeringAgent.h"
#include "TheFlock.h"
#include "CombinedSteeringBehaviors.h"

//Destructor
App_Flocking::~App_Flocking()
{	
	delete m_pFlock;
	
	delete m_pCohesion;
	delete m_pSeparation;
	delete m_pAlignment;
	delete m_pBlendedSteering;
	delete m_pPrioritySteering;
	delete m_pEvade;
	delete m_pEnemy;

	delete m_pSeek;
	delete m_pWander;
	delete m_pWanderEnemy;
}

//Functions
void App_Flocking::Start()
{
	
	m_pSeek = new Seek{};
	m_pWander = new Wander{};
	m_pWanderEnemy = new Wander{};
	m_pEnemy = new SteeringAgent{};

	m_pEvade = new Evade();
	m_pEnemy->SetBodyColor({ 1,0,0 });

	m_pEnemy->SetAutoOrient(true);

	m_pEnemy->SetSteeringBehavior(m_pWanderEnemy);
	
	m_pFlock = new Flock( 50,100,m_pEnemy,true );
	m_pCohesion = new Cohesion(m_pFlock);
	m_pSeparation = new Separation(m_pFlock);
	m_pAlignment = new Alignment(m_pFlock);
	m_pBlendedSteering = new BlendedSteering({ {m_pCohesion,0.3f} , { m_pAlignment,0.3f },{m_pSeparation,0.3f},{m_pSeek,0.3f},{m_pWander,0.3f} });
	m_pPrioritySteering = new PrioritySteering{ {m_pEvade,m_pBlendedSteering} };
	m_pFlock->SetBlendedSteering(m_pBlendedSteering);
	m_pFlock->SetPrioritySteering(m_pPrioritySteering);
	m_pFlock->SetSteeringBehaviour();


	DEBUGRENDERER2D->GetActiveCamera()->SetZoom(55.0f);
	DEBUGRENDERER2D->GetActiveCamera()->SetCenter(Elite::Vector2(m_TrimWorldSize / 1.5f, m_TrimWorldSize / 2));
}

void App_Flocking::Update(float deltaTime)
{
	//INPUT
	if (INPUTMANAGER->IsMouseButtonUp(InputMouseButton::eLeft) && m_VisualizeMouseTarget)
	{
		auto const mouseData = INPUTMANAGER->GetMouseData(InputType::eMouseButton, InputMouseButton::eLeft);
		m_MouseTarget.Position = DEBUGRENDERER2D->GetActiveCamera()->ConvertScreenToWorld({ static_cast<float>(mouseData.X), static_cast<float>(mouseData.Y) });
		
	}
	m_pCohesion->SetTarget(m_MouseTarget);
	m_pSeparation->SetTarget(m_MouseTarget);
	m_pAlignment->SetTarget(m_MouseTarget);
	
	
	m_pEvade->SetTarget(m_pEnemy->GetPosition());
	m_pSeek->SetTarget(m_MouseTarget);
	m_pEnemy->Update(deltaTime);
	m_pEnemy->TrimToWorld({ 0,0 }, {m_TrimWorldSize,m_TrimWorldSize});
	//m_pEnemy->SetPosition({ 1000000,100000 }); //test only
	m_pFlock->Update(deltaTime);
}

void App_Flocking::Render(float deltaTime) const
{
	m_pFlock->Render(deltaTime);
	m_pFlock->UpdateAndRenderUI(m_pBlendedSteering);
	m_pEnemy->Render(deltaTime);

	
	std::vector<Elite::Vector2> points =
	{
		{ 0,m_TrimWorldSize },
		{ m_TrimWorldSize,m_TrimWorldSize },
		{ m_TrimWorldSize,0 },
		{0,0 }
	};
	DEBUGRENDERER2D->DrawPolygon(&points[0], 4, { 1,0,0,1 }, 0.4f);

	//Render Target
	if(m_VisualizeMouseTarget)
		DEBUGRENDERER2D->DrawSolidCircle(m_MouseTarget.Position, 0.3f, { 0.f,0.f }, { 1.f,0.f,0.f },-0.8f);
}
