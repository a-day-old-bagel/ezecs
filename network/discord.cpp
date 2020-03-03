
#include <algorithm>
#include "discord.hpp"
#include "topics.hpp"

using namespace rtu::topics;
using namespace discord;

namespace ezecs::network {
	
	static constexpr auto appId = 667545903715975168;
	
	static std::string toString(discord::Status statusNum) { 
		switch(statusNum) {
			case discord::Status::Online: return "ACTIVE";
			case discord::Status::Idle: return "IDLE";
			case discord::Status::DoNotDisturb: return "NO DISTURB";
			case discord::Status::Offline: return "OFFLINE";
			default: return "UNKNOWN";
		}
	}
	static std::string toString(const discord::Activity &activity) {
		std::string verb;
		std::string object = appId == activity.GetApplicationId() ? "Precession" : "something";
		switch(activity.GetType()) {
			case discord::ActivityType::Listening: { verb = "listening to"; break; }
			case discord::ActivityType::Playing: { verb = "playing"; break; }
			case discord::ActivityType::Streaming: { verb = "streaming"; break; }
			case discord::ActivityType::Watching: { verb = "watching"; break; }
			default: { verb = "online"; object = ""; }
		}
		return verb + " " + object;
	}
	static std::string toString(discord::LogLevel levelNum) {
		switch(levelNum) {
			case discord::LogLevel::Info: return "INFO";
			case discord::LogLevel::Debug: return "DEBUG";
			case discord::LogLevel::Warn: return "WARNING";
			case discord::LogLevel::Error: return "ERROR";
			default: return "UNKNOWN";
		}
	}
	
	bool isBad(char c) {
		bool visibleAscii = (c >= 33 && c <= 126);
		return !visibleAscii;
	}
	static std::string asciiNoSpaces(std::string str) {
		std::replace_if(str.begin(), str.end(), isBad, ':');
		return str;
	}

	std::map<std::string, std::vector<std::string>> DiscordContext::Friend::statusList;

	bool DiscordContext::connect() {
		Core* core{};
		auto result = Core::Create(appId, DiscordCreateFlags_NoRequireDiscord, &core);
		state.core.reset(core);
		if ( ! state.core) {
			publishf("err", "Failed to instantiate discord core! (err %i)", result);
			return false;
		}
		state.core->SetLogHook(discord::LogLevel::Info, [](discord::LogLevel level, const char *message) {
			publishf("err", "DISCORD %s: %s", toString(level).c_str(), message);
			if (level == discord::LogLevel::Error) {
				publish("err", "\nPlease make sure you're logged into discord.");
			}
		});
		state.core->UserManager().OnCurrentUserUpdate.Connect([&]() {
			state.core->UserManager().GetCurrentUser(&state.currentUser);
		});
		updateFriends();
		return true;
	}
	
	void DiscordContext::tick() {
		if (state.core) {
			state.core->NetworkManager().Flush();
			state.core->RunCallbacks();
		}
	}
	bool DiscordContext::host() {
		discord::LobbyTransaction lobby{};
		state.core->LobbyManager().GetLobbyCreateTransaction(&lobby);
		lobby.SetCapacity(6);
		lobby.SetMetadata("name", "Precession Server");
		lobby.SetType(discord::LobbyType::Public);
		state.core->LobbyManager().CreateLobby(lobby, [&](discord::Result result, discord::Lobby const &lby) {
			if (result == discord::Result::Ok) {
				publishf("log", "Created lobby, secret is \"%s\"", lby.GetSecret());
			} else {
				publishf("err", "Failed to create lobby with error %i.", result);
			}
		});
		return true;
	}
	bool DiscordContext::join() {
		return false;
	}
	void DiscordContext::list() {
		discord::LobbySearchQuery query{};
		state.core->LobbyManager().GetSearchQuery(&query);
		query.Limit(1);
		state.core->LobbyManager().Search(query, [&](discord::Result result) {
			if (result == discord::Result::Ok) {
				std::int32_t lobbyCount{};
				state.core->LobbyManager().LobbyCount(&lobbyCount);
				publishf("log", "Lobby search found %i lobbies:", lobbyCount);
				for (auto i = 0; i < lobbyCount; ++i) {
					discord::LobbyId lobbyId{};
					state.core->LobbyManager().GetLobbyId(i, &lobbyId);
					char name[4096];
					state.core->LobbyManager().GetLobbyMetadataValue(lobbyId, "name", name);
					publishf("log", "%20li  %s", lobbyId, name);
				}
			} else {
				publishf("err", "Lobby search failed with error %i.", result);
			}
		});
	}
	void DiscordContext::frnd(const char *name, const char *dscrm) {
		if (name) { lastFrndName = std::string(name); } else { lastFrndName = std::string(); }
		if (dscrm) { lastFrndDscrm = std::string(dscrm); } else { lastFrndDscrm = std::string(); }
		updateFriends();
		if ( ! lastFrndName.empty()) {
			UserId idToGet = 0;
			uint32_t numMatches = friends.count(lastFrndName);
			if (numMatches > 1) {
				if ( ! lastFrndDscrm.empty() && friends[lastFrndName].count(lastFrndDscrm)) {
					idToGet = friends[lastFrndName][lastFrndDscrm].id;
				} else {
					publishf("log", "Multiple %s. Try again with name and number (separated by a space).", lastFrndName.c_str());
					return;
				}
			} else if (numMatches) {
				idToGet = friends[lastFrndName].begin()->second.id;
			} else {
				publishf("err", "Could not find user %s.", lastFrndName.c_str());
				return;
			}
			discord::Relationship relat{};
			if (discord::Result::Ok == state.core->RelationshipManager().Get(idToGet, &relat)) {
				publishf("log", "%s is %s.", asciiNoSpaces(relat.GetUser().GetUsername()).c_str(),
				         toString(relat.GetPresence().GetStatus()).c_str());
			} else {
				publishf("err", "Could not find %s.", lastFrndName.c_str());
			}
		} else {
			publish("friend_info", friends);
		}
	}
	bool DiscordContext::ovrl() {
		return false;
	}


	void DiscordContext::updateFriends() {
		state.core->RelationshipManager().Filter([](discord::Relationship const &relationship) -> bool {
			return relationship.GetType() == discord::RelationshipType::Friend;
		});
		std::int32_t friendCount{0};
		state.core->RelationshipManager().Count(&friendCount);
		discord::Relationship relat{};
		Friend::statusList.clear();
		for (auto i = 0; i < friendCount; ++i) {
			state.core->RelationshipManager().GetAt(i, &relat);
			std::string status = "UNKNOWN";
			if (relat.GetPresence().GetActivity().GetApplicationId() == appId) { status = " PRECESSING"; }
			else { status = toString(relat.GetPresence().GetStatus()); }
			friends[asciiNoSpaces(relat.GetUser().GetUsername())][relat.GetUser().GetDiscriminator()] = {
						relat.GetUser().GetId(), relat.GetPresence().GetStatus(),
			};
			Friend::statusList[status].emplace_back(
						(asciiNoSpaces(relat.GetUser().GetUsername()) + " " +  std::string(relat.GetUser().GetDiscriminator()))
						.c_str());
		}
	}
}
