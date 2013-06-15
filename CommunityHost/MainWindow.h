#pragma once
#include <map>
#include <string>

#include <nana\gui\wvl.hpp>
#include <nana\gui\widgets\widget.hpp>


#define WIDGET_PTR(__w) (nana::gui::widget*)__w
#define ADD_WIDGET(__c,__w) this->m_mapControl[__c].reset(WIDGET_PTR(__w));

namespace UI
{
	enum class MAIN_CONTROL
	{
		INVALID,BUTTON
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

		void NewClient();

	};
}

