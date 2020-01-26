
#include "netInterface.hpp"
#include "topics.hpp"

using namespace SLNet;
using namespace rtu::topics;

namespace ezecs::network {

  void NetInterface::assumeRole(Role role, const char *str) {
  	dctxt.connect();
	  currentRole = role;
    switch(role) {
      case SERVER: {
        client.reset();
        server = std::make_unique<Server>();
        if (RakNetGUID::ToUint32(server->getGuid()) < 2) {
          publish("err", "Attempted server creation using reserved GUID - recreating.");
          assumeRole(); // Recurse until guid is acceptable.
        }
        dctxt.host();
      } break;
      case CLIENT: {
        server.reset();
        client = std::make_unique<Client>(str);
        if (RakNetGUID::ToUint32(client->getGuid()) < 2) {
	        publish("err", "Attempted client creation using reserved GUID - recreating.");
          assumeRole(); // Recurse until guid is acceptable.
        }
      } break;
      default: {
        server.reset();
        client.reset();
      }
    }
  }

  void NetInterface::discardPacketCollection(std::vector<SLNet::Packet *> &packets) {
    for (auto pack : packets) {
      switch(currentRole) {
        case SERVER: {
          server->deallocatePacket(pack);
        } break;
        case CLIENT: {
          client->deallocatePacket(pack);
        } break;
        default: break;
      }
    }
    packets.clear();
  }

  uint32_t NetInterface::getRole() const {
    return currentRole;
  }
  
  void NetInterface::list() {
  	dctxt.list();
  }

	void NetInterface::frnd(const char *name, const char *dscrm) {
		dctxt.frnd(name, dscrm);
	}

  void NetInterface::tick() {
  	dctxt.tick();
    switch(currentRole) {
      case SERVER: {
        server->tick(requestPackets, syncPackets, freshConnections);
      } break;
      case CLIENT: {
        client->tick(requestPackets, syncPackets);
      } break;
      default: break;
    }
  }

  void NetInterface::send(const BitStream &stream, PacketPriority priority,
                          PacketReliability reliability, char channel) {
    switch(currentRole) {
      case SERVER: {
        server->send(stream, priority, reliability, channel);
      } break;
      case CLIENT: {
        client->send(stream, priority, reliability, channel);
      } break;
      default: break;
    }
  }

  void NetInterface::sendTo(const SLNet::BitStream &stream, const SLNet::AddressOrGUID &target, PacketPriority priority,
                            PacketReliability reliability, char channel) {
    switch(currentRole) {
      case SERVER: {
        server->sendTo(stream, target, priority, reliability, channel);
      } break;
      case CLIENT: {
        client->sendTo(stream, target, priority, reliability, channel);
      } break;
      default: break;
    }
  }

  const std::vector<SLNet::Packet *> & NetInterface::getRequestPackets() const {
    return requestPackets;
  }

  void NetInterface::discardRequestPackets() {
    discardPacketCollection(requestPackets);
  }

  const std::vector<SLNet::Packet *> & NetInterface::getSyncPackets() const {
    return syncPackets;
  }

  void NetInterface::discardSyncPackets() {
    discardPacketCollection(syncPackets);
  }

  const std::vector<SLNet::AddressOrGUID> &NetInterface::getFreshConnections() const {
    return freshConnections;
  }

  void NetInterface::discardFreshConnections() {
    freshConnections.clear();
  }

  const DataStructures::List<SLNet::SystemAddress> & NetInterface::getClientAddresses() const {
    switch(currentRole) {
      case SERVER: { return server->getClientAddresses(); }
      case CLIENT:
      default: { return emptyAddresses; }
    }
  }

  const DataStructures::List<SLNet::RakNetGUID> & NetInterface::getClientGuids() const {
    switch(currentRole) {
      case SERVER: { return server->getClientGuids(); }
      case CLIENT:
      default: { return emptyGuids; }
    }
  }

  uint32_t NetInterface::getClientSum() const {
    switch(currentRole) {
      case SERVER: { return server->getClientSum(); }
      case CLIENT:
      default: { return 0; }
    }
  }

}
