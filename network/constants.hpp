#pragma once

#ifdef WIN32
	# define EZECS_NET_THREAD_PRIORITY THREAD_PRIORITY_NORMAL
#else
	# include <sched.h>
	# define EZECS_NET_THREAD_PRIORITY SCHED_OTHER
#endif

#include "MessageIdentifiers.h"

namespace ezecs::network {
	
	constexpr uint32_t serverPort = 22022; // the entire range 22022-22122 is available
	constexpr uint32_t clientPort = 0; // 0 means OS will pick a port when the client starts. 22122 is option for future.
	constexpr uint32_t maxServerConns = 150;

	enum Role {
		NONE,
		SERVER,
		CLIENT
	};
	enum PacketEnums {
		ID_USER_PACKET_ECS_REQUEST_ENUM = ID_USER_PACKET_ENUM,
		ID_USER_PACKET_ECS_RESPONSE_ENUM,
		ID_USER_PACKET_ADMIN_COMMAND,
		ID_USER_PACKET_CLIENT_READY,

		ID_USER_PACKET_SYNC_ENUM
	};
}
