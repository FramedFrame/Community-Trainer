#include "MainWindow.h"

#include <nana\gui\widgets\button.hpp>
#include <nana\gui\msgbox.hpp>

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
}


MainWindow::~MainWindow(void)
{
}

void MainWindow::Show()
{
	this->m_form.show();
}

void MainWindow::NewClient()
{

	msgbox msg(this->m_form,STR("Info - Button Clicked"),msgbox::yes_no);
	msg << STR("Confirm that you want to spawn a new Client");
	auto x = msg.show();

	if(x == msgbox::pick_yes)
	{
		msgbox msg2(this->m_form,STR("Info - Button Clicked"),msgbox::ok);
		msg2.icon(msgbox::icon_information);
		msg2 << STR("About to Start the Client...");
		msg2.show();
	}
}