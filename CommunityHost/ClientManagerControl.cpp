#include "ClientManagerControl.h"
#include "ClientPanel.h"

#include <iostream>

#include <nana\gui\widgets\panel.hpp>

using namespace nana::gui;

UI::ClientManagerControl::ClientManagerControl( nana::gui::window w,nana::rectangle& rect ) :
	nana::gui::panel<true>(w,rect)
{
	this->m_scroll.reset(new nana::gui::scroll<true>(this->handle(),nana::rectangle(rect.width - 10,0,10,rect.height),true));
	this->m_scroll->amount(rect.height);
	this->m_uLines = 0;
	this->m_uScrollState = 0;
	this->m_scroll->ext_event().value_changed.append(std::bind(&UI::ClientManagerControl::Scrolled,this,std::placeholders::_1));
}

UI::ClientManagerControl::~ClientManagerControl( void )
{

}

std::shared_ptr<UI::ClientPanel> UI::ClientManagerControl::Append()
{
	this->m_vPanel.push_back(std::make_shared<UI::ClientPanel>(this->handle(),nana::rectangle(0,0,PANEL_WIDTH,PANEL_HEIGHT)));
	this->Reorder();
	return this->m_vPanel[this->m_vPanel.size()-1];
}

void UI::ClientManagerControl::Remove( uint32_t nIndex )
{
	if(0 >= nIndex || this->m_vPanel.size() < nIndex)
		return;

	this->m_vPanel.erase(this->m_vPanel.begin() + (nIndex - 1));
}

void UI::ClientManagerControl::Reorder()
{
	const int32_t WIDTH_WITH_BORDER = PANEL_WIDTH +5,
		HEIGHT_WITH_BORDER = PANEL_HEIGHT + 5;

	auto _size = this->size();

	int32_t nPanelsInRow = 0;

	if(_size.width)
		nPanelsInRow = _size.width / (PANEL_WIDTH + 5);

	if(!nPanelsInRow)
		return;

	auto x = this->m_vPanel.size();
	int32_t nCounter = 0,nLineCounter = 0;

	while(x)
	{
		this->m_vPanel[x-1]->move((nCounter*WIDTH_WITH_BORDER)+5,
			((nLineCounter*HEIGHT_WITH_BORDER)+5) - this->m_uScrollState);
		this->m_vPanel[x-1]->show();

		nCounter++;

		if(nCounter >= nPanelsInRow)
		{
			nCounter = 0;
			nLineCounter++;
		}
		x--;
	}
	this->m_uLines = nLineCounter;

	uint32_t u = std::max(this->size().height,(uint32_t)((nLineCounter-1)*HEIGHT_WITH_BORDER)+5);
	this->m_scroll->amount(u);
}

void UI::ClientManagerControl::Scrolled(widget& w)
{
	this->m_uScrollState = this->m_scroll->value();
	this->Reorder();
}
