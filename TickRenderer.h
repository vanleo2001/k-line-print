#pragma once

#include "TickReader.h"

class TickRenderer
{
private:

	Tick m_tick;
	CRect m_Rect;

public:
	
	TickRenderer(void);
	~TickRenderer(void);

	//	��������
	void SetTick(Tick tick) { m_tick = tick; }

	//	���û��Ʒ�Χ
	void SetRect(CRect rect) { m_Rect = rect; }

	//	�����̿�
	void Render(CDC* pDC);
};