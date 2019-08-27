#pragma once

#include <string>
#include <vector>

#pragma warning (disable : 4068 ) /* disable unknown pragma warnings */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"

#include "RakPeerInterface.h"
#include "MessageIdentifiers.h"
#include "BitStream.h"
#include "RakNetTypes.h"
#include "StringCompressor.h"

#pragma GCC diagnostic pop

namespace ezecs {
  class Client {
      SLNet::RakPeerInterface *peer;
      void connect(const char *serverAddress);
      void receive(std::vector<SLNet::Packet*> & requestBuffer, std::vector<SLNet::Packet*> & syncBuffer);
      static std::string getConnectionAttemptResultString(SLNet::ConnectionAttemptResult result);
    public:
      Client(const char *serverAddress);
      virtual ~Client();
      void tick(std::vector<SLNet::Packet*> & requestBuffer, std::vector<SLNet::Packet*> & syncBuffer);
      void send(const SLNet::BitStream &stream, PacketPriority priority, PacketReliability reliability, char channel);
      void sendTo(const SLNet::BitStream &stream, const SLNet::AddressOrGUID &target, PacketPriority priority,
                  PacketReliability reliability, char channel);
      void deallocatePacket(SLNet::Packet * packet);
      SLNet::RakNetGUID getGuid();
  };
}
