#ifndef WSOCKET_H
#define WSOCKET_H
/*=========================================================================*\
* Socket compatibilization module for Win32
* LuaSocket toolkit
*
* RCS ID: $Id: wsocket.h,v 1.4 2005/10/07 04:40:59 diego Exp $
\*=========================================================================*/

/*=========================================================================*\
* WinSock include files
\*=========================================================================*/
#ifdef _XBOX
#include <xtl.h>
#include <xbdm.h>
#else
#	ifdef _WIN32
		//#include <winsock.h>
		#include "Ws2tcpip.h"
#	endif
#endif

typedef int socklen_t;
typedef SOCKET t_socket;
typedef t_socket *p_socket;

#define SOCKET_INVALID (INVALID_SOCKET)

#endif /* WSOCKET_H */
