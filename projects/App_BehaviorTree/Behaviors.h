/*=============================================================================*/
// Copyright 2020-2021 Elite Engine
/*=============================================================================*/
// Behaviors.h: Implementation of certain reusable behaviors for the BT version of the Agario Game
/*=============================================================================*/
#ifndef ELITE_APPLICATION_BEHAVIOR_TREE_BEHAVIORS
#define ELITE_APPLICATION_BEHAVIOR_TREE_BEHAVIORS
//-----------------------------------------------------------------
// Includes & Forward Declarations
//-----------------------------------------------------------------
#include "framework/EliteMath/EMath.h"
#include "framework/EliteAI/EliteDecisionMaking/EliteBehaviorTree/EBehaviorTree.h"
#include "../Shared/Agario/AgarioAgent.h"
#include "../Shared/Agario/AgarioFood.h"
#include "../App_Steering/SteeringBehaviors.h"

//-----------------------------------------------------------------
// Behaviors
//-----------------------------------------------------------------
bool IsCloseToFood(Elite::Blackboard* pBlackboard)
{
	AgarioAgent* pAgent = nullptr;
	std::vector<AgarioFood*>* foodVec = nullptr;

	auto dataAvailable = pBlackboard->GetData("Agent", pAgent) &&
		pBlackboard->GetData("FoodVec", foodVec);

	if (!pAgent || !foodVec)
		return false;

	//TODO: Check for food closeby and set target accordingly
	const float closeToRange{ 20.f };
	auto foodIt = std::find_if(foodVec->begin(), foodVec->end(), [&pAgent, &closeToRange](AgarioFood* f)
		{
			//true if close to agent
			return DistanceSquared(pAgent->GetPosition(), f->GetPosition()) < (closeToRange * closeToRange);
		});
	if (foodIt != foodVec->end())
	{
		pBlackboard->ChangeData("Target", (*foodIt)->GetPosition());
		return true;
	}
	return false;
}
bool IsBiggerEnemyClose(Elite::Blackboard* pBlackboard)
{
	AgarioAgent* pAgent = nullptr;
	std::vector<AgarioAgent*>* agentsVec = nullptr;

	auto dataAvailable = pBlackboard->GetData("Agent", pAgent) &&
		pBlackboard->GetData("AgentsVec", agentsVec);

	if (!pAgent || !agentsVec)
		return false;

	//TODO: Check for food closeby and set target accordingly
	const float closeToRange{ 35.f };
	auto agentIt = std::find_if(agentsVec->begin(), agentsVec->end(), [&pAgent, &closeToRange](AgarioAgent* f)
		{
			//true if close to agent
			if (pAgent->GetRadius() < f->GetRadius())
			{
				return DistanceSquared(pAgent->GetPosition(), f->GetPosition()) < (closeToRange * closeToRange);
			}
			return false;
		});
	if (agentIt != agentsVec->end())
	{


		pBlackboard->ChangeData("Target", (*agentIt)->GetPosition());
		return true;

	}
	return false;
}

BehaviorState ChangeToFlee(Elite::Blackboard* pBlackboard)
{
	AgarioAgent* pAgent = nullptr;
	Vector2 fleeTarget{  };
	auto dataAvailable = pBlackboard->GetData("Agent", pAgent)&& pBlackboard->GetData("Target",fleeTarget);

	if (!pAgent || !dataAvailable)
		return Failure;
	std::cout << "Fleeing..." << std::endl;
	pAgent->SetToFlee(fleeTarget);

	return Success;
}

BehaviorState ChangeToWander(Elite::Blackboard* pBlackboard)
{
	AgarioAgent* pAgent = nullptr;
	auto dataAvailable = pBlackboard->GetData("Agent", pAgent);

	if (!pAgent)
		return Failure;
	std::cout << "Wandering..." << std::endl;
	pAgent->SetToWander();

	return Success;
}

BehaviorState ChangeToSeek(Elite::Blackboard* pBlackboard)
{
	AgarioAgent* pAgent = nullptr;
	Vector2 seekTarget{  };
	auto dataAvailable = pBlackboard->GetData("Agent", pAgent)&&
		pBlackboard->GetData("Target",seekTarget);

	if (!pAgent)
		return Failure;
	
	//TODO: Implement Change to seek (Target)
	std::cout << "Seeking..." << std::endl;
	pAgent->SetToSeek(seekTarget);

	return Success;
}

#endif