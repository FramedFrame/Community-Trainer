#pragma once
#include <nana\gui\wvl.hpp>
#include <nana\gui\widgets\widget.hpp>
#include <nana\gui\widgets\panel.hpp>
#include <nana\gui\widgets\scroll.hpp>

#include "ClientPanel.h"

#include <map>
#include <memory.h>
#include <stdint.h>
#include <vector>
namespace UI
{
	class ClientManagerControl : public nana::gui::panel<true>
	{
	public:
		ClientManagerControl(nana::gui::window w,nana::rectangle& rect);
		~ClientManagerControl(void);

		std::shared_ptr<UI::ClientPanel> Append();
		void Remove(uint32_t nIndex);
	private:
		std::vector<std::shared_ptr<UI::ClientPanel>> m_vPanel;
		std::shared_ptr<nana::gui::scroll<true>> m_scroll;
		uint32_t m_uScrollState,m_uLines;
		static const int32_t PANEL_HEIGHT = 150,PANEL_WIDTH = 100;
		void Reorder();

		void Scrolled(widget& w);
	};
}

