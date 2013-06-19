#include "MainWindow.h"
#include "ClientManagerControl.h"

#include <nana\gui\widgets\button.hpp>
#include <nana\gui\msgbox.hpp>

#include <string>
#include <iostream>
#include <Windows.h>

#include "Context.h"


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
	ContextInstance->ClientManager->StartClient();

	auto man = W_CAST(UI::ClientManagerControl,MAIN_CONTROL::CLIENT_MANAGER);
	auto y = man->Append();
	y->background(nana::make_rgb(0,0,0));
	HWND h =FindWindow(NULL,L"MapleStory");
	y->AttachWindow((native_window_type)h);
}