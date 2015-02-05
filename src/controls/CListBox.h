/*
 * sy-ui v0.1 Copyright (c) 2015 sy-ui.com, 
 * http://sy-ui.com
 * Released under MIT license
 * Email:5392402@qq.com
 * ά���Ŷӣ���������Ա���ˣ�QQȺ��79388177��
 */

#ifndef _CLISTBOX_H
#define _CLISTBOX_H

#include <list>

class CListItem
{
public:
	wchar_t *text;
	unsigned long luserid;
	int		len;
	RECT	rc;
};

class CListBox:public CPanel
{
private:
	friend void onCListBoxMouseWheel(CMouseEvent * evt);
	friend void onCListBoxMouseDown(CMouseEvent * evt);
	friend void onCListBoxMouseMove(CMouseEvent * evt);
	friend void onCListBoxMouseOut(CMouseEvent * evt);

public:
	int		margin;
	int		textAlign;
	int		fontSize;
	long	selectIndex;
	long	traceIndex;
	long	scrollTop;
	long	scrollHeight;
	DWORD	item_selectColor;
	DWORD	item_traceColor;
	DWORD	item_foreColor;
	DWORD	item_backColor;
	RECT	textRC;
	list<CListItem*> item;
	CRITICAL_SECTION cs;
	CScrollBar	* scr;
	wchar_t	*text;
	int    ntrim;
	int    nleft;
	CListBox(int resid, int x, int y, int w, int h, DWORD id=0, wchar_t * pname=NULL):CPanel(resid,x,y,w,h,id,pname)
	{
		typeID = OBJECT_LISTBOX;
		InitializeCriticalSection(&cs); //��ʼ���ٽ���

		scr					= NULL;
		selectIndex			= -1;
		traceIndex			= 0;
		margin				= 10;
		textAlign			= 0;
		scrollTop			= 0;
		scrollHeight		= 0;
		fontSize			= 9;
		item_foreColor		= 0xffEfEfEf;//0xFEE0E0E0;
		item_backColor		= 0xFE000000;
		item_selectColor	= 0xA0E0E0E0;
		item_traceColor		= 0x20E0E0E0;
		ntrim               = 0;
		nleft               = 0;
		addEventListener(CMouseEvent.MOUSE_WHEEL,(LPGUI_EVENT_FUNC)&onCListBoxMouseWheel);
		addEventListener(CMouseEvent.MOUSE_DOWN,(LPGUI_EVENT_FUNC)&onCListBoxMouseDown);
		addEventListener(CMouseEvent.MOUSE_MOVE,(LPGUI_EVENT_FUNC)&onCListBoxMouseMove);
		addEventListener(CMouseEvent.MOUSE_OUT,(LPGUI_EVENT_FUNC)&onCListBoxMouseOut);
	}

	virtual ~CListBox()
	{
		CPanel::~CPanel();

		LeaveCriticalSection(&cs);//�뿪�ٽ���
		DeleteCriticalSection(&cs);//ɾ���ٽ��� 
	};

	//�ƶ�
	void move(int x, int y, int w, int h)
	{
		CPanel::move(x,y,w,h);
		if(scr!=NULL) scr->move(width-scr->width,0,scr->width,height);
	};
	
	//�ƶ�
	void move(int x, int y)
	{
		CPanel::move(x,y);
		if(scr!=NULL) scr->move(width-scr->width,0);
	};

	//���ù�����
	void setScrollBar(int resid, int btnResId)
	{
		scr = new CScrollBar(resid,width-6,0,6,height,NULL,NULL);
		this->addChild(scr);
		scr->setImage(resid,btnResId);
		scr->bind(this);
		scr->X = width - scr->width;
		scr->visible = false;
		bindObject = scr;
	}

	void refresh()
	{
		if(!visible) return;

		CPanel::drawSelf();

		getTextRect(&textRC);

		setClip(
			textRC.left,
			textRC.top,
			textRC.right-textRC.left,
			textRC.bottom-textRC.top);
		
		EnterCriticalSection(&cs);	

		long	i=0;
		list<CListItem*>::iterator Iter;
		for(Iter = item.begin(); Iter != item.end(); Iter++)
		{
			CListItem * pitem = *Iter;

			if(pitem == NULL) continue;

			//����б�������ʾ��������������һ��ѭ��
			if (pitem->rc.bottom < scrollTop 
				|| (pitem->rc.top > scrollTop + (textRC.bottom-textRC.top))) 
				continue;

			DWORD	item_textColor = item_foreColor;
			DWORD	item_selColor  = 0;
			DWORD	item_treColor  = 0;

			//�������껮�����򻭿�
			if(i == traceIndex) item_treColor = item_traceColor;

			//����Ǳ�ѡ�����򻭱���
			if(i == selectIndex) 
			{
				item_selColor  = item_selectColor;
				item_textColor = 0xFE000000;
			}

			i++;

			if (item_treColor != 0)
			{
				graphics->fillRectangle(
					item_traceColor,
					textRC.left + pitem->rc.left,
					textRC.top + pitem->rc.top - scrollTop,
					pitem->rc.right - pitem->rc.left,
					pitem->rc.bottom - pitem->rc.top);
			}
			
			if(item_selColor != 0)
			{
				graphics->fillRectangle(
					item_selectColor,
					textRC.left + pitem->rc.left,
					textRC.top + pitem->rc.top - scrollTop,
					pitem->rc.right - pitem->rc.left,
					pitem->rc.bottom - pitem->rc.top);
			}
			
			//�������
			graphics->drawString(
				pitem->text,
				textRC.left + nleft,
				textRC.top + (pitem->rc.top - scrollTop) + ntrim,
				textRC.right - textRC.left,
				textRC.bottom - textRC.top - (pitem->rc.top - scrollTop),
				item_foreColor);				
 		}
		//	_cprintf(" scrollTop %d\n",scrollTop);
		LeaveCriticalSection(&cs);

		resetClip();

		CPanel::drawChild();
	};
	
	void setTrim(int n)
	{
		ntrim = n;
	}

	long find(unsigned long id)
	{
		long ret = -1;
		EnterCriticalSection(&cs);
		long i = 0;
		unsigned long l = 0;
		list<CListItem*>::iterator Iter;
		for(Iter = item.begin(); Iter != item.end(); Iter++)
		{
			CListItem * pitem = *Iter;
			l = pitem->luserid;
			if(l == id) 
			{
				ret = i;
			}
			i++;
		}
		LeaveCriticalSection(&cs);

		return ret;
	}

	long insert(const wchar_t * str,unsigned long ls = 0)
	{
		EnterCriticalSection(&cs);
	
		int len = wcslen(str);
		CListItem * pitem = new CListItem;
		
		pitem->text = new wchar_t[len+1];
		wcscpy(pitem->text,str);
		pitem->luserid = ls;
		getTextRect(&textRC);
		
		long h= g_font->calcTextHeight(pitem->text,1000);
		//_cprintf("%s %d\n",pitem->text,h);

		pitem->len			= sizeof(wchar_t)*(len+1);
		pitem->rc.left		= 0;
		pitem->rc.top		= scrollHeight;
		pitem->rc.right		= textRC.right - textRC.left;
		pitem->rc.bottom	= pitem->rc.top + h;
		
		item.push_back(pitem);
		
		if(scrollHeight > height  && scr != NULL) scr->visible = true;

		scrollHeight += h;

		LeaveCriticalSection(&cs);

		return h;
	};

	wchar_t * getItemAt(long idx)
	{
		EnterCriticalSection(&cs);
		long i = 0;
		wchar_t * ret = NULL;
		list<CListItem*>::iterator Iter;
		for(Iter = item.begin(); Iter != item.end(); Iter++)
		{
			if(i == idx) 
			{
				CListItem * pitem = *Iter;
				ret = pitem->text;
				//WideCharToMultiByte(CP_ACP,0,pitem->text,wcslen(pitem->text),str, len,NULL,NULL);
				break;;
			}
			i++;
		}
		LeaveCriticalSection(&cs);
		return ret;
	}

	void removeAt(long idx)
	{
		EnterCriticalSection(&cs);
		long i = 0;
		CListItem * pitem;
		list<CListItem*>::iterator Iter;
		for(Iter = item.begin();;)
		{
			if(i == idx) 
			{
				pitem = *Iter;
				Iter = item.erase(Iter);
			} else {
				Iter++;
			}

			//������б��β������
			if (Iter == item.end() ) break;

			//��������б���topֵ��ȥ��ɾ����ĸ߶�
			if(i >= idx)
			{
				CListItem * temp = *Iter;
				long th = pitem->rc.bottom - pitem->rc.top;
				temp->rc.top	-= th;
				temp->rc.bottom	-= th;
			}

			i++;
		}
		scrollHeight -= (pitem->rc.bottom - pitem->rc.top);
		delete pitem;
		
		if(scrollHeight <= height && scr != NULL) scr->visible = false;

		LeaveCriticalSection(&cs);
	}

	void clear()
	{
		EnterCriticalSection(&cs);
		list<CListItem*>::iterator Iter;
		for(Iter = item.begin();Iter != item.end();)
		{
			delete *Iter;
			Iter = item.erase(Iter);
		}

		scrollHeight = 0;
		LeaveCriticalSection(&cs);
		if(scr->visible  && scr != NULL) scr->visible = false;
	}

	long getItemCount()
	{
		long ret;
		EnterCriticalSection(&cs);
		ret = item.size();
		LeaveCriticalSection(&cs);
		return ret;
	}

	long PtToItemIndex(long x, long y)
	{

		EnterCriticalSection(&cs);

		long ret = -1;

		long	i			= 0;		
		long	h			= y -textRC.top + scrollTop ;

		list<CListItem*>::iterator Iter;
		for(Iter = item.begin(); Iter != item.end(); Iter++)
		{
			CListItem * pitem = *Iter;
			if(h > pitem->rc.top && h<= pitem->rc.bottom) 
			{
				ret = i;
				break;
			}
			i++;
		}
		LeaveCriticalSection(&cs);

		if (ret > getItemCount()) ret = -1;

		return ret;
	}

	void scrollTo(int n)
	{
		if (n<0) n = 0;
		if (n>100) n = 100;

		RECT rc;
		getTextRect(&rc);
		long totalHeight = scrollHeight - (rc.bottom - rc.top);

		scrollTop = totalHeight * ((float)n/100);

		iEvent->DispatchSystemEvent(CEvent.SCROLL,this,n);
	}

	void getTextRect(RECT * rc)
	{
		rc->left	= stageX;
		rc->top		= stageY;
		rc->right	= stageX + width;
		rc->bottom	= stageY + height;

		rc->left	+= margin;
		rc->top		+= margin;
		rc->right	-= margin;
		rc->bottom	-= margin;
	}

	unsigned long getItemIdAt(long idx)
	{
		EnterCriticalSection(&cs);
		long i = 0;
		unsigned long l = 0;
		list<CListItem*>::iterator Iter;
		for(Iter = item.begin(); Iter != item.end(); Iter++)
		{
			if(i == idx) 
			{
				CListItem * pitem = *Iter;
				l = pitem->luserid;
				//WideCharToMultiByte(CP_ACP,0,pitem->text,wcslen(pitem->text),str, len,NULL,NULL);
				break;;
			}
			i++;
		}
		LeaveCriticalSection(&cs);
		return l;
	}
};

void onCListBoxMouseWheel(CMouseEvent * evt)
{
	CListBox * listbox = (CListBox*)evt->target;

	long n = listbox->scrollTop;

	RECT rc;
	listbox->getTextRect(&rc);
	long totalHeight = listbox->scrollHeight - (rc.bottom - rc.top);

	n += -(evt->delta/6);

	if(n < 0) n=0;
	if(n> totalHeight) n = totalHeight;

	listbox->scrollTop = n;

	long i = (float)n / totalHeight * 100;

	if(listbox->bindObject!=NULL) ((CScrollBar*)listbox->bindObject)->scrollTo(i);

	iEvent->DispatchSystemEvent(CEvent.SCROLL,listbox,i);
}

void onCListBoxMouseDown(CMouseEvent * evt)
{
	CListBox * listbox = (CListBox*)evt->target;

	long n = listbox->PtToItemIndex(evt->clientX,evt->clientY);

	if(n != -1)
	{
		listbox->selectIndex = n;
		listbox->text = listbox->getItemAt(n);
		iEvent->DispatchSystemEvent(CEvent.CHANGE,listbox,listbox->selectIndex);
	}
}

void onCListBoxMouseMove(CMouseEvent * evt)
{
	CListBox * listbox = (CListBox*)evt->target;
	
	long n = listbox->PtToItemIndex(evt->clientX,evt->clientY);
	listbox->traceIndex = n;
}

void onCListBoxMouseOut(CMouseEvent * evt)
{
	((CListBox*)evt->target)->traceIndex = 0;
}
#endif