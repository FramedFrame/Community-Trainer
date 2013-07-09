#pragma once
#include <vector>
namespace Plugin
{
	struct DetourContext
	{
		void** pFunc;
		void* pHook;

		bool operator==(DetourContext const& rhs)
		{
			return !memcmp(this,&rhs,sizeof(DetourContext));
		}

	};

	class HookManager
	{
	public:
		HookManager(void)
		{
			this->m_f = false;
		}
		~HookManager(void)
		{
			if(this->m_f)
				this->Toogle(false);
		}

		virtual bool Toogle(bool f);
		virtual bool Toogle();

		virtual void Add(DetourContext dCtx);
		virtual void Remove(DetourContext dCtx);
		virtual void Clear();
	private:
		std::vector<Plugin::DetourContext> m_vHooks;
		bool m_f;
	};
}
