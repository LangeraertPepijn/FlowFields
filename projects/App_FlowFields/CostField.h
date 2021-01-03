#pragma once
#include <vector>
class CostField
{
private:
		int m_Width, m_Height;
		vector<int> m_Cost{};
public:
		CostField( int width,  int height);
		~CostField();
		
		void SetCostAt( int index,int cost );
		int GetCostAt( int index );
};

