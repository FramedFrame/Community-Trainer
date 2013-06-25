#pragma once
#include <nana\gui\wvl.hpp>
#include <nana\gui\widgets\button.hpp>
#include <nana\gui\widgets\widget.hpp>
#include <nana\gui\widgets\panel.hpp>
#include <nana\gui\widgets\frame.hpp>

#include <map>
#include <memory.h>
#include <stdint.h>
#include <vector>

struct ClientContext;

namespace UI
{
	class ClientPanel : public nana::gui::panel<true>
	{
	public:
		ClientPanel(nana::gui::window w,nana::rectangle& rect);
		~ClientPanel(void);

		void AttachWindow(nana::gui::native_window_type hWnd);
		void UpdateClient();

		void AppendData(std::shared_ptr<ClientContext>& pData);
	private:
		std::shared_ptr<nana::gui::frame> m_frame;
		std::shared_ptr<nana::gui::panel<true>> m_controlPanel;
		std::shared_ptr<ClientContext> m_clientContext;
		std::vector<std::shared_ptr<nana::gui::button>> m_vButton;
		nana::gui::native_window_type m_hWnd;

	};
}

