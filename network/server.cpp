
#include "server.hpp"
#include "constants.hpp"
#include "topics.hpp"

using namespace SLNet;
using namespace rtu::topics;

namespace ezecs::network {
  Server::Server() {
    peer = RakPeerInterface::GetInstance();
    peer->SetMaximumIncomingConnections(static_cast<uint16_t>(maxServerConns));
    SocketDescriptor sock(static_cast<uint16_t>(serverPort), nullptr);
    StartupResult startResult = peer->Startup(maxServerConns, &sock, 1, EZECS_NET_THREAD_PRIORITY);
    if (startResult == RAKNET_STARTED) {
      peer->SetOccasionalPing(true);
      publish("log", "Server is ready to receive connections.");
    } else {
	    publish("err", "Server failed to start.");
    }
  }
  Server::~Server() {
    if (peer->IsActive()) {
      peer->Shutdown(100);
    }
	  publish("log", "Server is being destroyed.");
    RakPeerInterface::DestroyInstance(peer);
  }

# define CASE_REPORT_PACKET(e) case ID_ ##e: publishf("err", "Received %s\n", #e); break
  void Server::receive(std::vector<SLNet::Packet*> & requestBuffer, std::vector<SLNet::Packet*> & syncBuffer,
                       std::vector<SLNet::AddressOrGUID> & connectionBuffer) {
    Packet *packet;
    for (packet=peer->Receive(); packet; packet=peer->Receive()) {
      switch (packet->data[0]) {
        CASE_REPORT_PACKET(CONNECTION_BANNED);
        CASE_REPORT_PACKET(INVALID_PASSWORD);
        case ID_DISCONNECTION_NOTIFICATION:
        case ID_CONNECTION_LOST: {
          connectionListsDirty = true;
          clientSum -= RakNetGUID::ToUint32(AddressOrGUID(packet).rakNetGuid);
          publishf("log", "Client Disconnected: %s", AddressOrGUID(packet).systemAddress.ToString());
        } break;
        case ID_NEW_INCOMING_CONNECTION: {
          connectionListsDirty = true;
          connectionBuffer.emplace_back(AddressOrGUID(packet));
          clientSum += RakNetGUID::ToUint32(AddressOrGUID(packet).rakNetGuid);
	        publishf("log", "Client Connected: %s", connectionBuffer.back().systemAddress.ToString());
        } break;
        default: {
          if ((MessageID)packet->data[0] >= ID_USER_PACKET_SYNC_ENUM) {
            syncBuffer.emplace_back(packet);
            continue; // Do not deallocate - pointer to packet now in buffer
          } else if ((MessageID)packet->data[0] >= ID_USER_PACKET_ECS_REQUEST_ENUM) {
            requestBuffer.emplace_back(packet);
            continue; // Do not deallocate - pointer to packet now in buffer
          }
        }
      }
      deallocatePacket(packet);
    }
  }
# undef CASE_REPORT_PACKET

  void Server::updateConnectionLists() {
    if (connectionListsDirty) {
      peer->GetSystemList(addresses, guids);
      connectionListsDirty = false;
    }
  }

  void Server::tick(std::vector<SLNet::Packet*> & requestBuffer, std::vector<SLNet::Packet*> & syncBuffer,
                    std::vector<SLNet::AddressOrGUID> & connectionBuffer) {
    receive(requestBuffer, syncBuffer, connectionBuffer);
  }

  void Server::send(const BitStream &stream, PacketPriority priority, PacketReliability reliability, char channel) {
    peer->Send(&stream, priority, reliability, channel, UNASSIGNED_SYSTEM_ADDRESS, true);
  }

  void Server::sendTo(const BitStream &stream, const AddressOrGUID &target, PacketPriority priority,
                      PacketReliability reliability, char channel) {
    peer->Send(&stream, priority, reliability, channel, target, false);
  }

  void Server::deallocatePacket(Packet *packet) {
    peer->DeallocatePacket(packet);
  }

  const DataStructures::List<SLNet::SystemAddress> &Server::getClientAddresses() {
    updateConnectionLists();
    return addresses;
  }

  const DataStructures::List<SLNet::RakNetGUID> &Server::getClientGuids() {
    updateConnectionLists();
    return guids;
  }

  uint32_t Server::getClientSum() {
    return clientSum;
  }

  SLNet::RakNetGUID Server::getGuid() {
    return peer->GetMyGUID();
  }
}
