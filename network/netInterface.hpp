#pragma once

#include "server.hpp"
#include "client.hpp"
#include "constants.hpp"

#include <memory>

namespace ezecs::network {
	
	class NetInterface {

		public:

			void assumeRole(Role role = NONE, const char *str = nullptr);
			[[nodiscard]] uint32_t getRole() const;

			void tick();
			void send(const SLNet::BitStream &stream, PacketPriority priority = LOW_PRIORITY,
			          PacketReliability reliability = UNRELIABLE, char channel = 0);
			void sendTo(const SLNet::BitStream &stream, const SLNet::AddressOrGUID &target,
			            PacketPriority priority = LOW_PRIORITY, PacketReliability reliability = UNRELIABLE, char channel = 0);

			[[nodiscard]] const std::vector<SLNet::Packet *> &getRequestPackets() const;
			void discardRequestPackets();
			[[nodiscard]] const std::vector<SLNet::Packet *> &getSyncPackets() const;
			void discardSyncPackets();
			[[nodiscard]] const std::vector<SLNet::AddressOrGUID> &getFreshConnections() const;
			void discardFreshConnections();

			[[nodiscard]] const DataStructures::List<SLNet::SystemAddress> &getClientAddresses() const;
			[[nodiscard]] const DataStructures::List<SLNet::RakNetGUID> &getClientGuids() const;
			[[nodiscard]] uint32_t getClientSum() const;

		private:

			std::unique_ptr<Server> server;
			std::unique_ptr<Client> client;
			std::vector<SLNet::Packet *> requestPackets;
			std::vector<SLNet::Packet *> syncPackets;
			std::vector<SLNet::AddressOrGUID> freshConnections;
			uint32_t currentRole = NONE;
			DataStructures::List<SLNet::SystemAddress> emptyAddresses;
			DataStructures::List<SLNet::RakNetGUID> emptyGuids;
			
			void discardPacketCollection(std::vector<SLNet::Packet *> &packets);

	};
}
