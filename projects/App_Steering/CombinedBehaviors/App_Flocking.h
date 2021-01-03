#ifndef STEERINGBEHAVIORS_APPLICATION_H
#define STEERINGBEHAVIORS_APPLICATION_H
//-----------------------------------------------------------------
// Includes & Forward Declarations
//-----------------------------------------------------------------
#include "framework/EliteInterfaces/EIApp.h"
#include "../SteeringBehaviors.h"
#include "FlockingSteeringBehaviors.h"

class SteeringAgent;
class Flock;
class BlendedSteering;
class PrioritySteering;


//-----------------------------------------------------------------
// Application
//-----------------------------------------------------------------
class App_Flocking final : public IApp
{
public:
	//Constructor & Destructor
	App_Flocking() = default;
	virtual ~App_Flocking();

	//App Functions
	void Start() override;
	void Update(float deltaTime) override;
	void Render(float deltaTime) const override;
private:
	//Datamembers
	TargetData m_MouseTarget = {};
	bool m_UseMouseTarget = true;
	bool m_VisualizeMouseTarget = true;
	SteeringAgent* m_pEnemy = nullptr;

	Seek* m_pSeek = nullptr;
	Seek* m_pWander = nullptr;
	Seek* m_pWanderEnemy = nullptr;

	Flock* m_pFlock =nullptr;
	
	Cohesion* m_pCohesion=nullptr;
	Separation* m_pSeparation=nullptr;
	Alignment* m_pAlignment=nullptr;

	Evade* m_pEvade = nullptr;
	BlendedSteering* m_pBlendedSteering = nullptr;
	PrioritySteering* m_pPrioritySteering = nullptr;


	float m_TrimWorldSize = 100.f;

	//C++ make the class non-copyable
	App_Flocking(const App_Flocking&) = delete;
	App_Flocking& operator=(const App_Flocking&) = delete;
};
#endif