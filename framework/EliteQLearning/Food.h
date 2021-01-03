#ifndef FOOD
#define FOOD

#include "framework/EliteMath/EVector2.h"
#include "framework/EliteInterfaces/EIApp.h"
//#include "Generation.h"
using namespace Elite;
class Food
{
private:
	Vector2 m_Location;
	Color m_Color;
	bool m_IsEaten = false;
	int m_RespawnCounter{ 1000 };
	int m_Respawn{ 1000 };

public:
	Food(float x, float y) : m_Location(x, y), m_Color(0.5f, 0.5f, 1) {

	}

	Vector2& GetLocation() {
		return m_Location;
	}

	void Render();

	void Eat() {
		m_IsEaten = true;
		m_RespawnCounter = m_Respawn;
	}

	bool IsEaten() {
		return m_IsEaten;
	}

	void Update() {
		if (m_IsEaten) {
			m_RespawnCounter--;
			if (m_RespawnCounter == 0) {
				m_IsEaten = false;
			}
		}
	}

	//void Reset(Generation* gen) {
	//	float angle = gen->Random(0, 1);
	//	angle *= angle * 2 * M_PI;
	//	float dist = gen->Random(0, 1);
	//	dist *= 100;
	//	dist += 20;
	//	m_Location.x = dist * cos(angle);
	//	m_Location.y = dist * sin(angle);

	//	m_IsEaten = false;
	//}
};
#endif