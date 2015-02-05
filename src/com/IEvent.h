/*
 * sy-ui v0.1 Copyright (c) 2015 sy-ui.com, 
 * http://sy-ui.com
 * Released under MIT license
 * Email:5392402@qq.com
 * ά���Ŷӣ���������Ա���ˣ�QQȺ��79388177��
 */

#ifndef _IEVENT_H
#define _IEVENT_H

#include "CObject.h"

#include <queue>
using namespace std;



class IEvent
{
private:
	queue<CEvent*>		evtQueue;
	static IEvent	*	instance;
	CRITICAL_SECTION	cs;

	IEvent()
	{
		InitializeCriticalSection(&cs); //��ʼ���ٽ���
	};
	
	~IEvent()
	{
		DeleteCriticalSection(&cs);//ɾ���ٽ��� 
	};

public:

	static IEvent * getInstance()
	{
		return instance;
	}

	/////////////////////////////////////////////////////////////
	//�¼��������
	
	//�����¼�������
	bool addEvent(CEvent * evt)
	{
		//����ؼ���������ȡ���¼�
		if(evt->target->enable == false || 
			evt->target->hasEvent == false)
			//evt->target->isVisible() == false) 
			return false;

		void * e;
		
		switch(evt->getClassType())
		{
			case CLASS_EVENT:
				e = new CEvent;
				memcpy(e,evt,sizeof(CEvent));
				break;

			case CLASS_MOUSE_EVENT:
				e = new CMouseEvent;
				memcpy(e,evt,sizeof(CMouseEvent));
				break;

			case CLASS_KEYBD_EVENT:
				e = new CKeyboardEvent;
				memcpy(e,evt,sizeof(CKeyboardEvent));
				break;
		}

		EnterCriticalSection(&cs);
		evtQueue.push((CEvent*)e);
		LeaveCriticalSection(&cs);
		return true;
	}

	//�Ӷ�����ȡ����Ϣ
	CEvent * getEvent()
	{
		EnterCriticalSection(&cs);

		CEvent * ret = NULL;		

		if (!evtQueue.empty())
		{
			ret = evtQueue.front();
			evtQueue.pop();
		}

		LeaveCriticalSection(&cs);
		return ret;
	}

	//��ȡ���������Ϣ����
	int getQueueSize()
	{
		EnterCriticalSection(&cs);
		int ret = evtQueue.size();
		LeaveCriticalSection(&cs);
		return ret;
	}


	////////////////////////////////////////////////////////////////
	//�¼��������

	////////////////////////////////////////////////////////////////
	//�ɷ�ϵͳ�¼�
	void DispatchSystemEvent(int evtType, CObject * obj, DWORD data = NULL)
	{
		//�����¼�����
		CEvent evt;
		evt.type			= evtType;
		evt.evtData			= data;
		
		//�ɷ��¼�
		DispatchEvent(obj,&evt);
	}

	////////////////////////////////////////////////////////////////
	//�ɷ�����¼�
	void DispatchMouseEvent(int evtType, int clientX, int clientY, int screenX, int screenY, int button, int delta, DWORD data=0)
	{
		CMouseEvent evt;
		
		evt.altKey			= (GetKeyState(VK_MENU)<0);
		evt.shiftKey		= (GetKeyState(VK_SHIFT)<0);
		evt.ctrlKey			= (GetKeyState(VK_CONTROL)<0);
		
		evt.target			= NULL;
		evt.currentTarget	= NULL;
		
		//�����Ϣ
		evt.clientX		= clientX;
		evt.clientY		= clientY;		
		evt.screenX		= screenX;
		evt.screenY		= screenY;
		evt.button		= button;
		evt.type		= evtType;
		evt.delta		= delta;
		if(data>0) evt.evtData = data;
				
		//�ɷ��¼�
		if(iObject->captureObject == NULL)
		{
			DispatchEvent(iObject->eventStage ,&evt);
		} else {
			//�򲶻��������Ϣ
			evt.target = iObject->captureObject;
			evt.currentTarget = iObject->captureObject;
			
			//ѹ���¼�����
			addEvent(&evt);	
		}

		return;
	}
	
	////////////////////////////////////////////////////////////////
	//�ɷ������¼�
	void DispatchKeyboardEvent(int evtType, int KeyCode)
	{
		//�����¼�����
		CKeyboardEvent evt;
		
		evt.type	= evtType;
		evt.keyCode	= KeyCode;

		//�ɷ��¼�
		DispatchEvent(iObject->eventStage,&evt);
	}

	//�ɷ��¼�
	bool DispatchEvent(CObject * obj, CEvent * evt)
	{
		if(obj == NULL) 
			return false;

		if (evt->getClassType() != CLASS_EVENT &&(obj->isVisible() == false || obj->enable == false)) 
			return false;


		if(evt->getClassType() != CLASS_EVENT)
		{
			//���ȴ����¼�����Ԫ�ش���(�������)
			int count = iObject->getChildNum(obj);
			for(int i=count-1;i>=0;i--)
			{
				CObject * child = iObject->getChildAt(obj,i);
				if (DispatchEvent(child,evt) == true)  return true;
			}
		}

		//���������Ӧ�¼��򷵻�
		if(obj->hasEvent == false) return false;

		switch(evt->getClassType())
		{
			case CLASS_EVENT:
				evt->target = obj;
				if (evt->currentTarget == NULL)
					evt->currentTarget = obj;
				//ѹ���¼�����
				this->addEvent(evt);
				break;
				
			case CLASS_MOUSE_EVENT:
				
				POINT	pt;

				pt.x = ((CMouseEvent*)evt)->clientX;
				pt.y = ((CMouseEvent*)evt)->clientY;

				//����¼������ڶ��������������¼�
				if(	   pt.x > obj->stageX
					&& pt.x < (obj->stageX + obj->width)
					&& pt.y > obj->stageY
					&& pt.y < (obj->stageY + obj->height)
					)
				{
					/////////////////////////////////////////////////////////////////
					//�����¼�
					if( obj->hasFocus == true
						&& evt->type == CMouseEvent.MOUSE_DOWN 
						&& iObject->focusObject != obj
						)
					{
						obj->setFocus();						
					}
					
					/////////////////////////////////////////////////////////////////
					//��������Ƴ��¼�
					if (evt->type == CMouseEvent.MOUSE_MOVE 
						&& iObject->mouseObject != obj
						)
					{
						if(iObject->mouseObject != NULL)
						{
							//��ԭ�ؼ�����mouse out�¼�
							evt->target			= iObject->mouseObject;
							evt->type			= CMouseEvent.MOUSE_OUT;
							this->addEvent(evt);
						}
						
						iObject->mouseObject	= obj;

						//��������mouse over�¼�
						evt->target			= iObject->mouseObject;
						evt->type			= CMouseEvent.MOUSE_OVER;
						this->addEvent(evt);
					}

					evt->target = obj;
					if (evt->currentTarget == NULL)
						evt->currentTarget = obj;
					
					//ѹ���¼�����
					this->addEvent(evt);

					return true;
				} 
				break;
				
			case CLASS_KEYBD_EVENT:
				//ֻ�н���Ŀ��Ž��ܰ����¼�
				if (iObject->focusObject != NULL)
				{
					evt->target = iObject->focusObject;
					if (evt->currentTarget == NULL)
						evt->currentTarget = iObject->focusObject;
					
					//ѹ���¼�����
					this->addEvent(evt);
				}
				return true;

			default:
				evt->target = obj;
				if (evt->currentTarget == NULL)
					evt->currentTarget = obj;
				//ѹ���¼�����
				this->addEvent(evt);
				break;
		}

		return false;
	};

	 
	//�����¼�
	bool ProcessEvent(CEvent * evt)
	{
		if(evt->type != CEvent.UNLOAD && evt->target->isUnload == false)
		{
			//�����¼��������Ļص�����
			evt->target->processCallBacks((void * )evt);

			if(evt->type == CMouseEvent.MOUSE_UP)
			{
				if(	   ((CMouseEvent*)evt)->clientX > evt->target->stageX
					&& ((CMouseEvent*)evt)->clientX < (evt->target->stageX + evt->target->width)
					&& ((CMouseEvent*)evt)->clientY > evt->target->stageY
					&& ((CMouseEvent*)evt)->clientY < (evt->target->stageY + evt->target->height)
					)
				{
					evt->type = CMouseEvent.MOUSE_CLICK;
					this->addEvent(evt);
					evt->type = CMouseEvent.MOUSE_UP;
				}
			} 

			if(evt->type != CMouseEvent.MOUSE_CLICK && evt->getClassType() != CLASS_EVENT)
			{
				//���Ŀ�������¼�ð��Ϊ����  �����ϴ����¼�
				if ( (evt->cancelBubble != true || evt->target->evtBubble == true)
					&& evt->target->parent != NULL 
					&& evt->target->parent != evt->target) 
				{

					evt->target = evt->target->parent;

					//���ϴ����¼�
					if(evt->getClassType() == CLASS_MOUSE_EVENT)
					{
						CMouseEvent * me = (CMouseEvent*)evt;
						this->addEvent(me);
					} else {
						CKeyboardEvent * ke = (CKeyboardEvent*)evt;
						this->addEvent(ke);
					}				
				}
			}
			
		} else {
			delete evt->target;
		}

		//�ͷ��¼�����
		delete evt;
		
		return true;
	};

	void doEvent()
	{
		CEvent * evt;
		
		do {
			evt = getEvent();
		
			if(evt != NULL) 
				ProcessEvent(evt);
			else
				Sleep(1);

		}while(evt!=NULL);		
	}
};

IEvent * IEvent::instance=new IEvent();


//�����ýӿڵ�ȫ��ָ��
IEvent * iEvent = IEvent::getInstance();


#endif