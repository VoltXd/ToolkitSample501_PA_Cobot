/////////////////////////////////////////////////////////////////////////////
//	[CircBuf.h]
//	Circular Buffer :
//
//	So there's complete wrap-around checking and it's
//	taken care that you cannot read further than you have
//	written.
//
//	It's not thread safe (yet), so you've got to add your
//	own critical sections.
//
/////////////////////////////////////////////////////////////////////////////
#ifndef __KR_CIRCBUF__
#define __KR_CIRCBUF__

#define KR_CIRCBUF_SIZE 9000000

/////////////////////////////////////////////////////////////////////////////
class CCircBuf
{
public:
	CCircBuf();
	CCircBuf(int iSize);
	virtual ~CCircBuf();

	int		GetWriteBufferPos();
	int		GetReadBufferPos();
	BOOL	Write(void* lpWriteBuf, int iBytes);
	BOOL	WriteString(void* lpWriteBuf);
	BOOL	WriteChar(char ch);
	BOOL	Read(void* lpReadBuf, int iBytes);
	BOOL	ReadWithoutRemoving(void *lpReadBuf,int iBytes);
	BOOL	ReadString(void *lpReadBuf, int iBytes);
	char	ReadChar();
	void	RemoveBuffer();
	BOOL	SetBuffer(int iSize);
	DWORD	SizeBuffered();
	int		SizeFree();

private:
	int		m_iSize;
	int		m_iWritePos;
	int		m_iReadPos;
	void*	m_lpBuf;
};



#endif