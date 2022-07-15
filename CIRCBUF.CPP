#include "stdafx.h"
#include <string.h>
#include <stdlib.h>
#include "CircBuf.h"



/////////////////////////////////////////////////////////////////////////////
CCircBuf::CCircBuf()
{
	m_lpBuf      = NULL;
	m_iSize      = 0;
	m_iReadPos   = 0;
	m_iWritePos  = 0;
}

/////////////////////////////////////////////////////////////////////////////
CCircBuf::CCircBuf(int uiSize)
{
	m_lpBuf      = NULL;
	m_iSize      = 0;
	m_iReadPos   = 0;
	m_iWritePos  = 0;
	SetBuffer(uiSize);
}

/////////////////////////////////////////////////////////////////////////////
CCircBuf::~CCircBuf()
{
	RemoveBuffer();
}

/////////////////////////////////////////////////////////////////////////////
int CCircBuf::GetReadBufferPos()
{
	if (!m_lpBuf)
		return 0;
	return m_iReadPos;
}

/////////////////////////////////////////////////////////////////////////////
int CCircBuf::GetWriteBufferPos()
{
	if (!m_lpBuf)
		return 0;
	return m_iWritePos;
}

/////////////////////////////////////////////////////////////////////////////
DWORD CCircBuf::SizeBuffered()
{
	if (m_iReadPos<=m_iWritePos)
		return (DWORD)(m_iWritePos - m_iReadPos);
	else
		return (DWORD)(m_iWritePos - m_iReadPos + m_iSize);
}

/////////////////////////////////////////////////////////////////////////////
int CCircBuf::SizeFree()
{
	return m_iSize - SizeBuffered();
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCircBuf::SetBuffer(int iSize)
{
	m_iReadPos   = 0;
	m_iWritePos  = 0;

	if (m_lpBuf)
		return FALSE;
	m_lpBuf = new char[m_iSize = iSize];

	return (m_lpBuf!=NULL);
}

/////////////////////////////////////////////////////////////////////////////
void CCircBuf::RemoveBuffer()
{
	if(m_lpBuf)
	{
		delete [] m_lpBuf;

		m_lpBuf      = NULL;
		m_iSize      = 0;
		m_iReadPos   = 0;
		m_iWritePos  = 0;
	}
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCircBuf::WriteString(void *lpWriteBuf)
{
	return ( Write(lpWriteBuf,(int)strlen((char*)lpWriteBuf)) );
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCircBuf::ReadString(void *lpReadBuf, int iBytes)
{
	BOOL bRval = Read(lpReadBuf, iBytes);
	if (bRval)
		((char*)lpReadBuf)[iBytes] = '\0';
	return(bRval);
}

/////////////////////////////////////////////////////////////////////////////
char CCircBuf::ReadChar()
{
	char ch;
	BOOL bRval = Read(&ch, 1);
	if (bRval)
		return(ch);
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCircBuf::WriteChar(char ch)
{
	return(Write(&ch, 1));
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCircBuf::Read(void *lpReadBuf, int iBytes)
{
	if (m_lpBuf==NULL) return FALSE;
	if (!iBytes) return TRUE;
	if (iBytes<0) return FALSE;
	if (m_iReadPos==m_iWritePos) return(!iBytes);

	if (m_iReadPos<m_iWritePos)
	{
		if(m_iReadPos+iBytes>m_iWritePos) return FALSE;
		memcpy(lpReadBuf, (char*)m_lpBuf+m_iReadPos, iBytes);
		m_iReadPos+=iBytes;
	}
	else
	{
		int i2BufEnd = m_iSize-m_iReadPos;
		if (iBytes-i2BufEnd>m_iWritePos) return FALSE;
		if (iBytes<i2BufEnd)
		{
			memcpy(lpReadBuf, (char*)m_lpBuf+m_iReadPos, iBytes);
			m_iReadPos += iBytes;
		}
		else
		{
			memcpy(lpReadBuf, (char*)m_lpBuf+m_iReadPos, i2BufEnd);
			memcpy((char*)lpReadBuf+i2BufEnd, m_lpBuf, iBytes-i2BufEnd);
			m_iReadPos = iBytes-i2BufEnd;
		}
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCircBuf::ReadWithoutRemoving(void *lpReadBuf, int iBytes)
{
	if (m_lpBuf==NULL) return FALSE;
	if (!iBytes) return TRUE;
	if (iBytes<0) return FALSE;
	if (m_iReadPos==m_iWritePos) return(!iBytes);

	if (m_iReadPos<m_iWritePos)
	{
		if(m_iReadPos+iBytes>m_iWritePos) return FALSE;
		memcpy(lpReadBuf, (char*)m_lpBuf+m_iReadPos, iBytes);
	}
	else
	{
		int i2BufEnd = m_iSize-m_iReadPos;
		if (iBytes-i2BufEnd>m_iWritePos) return FALSE;
		if (iBytes<i2BufEnd)
		{
			memcpy(lpReadBuf, (char*)m_lpBuf+m_iReadPos, iBytes);
		}
		else
		{
			memcpy(lpReadBuf, (char*)m_lpBuf+m_iReadPos, i2BufEnd);
			memcpy((char*)lpReadBuf+i2BufEnd, m_lpBuf, iBytes-i2BufEnd);
		}
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCircBuf::Write(void *lpWriteBuf, int iBytes)
{
	if (m_lpBuf==NULL)
		return FALSE;
	if (!iBytes) return TRUE;
	if (iBytes<0) return FALSE;

	if (m_iWritePos<m_iReadPos)
	{
		if(m_iWritePos+iBytes>=m_iReadPos) return FALSE;
		memcpy((char*)m_lpBuf+m_iWritePos, lpWriteBuf, iBytes);
		m_iWritePos+=iBytes;
	}
	else
	{
		int i2BufEnd = m_iSize-m_iWritePos;
		if (iBytes-i2BufEnd>=m_iReadPos) return FALSE;
		if (iBytes<i2BufEnd)
		{
			memcpy((char*)m_lpBuf+m_iWritePos, lpWriteBuf, iBytes);
			m_iWritePos+=iBytes;
		}
		else
		{
			memcpy((char*)m_lpBuf+m_iWritePos, lpWriteBuf, i2BufEnd);
			memcpy(m_lpBuf, (char*)lpWriteBuf+i2BufEnd, iBytes-i2BufEnd);
			m_iWritePos = iBytes-i2BufEnd;
		}
	}
	return TRUE;
}