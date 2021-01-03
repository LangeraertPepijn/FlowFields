#include "stdafx.h"
#include "Food.h"

void Food::Render() {
	if (!m_IsEaten) {
		DEBUGRENDERER2D->DrawCircle(m_Location, .2f, m_Color,0.5f);
	}
}