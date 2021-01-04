#pragma once
class FlowField
{
public:
	FlowField(int width, int height);
	~FlowField();
	Elite::Vector2 GetDirectionAt(int index);
	void SetValueAt(int index, float cost);
	float GetValueAt(int index);
	int GetWidth();
	int GetHeight();
	void CalculateValues(int index);
	void SetCostAt(int index, int cost);
	void SetDirtAt(int index,const Elite::Vector2 & dir);
	int GetCostAt(int index)const;
	Elite::Vector2 GetDirAt(int index)const;
	void CalculateDirections();
	void ResetValues();
private:

	int m_Width, m_Height;
	vector<float> m_Values{};
	vector<int> m_Cost{};
	vector<Elite::Vector2> m_Directions{};


};

