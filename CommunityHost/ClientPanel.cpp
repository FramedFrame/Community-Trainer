#include "ClientPanel.h"

#include <iostream>
#include <Windows.h>

using namespace nana::gui;
using namespace UI;


UI::ClientPanel::ClientPanel( nana::gui::window w,nana::rectangle& rect ) :
	panel(w,rect,true)
{
	this->m_controlPanel.reset(new panel<true>(this->handle(),nana::rectangle(0,rect.height-50,rect.width,50)));
	this->m_frame.reset(new frame(this->handle(),nana::rectangle(0,0,rect.width,rect.height-50)));

	nana::string str = STR("");

	for(int i = 0; i < 5;i++)
	{
		str = STR("");
		auto s = std::make_shared<button>(this->m_controlPanel->handle(),nana::rectangle(0+(i*20),0,20,50));
		str += (STR('a') + i);
		s->caption(str);
		this->m_vButton.push_back(s);
	}
}

UI::ClientPanel::~ClientPanel( void )
{

}

void UI::ClientPanel::AttachWindow( nana::gui::native_window_type hWnd )
{
	this->m_hWnd = hWnd;

	this->UpdateClient();
	this->m_frame->insert(hWnd);
}

void UI::ClientPanel::UpdateClient()
{
	HWND hParent = reinterpret_cast<HWND>(this->m_frame->container()),
		hwnd = reinterpret_cast<HWND>(this->m_hWnd);

	SetParent(hwnd,hParent);

	LONG lStyle = GetWindowLong(hwnd, GWL_STYLE);
	lStyle &= ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZE | WS_MAXIMIZE | WS_SYSMENU);
	SetWindowLong(hwnd, GWL_STYLE, lStyle);

	LONG lExStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
	lExStyle &= ~(WS_EX_DLGMODALFRAME | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE);
	SetWindowLong(hwnd, GWL_EXSTYLE, lExStyle);

	auto sz = this->m_frame->size();
	SetWindowPos(hwnd,NULL,0,0,sz.width,sz.height,SWP_NOZORDER|SWP_FRAMECHANGED);
}
void UI::ClientPanel::AppendData(std::shared_ptr<ClientContext>& pData)
{
	this->m_clientContext = pData;
}