
#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include "discord.h"

namespace ezecs::network {
	class DiscordContext {
			struct State {
				discord::User currentUser;
				std::unique_ptr<discord::Core> core;
			} state;
			struct Friend {
				discord::UserId id;
				discord::Status status;
			};
			std::map<std::string, std::map<std::string, Friend>> friends;
			void updateFriends(std::map<std::string, std::vector<std::string>> *statToName = nullptr);
			std::string lastFrndName, lastFrndDscrm;
			
		public:
			bool connect();
			void tick();
			bool host();
			bool join();
			void list();
			void frnd(const char *name, const char *dscrm);
			bool ovrl();
	};	
}