#pragma once
class FlowField
{
public:
	FlowField(int width, int height);
	~FlowField();
	Elite::Vector2 GetDirectionAt(int index);
	void SetValueAt(int index, int cost);
	int GetValueAt(int index);
	void CalculateValues(int index);
	void SetCostAt(int index, int cost);
	int GetCostAt(int index)const;
	Elite::Vector2 GetDirAt(int index)const;
	void CalculateDirections();
private:

	int m_Width, m_Height;
	vector<int> m_Values{};
	vector<int> m_Cost{};
	vector<Elite::Vector2> m_Directions{};


};

