#include "MainWindow.h"
#include "ClientManagerControl.h"

#include <nana\gui\widgets\button.hpp>
#include <nana\gui\msgbox.hpp>

#include <CommunityLib\Detour.h>

#include <string>
#include <iostream>
#include <Windows.h>

#include "ClientContext.h"
#include "Context.h"

#include <CommunityLib\Writer.h>
#include <CommunityLib\Reader.h>

#include <CommunityLib\Accessor.h>
#include <CommunityLib\Detour.h>
#include <CommunityLib\Dispatcher.h>



using namespace UI;
using namespace nana::gui;

MainWindow::MainWindow(void):
	m_form(API::make_center(500,500))
{
	this->m_form.caption(STR("Community Trainer ~ Host"));

	auto btn = new button(m_form,nana::rectangle(5,5,100,20));
	btn->caption(STR("New Client"));
	btn->make_event<events::click>(std::bind(&MainWindow::NewClient,this));
	ADD_WIDGET(MAIN_CONTROL::BUTTON,btn);

	auto clientManager = new UI::ClientManagerControl(this->m_form,nana::rectangle(5,30,480,450));
	clientManager->background(nana::make_rgb(137,137,137));
	ADD_WIDGET(MAIN_CONTROL::CLIENT_MANAGER,clientManager);
}


MainWindow::~MainWindow(void)
{
	this->m_mapControl.clear();

}

void MainWindow::Show()
{
	this->m_form.show();
}

void MainWindow::NewClient()
{
	auto y = std::make_shared<ClientContext>();
	auto x = ContextInstance->ClientManager->StartClient(y);

	if(!x)
	{
		nana::gui::msgbox msgBox(this->m_form,STR("Error"),nana::gui::msgbox::ok);
		msgBox.icon(nana::gui::msgbox::icon_error);
		msgBox << STR("Failed to start the Client");
		msgBox.show();
		return;
	}

	auto man = W_CAST(UI::ClientManagerControl,MAIN_CONTROL::CLIENT_MANAGER);
	auto clientPanel = man->Append();
	clientPanel->AppendData(y);
	y->Panel = clientPanel;
	y->Memory.reset(new Memory::BasicAccessor(y->Process));
	y->Host.reset(new Plugin::Host());
	InitHost(*y->Host.get(),*y.get());
	
}


template<typename T> Util::nana::any PluginCtor(Plugin::parameter_t& p)
{
	return std::make_shared<T>();
}
template<typename T> Util::nana::any PluginCtor1(Plugin::parameter_t& p)
{
	if(p.size() < 1)
		throw std::exception("Invalid Parameter Amount");
	return std::make_shared<T>(p[0]);
}

void MainWindow::InitHost(Plugin::Host& host,ClientContext &clientCtx)
{
	using namespace Plugin;
	using namespace Util::nana;

	auto& var = *host()->variablePool.get();
	var("IsHost",std::make_shared<int>(1));
	var("IsClient",std::make_shared<int>(0));
	var("MemoryAccessor",clientCtx.Memory);

	auto& func = *host()->functionPool.get();
	func.Add("Writer",PluginCtor<IO::Writer>);
	func.Add("Reader",PluginCtor<IO::Reader>);
	func.Add("ReaderFromData",PluginCtor1<IO::Reader>);

	func.Add("Detour",PluginCtor1<Memory::Detour>);
	func.Add("DetourList",PluginCtor<Memory::DetourList>);
	func.Add("Dispatcher",PluginCtor<Memory::Dispatcher>);
}