
#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include "discord.h"

namespace ezecs::network {
	class DiscordContext {
		public:
			bool connect();
			void tick();
			bool host();
			bool join();
			void list();
			void frnd(const char *name, const char *dscrm);
			bool ovrl();
			
			struct Friend {
				discord::UserId id;
				discord::Status status;
				static std::map<std::string, std::vector<std::string>> statusList;
			};
			typedef std::map<std::string, std::map<std::string, Friend>> friendsList;
			
		private:
			struct State {
				discord::User currentUser;
				std::unique_ptr<discord::Core> core;
			} state;
			friendsList friends;
			void updateFriends();
			std::string lastFrndName, lastFrndDscrm;
	};	
}