#pragma once
#include <map>
#include <string>
#include "ClientContext.h"

#include <nana\gui\wvl.hpp>
#include <nana\gui\widgets\widget.hpp>
#include "Host.h"

#include <CommunityLib\Host.h>

#define WIDGET_PTR(__w) (nana::gui::widget*)__w
#define ADD_WIDGET(__c,__w) this->m_mapControl[__c].reset(WIDGET_PTR(__w));
#define W_CAST(__t,__c) (__t*)this->m_mapControl[__c].get()

namespace UI
{
	enum class MAIN_CONTROL : uint8_t
	{
		INVALID,BUTTON,CLIENT_MANAGER
	};

	class MainWindow
	{
	public:
		MainWindow(void);
		~MainWindow(void);

		void Show();
	private:
		std::map<MAIN_CONTROL,std::shared_ptr<nana::gui::widget>> m_mapControl;
		nana::gui::form m_form;

		void InitHost(Plugin::Host& host,ClientContext &clientCtx);

		void NewClient();

	};
}

