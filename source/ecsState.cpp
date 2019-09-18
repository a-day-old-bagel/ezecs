/*
 * Copyright (c) 2016 Galen Cochrane
 * Galen Cochrane <galencochrane@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */
#include <limits>
#include "ecsState.generated.hpp"
#include "ecsHelpers.hpp"

#pragma clang diagnostic ignored "-Wundefined-var-template"

using namespace rtu::topics;
using namespace SLNet;

namespace ezecs {


	void State::openEntityRequest() {
		if (entityRequestOpen) {
			publish("err", "Entity request is already open. Cannot open again until finalized!");
		} else {
			entityRequestOpen = true;
			if (net.getRole() == network::NONE) {
				createEntity(&openRequestId);
			} else {
				stream.Reset();
				for (auto &stream : compStreams) {
					stream->Reset();
				}
			}
		}
	}

	entityId State::closeEntityRequest() {
		entityId id = 0;
		if (entityRequestOpen) {
			entityRequestOpen = false;
			if (net.getRole() == network::NONE) {
				return openRequestId;
			}
			writeEntityRequestHeader(stream);
			switch (net.getRole()) {
				case network::SERVER: {
					BitStream headerless;
					serializeEntityCreationRequest(true, headerless, 0, &compStreams); // ID 0 = request without action
					id = serializeEntityCreationRequest(false, headerless); // treat it as if it came from a client
					stream.Write(headerless);
				} break;
				case network::CLIENT: {
					serializeEntityCreationRequest(true, stream, 0, &compStreams); // ID 0 = request without action
				} break;
				default: break;
			}
			net.send(stream, LOW_PRIORITY, RELIABLE_ORDERED, network::CH_ECS_UPDATE);
		} else {
			publish("err", "Close entity request: No entity request was open!");
		}
		// TODO: for a client request, instead of returning 0, return a local-end ID that can later be replaced.
		return id;
	}

	void State::broadcastManualEntity(const entityId &id) {
		// Solo's don't need to do this, and clients should not do this. This might be a redundant check, though.
		if (net.getRole() == network::SERVER) {
			stream.Reset();
			writeEntityRequestHeader(stream);
			serializeEntityCreationRequest(true, stream, id);
			net.send(stream, LOW_PRIORITY, RELIABLE_ORDERED, network::CH_ECS_UPDATE);
		}
	}

	void State::requestEntityDeletion(const entityId &id) {
		switch (net.getRole()) {
			case network::SERVER: {
				stream.Reset();
				serializeEntityDeletionRequest(true, stream, id);
				net.send(stream, LOW_PRIORITY, RELIABLE_ORDERED, network::CH_ECS_UPDATE);

				EZECS_VERBOSE(deleteEntity(id));
			} break;
			case network::CLIENT: {
				// TODO: Along with entity creation, decide if clients should be able to send these at all.
				// stream.Reset();
				// serializeEntityDeletionRequest(true, stream, id);
				// net.send(stream, IMMEDIATE_PRIORITY, RELIABLE_ORDERED, network::CH_ECS_UPDATE);
			} break;
			default: {
				deleteEntity(id);
			} break;
		}
	}

	void State::writeEntityRequestHeader(BitStream &stream) {
		stream.Write((MessageID)network::ID_USER_PACKET_ECS_REQUEST_ENUM);
		stream.WriteBitsFromIntegerRange((uint8_t)network::REQ_ENTITY_OP, 
					(uint8_t)0, (uint8_t)(network::REQ_END_ENUM - 1), false);
		stream.WriteBitsFromIntegerRange((uint8_t)network::OP_CREATE, 
					(uint8_t)0, (uint8_t)(network::OP_END_ENUM - 1), false);
	}

	entityId State::serializeEntityCreationRequest(bool rw, BitStream &stream, entityId id,
	                                        std::vector<std::unique_ptr<BitStream>> *compStreams) {
		stream.Serialize(rw, id);
		if (!rw) {  // reading a request
			if (id) { // Receive a remote server request to change the local client ECS, which gets fulfilled.

				publishf("log", "received entity creation request for %u\n", id);

				// If outgoing ECS request server messages are ordered, we shouldn't need to check for inappropriate id.
				// That means this loop should always do exactly one iteration, but if it doesn't, we need to fix it.
				int i = 0;
				for (; !getComponents(id) && i < 10; ++i) {  // times out after 10 loops if there IS an anomaly
					createEntity();
				}
				if (i != 1) { // TODO: make this an assert?
					publishf("err", "Anomaly found while processing entity creation request! Check logic (i was %i)!", i);
				}
				serializeComponentCreationRequest(false, stream, id);
			} else { // Receive a client's request to update all networked ECS's. The server fulfills it and rebroadcasts.
				createEntity(&id);
				serializeComponentCreationRequest(false, stream, id);
				stream.Reset(); // Rewrite the passed-in stream to be the rebroadcasted command that includes the new ID.
				serializeEntityCreationRequest(true, stream, id); // Recurse to accomplish this.
			}
		} else {  // writing a request
			if (id) {
				if (getComponents(id)) {
					serializeComponentCreationRequest(true, stream, id);
				} else {  // TODO: make this an assert?
					stream.Reset();
					publish("err", "Attempted to write entity creation request using invalid ID!");
				}
			} else {  // This is a request made without actually adding anything to your own ECS (a client does this)
				if (compStreams) {
					serializeComponentCreationRequest(false, stream, 0, compStreams);
				} else {
					stream.Reset();
					publishf("err", "Passed a nullptr as compStreams when creating an unfulfilled ECS request!");
				}
			}
		}
		return id;
	}

	void State::serializeComponentCreationRequest(bool rw, BitStream &stream, entityId id,
	                                       std::vector<std::unique_ptr<BitStream>> *compStreams) {
		// SERIALIZE COMPONENT CREATION REQUEST DEFINITION BODY APPEARS HERE
	}

	entityId State::serializeEntityDeletionRequest(bool rw, BitStream &stream, entityId id) {
		if (rw) {
			stream.Write((MessageID)network::ID_USER_PACKET_ECS_REQUEST_ENUM);
			stream.WriteBitsFromIntegerRange((uint8_t)network::REQ_ENTITY_OP, (uint8_t)0, (uint8_t)(network::REQ_END_ENUM - 1), false);
			stream.WriteBitsFromIntegerRange((uint8_t)network::OP_DESTROY, (uint8_t)0, (uint8_t)(network::OP_END_ENUM - 1), false);
		}
		stream.Serialize(rw, id);
		if (! rw) {
			EZECS_VERBOSE(deleteEntity(id));
		}
		return id;
	}

	bool State::hasComponent(bool rw, BitStream &stream, const entityId &id, const compMask &type) {
		bool hasComp;
		if (rw) { hasComp = (bool) (getComponents(id) & type); }
		stream.SerializeCompressed(rw, hasComp);
		return hasComp;
	}
	

  CompOpReturn State::createEntity(entityId *newId) {
    entityId id;
    if (freedIds.empty()) {
      if (nextId == std::numeric_limits<entityId>::max() || ++nextId == std::numeric_limits<entityId>::max()) {
        if (newId) {
          *newId = 0; // ID 0 is not a valid id - ids start at 1.
        }
        return MAX_ID_REACHED;
      }
      id = nextId;
    } else {
      id = freedIds.top();
      freedIds.pop();
    }
    Existence *existence = &comps_Existence[id];
    existence->turnOnFlags(Existence::flag);
    if (newId) {
      *newId = id;
    }
    return SUCCESS;
  }

  CompOpReturn State::clearEntity(const entityId& id) {
    Existence* existence;
    CompOpReturn status = getExistence(id, &existence);
    if (status != SUCCESS) {
      return NONEXISTENT_ENT; // only fail status possible here indicates no existence component, hence no entity.
    }

    // A LOOP TO CLEAR ALL COMPONENTS APPEARS HERE

    if (existence->componentsPresent != Existence::flag) {
      return SOMETHING_REALLY_BAD;
    }
    return SUCCESS;
  }

  CompOpReturn State::deleteEntity(const entityId& id) {
    CompOpReturn cleared = clearEntity(id);
    if (cleared != SUCCESS) {
      return cleared;
    }
    comps_Existence.erase(id);
    freedIds.push(id);
    return SUCCESS;
  }

  void State::listenForLikeEntities(const compMask& likeness,
                                    EntNotifyDelegate&& additionDelegate, EntNotifyDelegate&& removalDelegate)
  {
    // CODE TO REGISTER THE APPROPRIATE CALLBACKS APPEARS HERE
  }

  compMask State::getComponents(const entityId& id) {
    Existence* existence;
    if (getExistence(id, &existence) == SUCCESS) {
      return existence->componentsPresent;
    }
    return 0; // does not return compOpReturn, so a 0 indicates no components (even existence) present.
  }

  entityId State::getNextId() {
    return nextId;
  }

	KvMap<entityId, Existence> State::getDump() const {
  	return comps_Existence;
  }
	const KvMap<entityId, Existence> &State::getDumpRef() const {
		return comps_Existence;
	}

  void State::clear() {
    std::vector<entityId> idsToErase;
    for (auto pair : comps_Existence) {
    	if ( ! (pair.second.componentsPresent & persistenceMask)) {
		    idsToErase.push_back(pair.first);
    	}
    }
    for (auto id : idsToErase) {
      deleteEntity(id);
    }
  }
  
  template<typename compType>
  inline CompOpReturn State::remCompNoChecks(KvMap<entityId, compType>& coll, Existence* existence,
                                                      const entityId& id, const EntNotifyDelegates& callbacks)
  {
    for (auto dlgt : callbacks) {
      if (shouldFireRemovalDlgt(dlgt.likeness, existence->componentsPresent, compType::flag)) {
        dlgt.fire(id);
      }
    }
    coll.erase(id);
    existence->turnOffFlags(compType::flag);
    return SUCCESS;
  }

  template<typename compType, typename ... types>
  inline CompOpReturn State::addComp(KvMap<entityId, compType>& coll, const entityId& id,
                                     const EntNotifyDelegates& callbacks, const types &... args)
  {
	  if (comps_Existence.count(id)) {
		  Existence* existence = &comps_Existence.at(id);
		  if (existence->passesPrerequisitesForAddition(compType::requiredComps)) {
			  bool wasEmplaced = coll.try_emplace(id, args...);
		  	if (wasEmplaced) {
				  for (auto dlgt : callbacks) {
					  if (shouldFireAdditionDlgt(dlgt.likeness, existence->componentsPresent, compType::flag)) {
						  dlgt.fire(id);
					  }
				  }
				  existence->turnOnFlags(compType::flag);
				  return SUCCESS;
		  	}
			  return REDUNDANT;
		  }
		  return PREREQ_FAIL;
	  }
	  return NONEXISTENT_ENT;
  }

	template<typename compType>
	inline CompOpReturn State::insertComp(KvMap<entityId, compType>& coll, const entityId& id,
	                                      const EntNotifyDelegates& callbacks, compType && input)
	{
		if (comps_Existence.count(id)) {
			Existence* existence = &comps_Existence.at(id);
			if (existence->passesPrerequisitesForAddition(compType::requiredComps)) {
				bool wasInserted = coll.insert(id, std::forward<compType>(input));
				if (wasInserted) {
					for (auto dlgt : callbacks) {
						if (shouldFireAdditionDlgt(dlgt.likeness, existence->componentsPresent, compType::flag)) {
							dlgt.fire(id);
						}
					}
					existence->turnOnFlags(compType::flag);
					return SUCCESS;
				}
				return REDUNDANT;
			}
			return PREREQ_FAIL;
		}
		return NONEXISTENT_ENT;
	}

  template<typename compType>
  inline CompOpReturn State::remComp(KvMap<entityId, compType>& coll, const entityId& id,
                                     const EntNotifyDelegates& callbacks)
  {
    if (comps_Existence.count(id)) {
      if (coll.count(id)) {
        Existence* existence = &comps_Existence.at(id);
        if (existence->passesDependenciesForRemoval(compType::dependentComps)) {
          return remCompNoChecks(coll, existence, id, callbacks);
        }
        return DEPEND_FAIL;
      }
      return NONEXISTENT_COMP;
    }
    return NONEXISTENT_ENT;
  }

  template<typename compType>
  inline CompOpReturn State::getComp(KvMap<entityId, compType> &coll, const entityId& id, compType** out) {
    if (coll.count(id)) {
      *out = &coll.at(id);
      return SUCCESS;
    }
    *out = NULL; // hopefully a null pointer exception will catch some bugs if somebody uses this wrong.
    return NONEXISTENT_COMP;
  }

  inline bool State::shouldFireRemovalDlgt(const compMask& likeness, const compMask& current,
                                           const compMask& typeRemoved)
  {
    if ( (likeness & current) == likeness ) {
      if ( (likeness & ~typeRemoved) != likeness) {
        return true;
      }
    }
    return false;
  }
  inline bool State::shouldFireAdditionDlgt(const compMask& likeness, const compMask& current,
                                            const compMask& typeAdded)
  {
    if ( (likeness & current) != likeness) {
      if ( (likeness & (current | typeAdded)) == likeness) {
        return true;
      }
    }
    return false;
  }

  /*
   * Component collection manipulation method definitions
   */
  CompOpReturn State::getExistence(const entityId& id, Existence** out) { return getComp(comps_Existence, id, out); }

  // COMPONENT COLLECTION MANIPULATION METHOD DEFINITIONS APPEAR HERE

}
