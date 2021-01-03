/*=============================================================================*/
// Copyright 2020-2021 Elite Engine
/*=============================================================================*/
// StatesAndTransitions.h: Implementation of the state/transition classes
/*=============================================================================*/
#ifndef ELITE_APPLICATION_FSM_STATES_TRANSITIONS
#define ELITE_APPLICATION_FSM_STATES_TRANSITIONS

#include "../Shared/Agario/AgarioAgent.h"
#include "../Shared/Agario/AgarioFood.h"
#include "../App_Steering/SteeringBehaviors.h"

//---States
//----------
class WanderState : public Elite::FSMState
{

public:
	WanderState() : FSMState() {};
	virtual void OnEnter(Blackboard* pBlackboard)override
	{
		AgarioAgent* pAgent = nullptr;
		bool dataAvailable = pBlackboard->GetData("Agent", pAgent);
		if (!dataAvailable)
			return;
		if (!pAgent)
			return;
		pAgent->SetToWander();

	}
private:

};
class PursuitState : public Elite::FSMState
{

public:
	PursuitState() : FSMState() {};
	virtual void OnEnter(Blackboard* pBlackboard)override
	{
		AgarioAgent* pAgent = nullptr;
		bool dataAvailable = pBlackboard->GetData("Agent", pAgent);
		if (!dataAvailable)
			return;
		if (!pAgent)
			return;

		std::vector<AgarioAgent*>* pEnemyVec{};
		dataAvailable = pBlackboard->GetData("EnemyVec", pEnemyVec);
		if (!dataAvailable)
			return;
		if (!pEnemyVec)
			return;
		float  distance{ 100000 };
		Elite::Vector2 agentPos{};
		
		for (AgarioAgent* enemy : *pEnemyVec)
		{
			if (enemy->GetRadius() > pAgent->GetRadius() && Distance(enemy->GetPosition(), pAgent->GetPosition()) < distance)
			{
				agentPos = enemy->GetPosition();
				distance = Distance(enemy->GetPosition(), pAgent->GetPosition());

			}
		}
		pAgent->SetToFlee(agentPos);
		pBlackboard->ChangeData("AgentToChase", agentPos);

	}

private:
	
};
class EvadeState : public Elite::FSMState
{

public:
	EvadeState() : FSMState() {};
	virtual void OnEnter(Blackboard* pBlackboard)override
	{
		AgarioAgent* pAgent = nullptr;
		bool dataAvailable = pBlackboard->GetData("Agent", pAgent);
		if (!dataAvailable)
			return;
		if (!pAgent)
			return;

		std::vector<AgarioAgent*>* pEnemyVec{};
		dataAvailable = pBlackboard->GetData("EnemyVec", pEnemyVec);
		if (!dataAvailable)
			return;
		if (!pEnemyVec)
			return;
		float  distance{ 100000 };

		for (AgarioAgent* enemy : *pEnemyVec)
		{
			if (enemy->GetRadius() > pAgent->GetRadius() && Distance(enemy->GetPosition(), pAgent->GetPosition()) <  distance)
			{
				m_PosToEvade= enemy->GetPosition();
				distance = Distance(enemy->GetPosition(), pAgent->GetPosition());
			
			
			}
		}
		pAgent->SetToFlee(m_PosToEvade);
		pBlackboard->ChangeData("EnemyToFlee", m_PosToEvade);


	}

private:
	Vector2 m_PosToEvade{};

};
class SeekFoodState : public Elite::FSMState
{

public:
	SeekFoodState() : FSMState(){};
	virtual void OnEnter(Blackboard* pBlackboard)override
	{
		AgarioAgent* pAgent = nullptr;
		bool dataAvailable = pBlackboard->GetData("Agent", pAgent);
		if (!dataAvailable)
			return;
		if (!pAgent)
			return;

		std::vector<AgarioFood*>* pFoodVec{};
		dataAvailable = pBlackboard->GetData("FoodVec", pFoodVec);
		if (!dataAvailable)
			return;
		if (!pFoodVec)
			return;
		float distance{ 100000 };
		for (AgarioFood* food : *pFoodVec)
		{

			if (Distance(pAgent->GetPosition(), food->GetPosition()) <  distance)
			{
				distance = Distance(pAgent->GetPosition(), food->GetPosition());
				m_PosToSeek = food->GetPosition();
			}

		}

		pAgent->SetToSeek(m_PosToSeek);
		pBlackboard->ChangeData("PosToSeek", m_PosToSeek);


	}


	virtual void Update(Blackboard* pBlackboard, float deltaTime)
	{
	};
private:
	Vector2 m_PosToSeek{};


};


//Transitions
//-------------------
class HasFoodCloseby : public Elite::FSMTransition
{
public:
	float m_Distance{ 15 };
	virtual bool ToTransition(Blackboard* pBlackBoard)const override
	{
		//get our agent from the blackboard
		AgarioAgent* pAgent = nullptr;
		pBlackBoard->GetData("Agent", pAgent);
		//Get our food information from the blackboard
		std::vector<AgarioFood*>* pFoodVec = nullptr;
		pBlackBoard->GetData("FoodVec", pFoodVec);
		//chack if food is closeby(within radius)
		for (AgarioFood* food : *pFoodVec)
		{
			if (Distance(pAgent->GetPosition(), food->GetPosition()) < m_Distance)
			{
				//return true food is closeby
				return true;
			}
		}
		//return false if foodis not closeby
		return false;
	}
};
class HasReachedFood : public Elite::FSMTransition
{
public:
	virtual bool ToTransition(Blackboard* pBlackBoard)const override
	{
		//get our agent from the blackboard
		AgarioAgent* pAgent = nullptr;
		pBlackBoard->GetData("Agent", pAgent);
		//Get our food information from the blackboard
		Vector2 pos{};
		pBlackBoard->GetData("PosToSeek", pos);
		//chack if food is closeby(within radius)

		
		if (Distance(pAgent->GetPosition(), pos) < pAgent->GetRadius())
		{
			return true;
		}
		
		//return false if foodis not closeby
		return false;
	}
};
class BigEnemyIsClose : public Elite::FSMTransition
{
public:
	float m_Distance{ 40 };
	virtual bool ToTransition(Blackboard* pBlackBoard)const override
	{
		//get our agent from the blackboard
		AgarioAgent* pAgent = nullptr;
		pBlackBoard->GetData("Agent", pAgent);
		//Get our food information from the blackboard
		std::vector<AgarioAgent*>* pEnemyVec{};
		pBlackBoard->GetData("EnemyVec", pEnemyVec);
		//chack if food is closeby(within radius)

		for (AgarioAgent* enemy : *pEnemyVec)
		{
			if ((enemy->GetRadius() > pAgent->GetRadius())&& Distance(enemy->GetPosition(),pAgent->GetPosition())<m_Distance)
			{
				//check
				//std::cout << Distance(enemy->GetPosition(), pAgent->GetPosition()) << std::endl;
				//std::cout << enemy->GetPosition() << std::endl;
				return true;
			}
		}

		//return false if foodis not closeby
		return false;
	}
};
class SmalEnemyIsClose : public Elite::FSMTransition
{
public:
	float m_Distance{ 25 };
	virtual bool ToTransition(Blackboard* pBlackBoard)const override
	{
		//get our agent from the blackboard
		AgarioAgent* pAgent = nullptr;
		pBlackBoard->GetData("Agent", pAgent);
		//Get our food information from the blackboard
		std::vector<AgarioAgent*>* pEnemyVec{};
		pBlackBoard->GetData("EnemyVec", pEnemyVec);
		//chack if food is closeby(within radius)

		for (AgarioAgent* enemy : *pEnemyVec)
		{
			if ((enemy->GetRadius() < pAgent->GetRadius()) && Distance(enemy->GetPosition(), pAgent->GetPosition()) < m_Distance)
			{

				return true;
			}
		}

		//return false if foodis not closeby
		return false;
	}
};
class HasFled : public Elite::FSMTransition
{
public:
	virtual bool ToTransition(Blackboard* pBlackBoard)const override
	{
		//get our agent from the blackboard
		AgarioAgent* pAgent = nullptr;
		pBlackBoard->GetData("Agent", pAgent);
		//Get our food information from the blackboard
		Elite::Vector2 enemy{};
		pBlackBoard->GetData("EnemyToFlee", enemy);
		//chack if food is closeby(within radius)

		//std::cout << Distance(pAgent->GetPosition(), pos) << std::endl;


		if (Distance(pAgent->GetPosition(), enemy) > 41)
		{
			return true;
		}



		//return false if foodis not closeby
		return false;
	}
};
class HasKilled : public Elite::FSMTransition
{
public:
	virtual bool ToTransition(Blackboard* pBlackBoard)const override
	{
		//get our agent from the blackboard
		AgarioAgent* pAgent = nullptr;
		pBlackBoard->GetData("Agent", pAgent);
		//Get our food information from the blackboard
		
		Elite::Vector2 enemyTarget  {};
		pBlackBoard->GetData("AgentToChase", enemyTarget);
		//chack if food is closeby(within radius)


		if (Distance(pAgent->GetPosition(), enemyTarget) < pAgent->GetRadius())
		{
			return true;
		}

		//return false if foodis not closeby
		return false;
	}
};

#endif