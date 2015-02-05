#ifndef _IGRAPHICS_H
#define _IGRAPHICS_H

#include <map>
using namespace std;

class IGraphics
{

private:
	CRITICAL_SECTION	cs;
	static IGraphics  * instance;

	IGraphics()
	{
		InitializeCriticalSection(&cs); //��ʼ���ٽ���
	};
	
	~IGraphics()
	{
		LeaveCriticalSection(&cs);//�뿪�ٽ���
		DeleteCriticalSection(&cs);//ɾ���ٽ��� 
	};

public:
	map<CObject*,CGraphics*> graphics;

	static IGraphics * getInstance()
	{
		return instance;
	}

	void  addGraph(CObject * stage)
	{
		EnterCriticalSection(&cs);
		LeaveCriticalSection(&cs);
	}

	void removeGraph(CObject * stage)
	{
		EnterCriticalSection(&cs);
		graphics.erase(stage);
		LeaveCriticalSection(&cs);
	}

	CGraphics * getGraph(CObject * stage)
	{
		CGraphics * graph = NULL;
		EnterCriticalSection(&cs);
		graph = graphics[stage];
		LeaveCriticalSection(&cs);
		return graph;
	}
};

IGraphics * IGraphics::instance=new IGraphics();

//�����ýӿڵ�ȫ��ָ��
IGraphics * iGraphics = IGraphics::getInstance();

#endif