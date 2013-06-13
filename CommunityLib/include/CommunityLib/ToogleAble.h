#pragma once
namespace Memory
{
class ToogleAble
{
public:
	ToogleAble(void);
	~ToogleAble(void);
	bool Enable(void);
	bool Disable(void);
	bool Toogle(void);
	bool Toogle(bool fToogle);
protected:
	virtual bool enable(void)=0;
	virtual bool disable(void)=0;

private:
	bool m_fStatus;
};
}

