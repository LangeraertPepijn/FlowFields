#include "stdafx.h"
#include "TheFlock.h"

#include "../SteeringAgent.h"
#include "../SteeringBehaviors.h"
#include "CombinedSteeringBehaviors.h"

using namespace Elite;

//Constructor & Destructor
Flock::Flock(
	int flockSize /*= 50*/, 
	float worldSize /*= 100.f*/, 
	SteeringAgent* pAgentToEvade /*= nullptr*/, 
	bool trimWorld /*= false*/

)

	: m_WorldSize{ worldSize }
	, m_FlockSize{ flockSize }
	, m_TrimWorld { trimWorld }
	, m_pAgentToEvade{pAgentToEvade}
	, m_NeighborhoodRadius{ 15 }
	, m_NrOfNeighbors{0}
	, m_CellSpace{worldSize,worldSize,4,4,m_FlockSize}
	, m_UsePartitioning{false}

{
	
	
	for (int i{}; i < m_FlockSize; i++)
	{
		m_Agents.push_back(new SteeringAgent{});
		const Elite::Vector2 temp{ randomFloat(0,worldSize),randomFloat(0,worldSize) };
		m_Agents[i]->SetPosition(temp);
		m_Agents[i]->SetMaxAngularSpeed(10.f);
		m_Agents[i]->SetMaxLinearSpeed(15.f);
		m_Agents[i]->SetAutoOrient(true);
		m_CellSpace.AddAgent(m_Agents[i]);
		m_OldPos.push_back(m_Agents[i]->GetPosition());
	}
}

Flock::~Flock()
{
	for (size_t i{}; i < m_Agents.size(); i++)
	{
		delete m_Agents[i];
	}
	 m_pBlendedSteering=nullptr;
	 m_pPrioritySteering=nullptr;
	m_pAgentToEvade=nullptr;

}

void Flock::Update(float deltaT)
{
	// loop over all the boids
	// register its neighbors
	// update it
	// trim it to the world
	m_pPrioritySteering->SetTarget(m_pAgentToEvade->GetPosition());
	if (m_UsePartitioning)
	{
		for (size_t i{}; i < m_Agents.size(); i++)
		{
			if (i == 49)
			{
				int j{};
			}
			m_CellSpace.UpdateAgentCell(m_Agents[i], m_OldPos[i]);
			m_CellSpace.RegisterNeighbors(m_Agents[i]->GetPosition(),m_NeighborhoodRadius);
			m_NrOfNeighbors = m_CellSpace.GetNrOfNeighbors();
			m_Neighbors = m_CellSpace.GetNeighbors();
			m_Agents[i]->Update(deltaT);
			m_OldPos[i] = m_Agents[i]->GetPosition();
			if (m_TrimWorld)
			{

				m_Agents[i]->TrimToWorld({ 0,0 }, { m_WorldSize,m_WorldSize });

			}

		}
		//DEBUGRENDERER2D->DrawCircle(m_Agents[49]->GetPosition(), m_NeighborhoodRadius, Elite::Color{ 1,0,0 }, 0);
		//for (int i = 0; i < m_NrOfNeighbors; i++)
		//{

		//	DEBUGRENDERER2D->DrawPoint(m_Neighbors[i]->GetPosition(), 5, Elite::Color{ 0,1,0 }, 0);
		//}

	}
	else
	{


		for (size_t i{}; i < m_Agents.size(); i++)
		{
			RegisterNeighbors(m_Agents[i]);
			m_Agents[i]->Update(deltaT);
			if (m_TrimWorld)
			{

				m_Agents[i]->TrimToWorld({ 0,0 }, { m_WorldSize,m_WorldSize });

			}

		}
		//DEBUGRENDERER2D->DrawCircle(m_Agents[49]->GetPosition(), m_NeighborhoodRadius, Elite::Color{ 1,0,0 }, 0);
		//for (int i = 0; i < m_NrOfNeighbors; i++)
		//{

		//	DEBUGRENDERER2D->DrawPoint(m_Neighbors[i]->GetPosition(), 5, Elite::Color{ 0,1,0 }, 0);
		//}
	}
	
	
}



void Flock::Render(float deltaT)
{
	for (size_t i{}; i < m_Agents.size(); i++)
	{
		m_Agents[i]->Render(deltaT);
		

	}
	m_CellSpace.RenderCells();
}



void Flock::UpdateAndRenderUI(BlendedSteering* m_pBlendedSteering)
{
	//Setup
	int menuWidth = 235;
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
	ImGui::Text("LMB: place target");
	ImGui::Text("RMB: move cam.");
	ImGui::Text("Scrollwheel: zoom cam.");
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

	ImGui::Text("Flocking");
	ImGui::Spacing();

	// Implement checkboxes and sliders here
	ImGui::Checkbox("Trim World", &m_TrimWorld);
	if (m_TrimWorld)
	{
		ImGui::SliderFloat("Trim Size", &m_WorldSize, 0.f, 500.f, "%1.");
	}
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Checkbox("RenderPartitions", &m_CellSpace.GetRenderActivity());
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Checkbox("UsePartitions", &m_UsePartitioning);
	ImGui::Spacing();
	ImGui::Spacing();



	ImGui::Text("Behavior Weighs");
	ImGui::Spacing();
	ImGui::SliderFloat("Cohesion", &m_pBlendedSteering->m_WeightedBehaviors[0].weight, 0.f, 1.f, "%.2");
	ImGui::SliderFloat("Alignment", &m_pBlendedSteering->m_WeightedBehaviors[1].weight, 0.f, 1.f, "%.2");
	ImGui::SliderFloat("Separation", &m_pBlendedSteering->m_WeightedBehaviors[2].weight, 0.f, 1.f, "%.2");
	ImGui::SliderFloat("Seek", &m_pBlendedSteering->m_WeightedBehaviors[3].weight, 0.f, 1.f, "%.2");
	ImGui::SliderFloat("Wander", &m_pBlendedSteering->m_WeightedBehaviors[4].weight, 0.f, 1.f, "%.2");
	//End
	ImGui::PopAllowKeyboardFocus();
	ImGui::End();
	
}

void Flock::RegisterNeighbors(SteeringAgent* pAgent)
{
	// register the agents neighboring the currently evaluated agent
	// store how many they are, so you know which part of the vector to loop over
	int noAgents{};
	std::vector<SteeringAgent*> tempNeighbors;
	for (size_t i{}; i < m_Agents.size(); i++)
	{
		Elite::Vector2 vecToOther =m_Agents[i]->GetPosition()-pAgent->GetPosition();
		if (vecToOther.Magnitude() < m_NeighborhoodRadius&& vecToOther.Magnitude()!=0)
		{
			tempNeighbors.push_back(m_Agents[i]);
			noAgents++;
		}
	}
	m_NrOfNeighbors = noAgents;
	m_Neighbors = tempNeighbors;
}

void Flock::SetBlendedSteering(BlendedSteering* pBlendedSteering)
{
	m_pBlendedSteering = pBlendedSteering;
}

void Flock::SetPrioritySteering(PrioritySteering* pPrioritySteering)
{
	m_pPrioritySteering = pPrioritySteering;
}

void Flock::SetSteeringBehaviour()
{
	for (size_t i{}; i < m_Agents.size(); i++)
	{
		m_Agents[i]->SetSteeringBehavior(m_pPrioritySteering);


	}
}

Elite::Vector2 Flock::GetAverageNeighborPos() const
{
	Elite::Vector2 temp{};
	for (size_t i = 0; i < size_t(m_NrOfNeighbors); i++)
	{
		temp += m_Neighbors[i]->GetPosition();
	}
	temp /= float(m_NrOfNeighbors);
	
	return temp;

}

Elite::Vector2 Flock::GetAverageNeighborVelocity() const
{
	Elite::Vector2 temp{};
	for (size_t i = 0; i < size_t(m_NrOfNeighbors); i++)
	{
		temp += m_Neighbors[i]->GetLinearVelocity();
	}
	temp /= float(m_NrOfNeighbors);

	return temp;


}


float* Flock::GetWeight(ISteeringBehavior* pBehavior) 
{
	if (m_pBlendedSteering)
	{
		auto& weightedBehaviors = m_pBlendedSteering->m_WeightedBehaviors;
		auto it = find_if(weightedBehaviors.begin(),
			weightedBehaviors.end(),
			[pBehavior](BlendedSteering::WeightedBehavior el)
			{
				return el.pBehavior == pBehavior;
			}
		);

		if(it!= weightedBehaviors.end())
			return &it->weight;
	}

	return nullptr;
}
