#include "stdafx.h"
#include "QLearning.h"
#include <stdio.h>

QLearning::QLearning(int nrOfLocations, int startIndex, int endIndex)
	:m_pRewardMatrix(new FMatrix(nrOfLocations, nrOfLocations)),
	m_pQMatrix(new FMatrix(nrOfLocations, nrOfLocations)),
	m_pTreasure(new FMatrix(nrOfLocations, nrOfLocations)),
	m_pKobold(new FMatrix(nrOfLocations, nrOfLocations)),
	m_pEnvMatrix(new FMatrix(nrOfLocations, nrOfLocations)),
	m_StartIndex(startIndex),
	m_EndIndex(endIndex),
	m_NrOfLocations(nrOfLocations),
	m_pIndexBuffer(new int[nrOfLocations])
{
	m_Locations.resize(nrOfLocations);
	m_pRewardMatrix->SetAll(-1.0f);
	m_pQMatrix->SetAll(0.0f);
	m_pTreasure->SetAll(0.0f);
	m_pKobold->SetAll(0.0f);
	m_pEnvMatrix->SetAll(0.0f);
	m_pRewardMatrix->Set(endIndex, endIndex, 100);
	m_Kobolds.push_back(4);
	m_Kobolds.push_back(5);
	m_Kobolds.push_back(6);
	m_Treasure.push_back(2);
}

void QLearning::SetLocation(int index, Elite::Vector2 location)
{
	if (index < m_NrOfLocations) {
		m_Locations[index] = location;
	}
}

void QLearning::AddConnection(int from, int to)
{
	// ----------------------------------------------
	// connections are set in the m_pRewardMatrix !!!
	// ----------------------------------------------
	// set two cells corresponding to (from,to) and (to,from) to 0
	// unless the to is equal to the end index, then the (from,to) should be 100.
	// use the set method of the fmatrix class
	if (m_EndIndex== to )
	{

		
		m_pRewardMatrix->Set(to, from, 0);
		m_pRewardMatrix->Set(from, to, 100);
	}
	else
	{
		m_pRewardMatrix->Set(from, to, 0);
		m_pRewardMatrix->Set(to, from, 0);
	}
}

void QLearning::Render(float deltaTime)
{
	char buffer[10];
	Elite::Vector2 arrowPoints[3];
	for (int row = 0; row < m_pRewardMatrix->GetNrOfRows(); ++row)
	{
		for (int column = 0; column < m_pRewardMatrix->GetNrOfColumns(); ++column)
		{
			if (m_pRewardMatrix->Get(row, column) >= 0.f) {

				Elite::Vector2 start = m_Locations[row];
				Elite::Vector2 end = m_Locations[column];
				float length = start.Distance(end);

				
				Elite::Vector2 dir = end - start;
				dir.Normalize();
				Elite::Vector2 perpDir(dir.y, -dir.x);
				

				Elite::Vector2 tStart = start + perpDir * 2;
				Elite::Vector2 tEnd = end + perpDir * 2;

				Elite::Vector2 mid = (tEnd + tStart) * .5 + 5 * dir;



				



				arrowPoints[0] = mid + dir * 5;
				arrowPoints[1] = mid + perpDir * 1.5f;
				arrowPoints[2] = mid - perpDir * 1.5f;

				float qValue = m_pQMatrix->Get(row, column);
				float max = m_pQMatrix->Max();
				float ip = qValue / max;
				float ipOneMinus = 1 - ip;
				Elite::Color c;
				c.r = m_NoQConnection.r * ipOneMinus + m_MaxQConnection.r * ip;
				c.g = m_NoQConnection.g * ipOneMinus + m_MaxQConnection.g * ip;
				c.b = m_NoQConnection.b * ipOneMinus + m_MaxQConnection.b * ip;
				DEBUGRENDERER2D->DrawSegment(tStart, tEnd, c);
				DEBUGRENDERER2D->DrawSolidPolygon(&arrowPoints[0], 3, c, 0.5);
				snprintf(buffer, 10, "%.0f", qValue);
				DEBUGRENDERER2D->DrawString(mid + perpDir*3, buffer);
			}
		}
	}

	int index = 0;
	

	for (Elite::Vector2 loc : m_Locations)
	{
		snprintf(buffer, 3, "%d", index);
		DEBUGRENDERER2D->DrawString(loc + Elite::Vector2(1.5f, 0), buffer);
		if (index == m_StartIndex)
		{
			DEBUGRENDERER2D->DrawSolidCircle(loc, 2.0f, Elite::Vector2(1, 0), m_StartColor, 0.5f);
		}
		else if (index == m_EndIndex) {
			DEBUGRENDERER2D->DrawSolidCircle(loc, 2.0f, Elite::Vector2(1, 0), m_EndColor, 0.5f);
		}
		else {
			DEBUGRENDERER2D->DrawSolidCircle(loc, 2.0f, Elite::Vector2(1, 0), m_NormalColor, 0.5f);
		}

		++index;
	}

}

int QLearning::SelectAction(int currentLocation)
{
	// Step 2 in the slides, select a to node via the reward matrix.
	// return this to-node as the result
	
	const int nrOfColumns = m_pRewardMatrix->GetNrOfColumns();
	std::vector<int> optionalIndices{};
	for (int i{};i < nrOfColumns; i++)
	{
		if (m_pRewardMatrix->Get(currentLocation, i)>=0)
		{
			optionalIndices.push_back(i);
		}

	}
	int index = rand();
	index%=optionalIndices.size();
	return optionalIndices[index];
}

int QLearning::SelectActionWithEnv(int currentLoc)
{
	const int nrOfColumns = m_pRewardMatrix->GetNrOfColumns();
	std::vector<int> optionalIndices{};
	for (int i{}; i < nrOfColumns; i++)
	{
		if (m_pRewardMatrix->Get(currentLoc, i) >= 0)
		{

			optionalIndices.push_back(i);
		}

	}
	std::vector<int> optionalIndicesEnv{};

	for (int i{}; i < optionalIndices.size(); i++)
	{
		if (m_pEnvMatrix->Get(currentLoc, optionalIndices[i]) >= 0)
		{
			optionalIndicesEnv.push_back(optionalIndices[i]);
		}

	}
	if (optionalIndicesEnv.size() != 0)
	{
		int index = rand();
		index %= optionalIndicesEnv.size();
		return optionalIndicesEnv[index];
	}
	else
	{

		int index = rand();
		index %= optionalIndices.size();
		return optionalIndices[index];
	}
}

float QLearning::Update(int currentLocation, int nextAction)
{
	//kobolds and treasure
	for (auto location : m_Kobolds)
	{
		if (location == nextAction)
		{
			m_pKobold->Add(currentLocation, nextAction, 1);
		}

	}
	for (auto location : m_Treasure)
	{
		if (location == nextAction)
		{
			m_pTreasure->Add(currentLocation, nextAction, 1);
		}

	}

	

	m_pEnvMatrix->Copy(*m_pTreasure);
	m_pEnvMatrix->Subtract(*m_pKobold);
	// step 3 
	// A : get the max q-value of the row nextAction in the Q matrix
	float max = m_pQMatrix->MaxOfRow(nextAction);
	// B gather the elements that are equal to this max in an index buffer.
	// can use m_pIndexBuffer if it suits you. Devise your own way if you don't like it.
	const int nrOfColumns = m_pQMatrix->GetNrOfColumns();
	std::vector<int> buffer{};
	for (int i{}; i < nrOfColumns; i++)
	{
		if (m_pQMatrix->Get(nextAction, i) == max)
		{
			buffer.push_back(i);
		}
	}

	// C pick a random index from the index buffer and implement the update formula
	// for the q matrix. (slide 14)
	int index = rand() % buffer.size();
	float value = m_pQMatrix->Get(nextAction, buffer[index]);
	float qUpdate = m_pRewardMatrix->Get(currentLocation, nextAction) + m_Gamma * value;
	m_pQMatrix->Set(currentLocation,nextAction,qUpdate);
	// calculate the score of the q-matrix and return it. (slide 15)
	float Score = 100 * m_pQMatrix->Sum() / m_pQMatrix->Max();
	return Score;
}

void QLearning::Train() {
	if (m_CurrentIteration < m_NrOfIterations)
	{
		int currentLocation = Elite::randomInt(m_NrOfLocations);
		int nextLocation = SelectActionWithEnv(currentLocation);
		float score = Update(currentLocation, nextLocation);
		printf("Score %.2f\n", score);
		m_CurrentIteration++;
		
	}else if (m_CurrentIteration == m_NrOfIterations){
		m_pQMatrix->Print();
		//test from start point 0
		int location = m_StartIndex;
		std::cout << "kobold" << std::endl;
		m_pKobold->Print();
		std::cout << "treasure" << std::endl;
		m_pTreasure->Print();
		std::cout << "Env" << std::endl;
		m_pEnvMatrix->Print();
		printf("start at %d\t", location);
		
		// TODO : find the best path via the q-matrix.
		// uncomment the while loop when implementing, be careful for infinite loop.
		while (location != m_EndIndex)
		{
			//what is the maximum of the next action in the q-matrix
			float max=m_pQMatrix->MaxOfRow(location);
			//gather the elements that are equal to this max in an index buffer.
			const int nrOfColumns = m_pQMatrix->GetNrOfColumns();
			std::vector<int> buffer{};
			for (int i{}; i < nrOfColumns; i++)
			{
				if (m_pQMatrix->Get(location, i) == max)
				{
					buffer.push_back(i);
				}
			}
			
				// pick a random index from the index buffer.
			int index = rand() % buffer.size();
			location = buffer[index];
			printf("%d\t", location);



		}
		m_CurrentIteration++;
	}
}