#pragma once
namespace Memory
{
class ToogleAble
{
public:
	ToogleAble(void)
	{
		this->m_fStatus = false;
	}

	virtual bool Enable(void);
	virtual bool Disable(void);
	virtual bool Toogle(void);
	virtual bool Toogle(bool fToogle);
	virtual bool Status();
protected:
	virtual bool enable(void)=0;
	virtual bool disable(void)=0;

private:
	bool m_fStatus;
};
}

