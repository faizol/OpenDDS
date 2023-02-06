/*
 * Distributed under the OpenDDS License.
 * See: http://www.opendds.org/license.html
 */

#ifndef OPENDDS_DCPS_GUIDUTILS_H
#define OPENDDS_DCPS_GUIDUTILS_H

#include "dcps_export.h"
#include "PoolAllocator.h"
#include "Serializer.h"
#include "Hash.h"

#include <dds/DdsDcpsGuidC.h>
#include <dds/DdsDcpsInfoUtilsC.h>

#ifndef OPENDDS_SAFETY_PROFILE
#  include <iosfwd>
#endif
#include <cstring>

OPENDDS_BEGIN_VERSIONED_NAMESPACE_DECL

namespace OpenDDS {
namespace DCPS {

/// Vendor Id value specified for OCI is used for OpenDDS.
const GuidVendorId_t VENDORID_OCI = {0x01, 0x03};

/// Nil value for the GUID prefix (participant identifier).
const GuidPrefix_t GUIDPREFIX_UNKNOWN = {0};

///@{
/// Entity Id values specified in Version 2.1 of RTPS specification.
const EntityId_t ENTITYID_UNKNOWN = {{0x00, 0x00, 0x00}, 0x00};
const EntityId_t ENTITYID_PARTICIPANT = {{0x00, 0x00, 0x01}, 0xc1};
const EntityId_t ENTITYID_SEDP_BUILTIN_TOPIC_WRITER = {{0x00, 0x00, 0x02}, 0xc2};
const EntityId_t ENTITYID_SEDP_BUILTIN_TOPIC_READER = {{0x00, 0x00, 0x02}, 0xc7};
const EntityId_t ENTITYID_SEDP_BUILTIN_PUBLICATIONS_WRITER = {{0x00, 0x00, 0x03}, 0xc2};
const EntityId_t ENTITYID_SEDP_BUILTIN_PUBLICATIONS_READER = {{0x00, 0x00, 0x03}, 0xc7};
const EntityId_t ENTITYID_SEDP_BUILTIN_SUBSCRIPTIONS_WRITER = {{0x00, 0x00, 0x04}, 0xc2};
const EntityId_t ENTITYID_SEDP_BUILTIN_SUBSCRIPTIONS_READER = {{0x00, 0x00, 0x04}, 0xc7};
const EntityId_t ENTITYID_SPDP_BUILTIN_PARTICIPANT_WRITER = {{0x00, 0x01, 0x00}, 0xc2};
const EntityId_t ENTITYID_SPDP_BUILTIN_PARTICIPANT_READER = {{0x00, 0x01, 0x00}, 0xc7};
const EntityId_t ENTITYID_P2P_BUILTIN_PARTICIPANT_MESSAGE_WRITER = {{0x00, 0x02, 0x00}, 0xc2};
const EntityId_t ENTITYID_P2P_BUILTIN_PARTICIPANT_MESSAGE_READER = {{0x00, 0x02, 0x00}, 0xc7};
///@}

///@{
/// XTypes Type Lookup Service
const EntityId_t ENTITYID_TL_SVC_REQ_WRITER = {{0x00, 0x03, 0x00}, 0xc3};
const EntityId_t ENTITYID_TL_SVC_REQ_READER = {{0x00, 0x03, 0x00}, 0xc4};
const EntityId_t ENTITYID_TL_SVC_REPLY_WRITER = {{0x00, 0x03, 0x01}, 0xc3};
const EntityId_t ENTITYID_TL_SVC_REPLY_READER = {{0x00, 0x03, 0x01}, 0xc4};
///@}

/// Nil value for GUID.
const GUID_t GUID_UNKNOWN = {{0}, {{0}, 0}};

/**
 * Identifies the kinds of entities used in a GUID.
 *
 * See dds/DdsDcpsGuid.idl for the values these map to.
 */
enum EntityKind {
  KIND_UNKNOWN, ///< ENTITYKIND_USER_UNKNOWN and ENTITYKIND_BUILTIN_UNKNOWN
  KIND_PARTICIPANT, ///< ENTITYKIND_BUILTIN_PARTICIPANT
  KIND_USER_WRITER, ///< ENTITYKIND_USER_WRITER_WITH_KEY and ENTITYKIND_USER_WRITER_NO_KEY
  KIND_USER_READER, ///< ENTITYKIND_USER_READER_WITH_KEY and ENTITYKIND_USER_READER_NO_KEY
  KIND_USER_TOPIC, ///< ENTITYKIND_OPENDDS_TOPIC
  KIND_BUILTIN_WRITER, ///< ENTITYKIND_BUILTIN_WRITER_WITH_KEY and ENTITYKIND_USER_WRITER_NO_KEY
  KIND_BUILTIN_READER, ///< ENTITYKIND_BUILTIN_READER_WITH_KEY and ENTITYKIND_USER_READER_NO_KEY
  KIND_BUILTIN_TOPIC, ///< ENTITYKIND_BUILTIN_TOPIC
  KIND_PUBLISHER, ///< Publisher (OpenDDS-specific)
  KIND_SUBSCRIBER, ///< Subscriber (OpenDDS-specific)
  KIND_USER ///< For creating custom GUIDs for things that are not DDS entities (OpenDDS-specific)
};

template <typename T>
inline int mem_cmp(const T& a, const T& b)
{
  return std::memcmp(&a, &b, sizeof(T));
}

template <typename T>
inline bool mem_less(const T& a, const T& b)
{
  return mem_cmp(a, b) < 0;
}

template <typename T>
inline bool mem_eq(const T& a, const T& b)
{
  return mem_cmp(a, b) == 0;
}

inline bool operator<(const GUID_t& lhs, const GUID_t& rhs)
{
  return mem_less(lhs, rhs);
}

struct OpenDDS_Dcps_Export GUID_tKeyLessThan {
  static bool entity_less(const EntityId_t& v1, const EntityId_t& v2)
  {
    return mem_less(v1, v2);
  }

  bool operator()(const GUID_t& v1, const GUID_t& v2) const
  {
    return v1 < v2;
  }
};

struct OpenDDS_Dcps_Export EntityId_tKeyLessThan {
  bool operator()(const EntityId_t& v1, const EntityId_t& v2) const
  {
    return mem_less(v1, v2);
  }
};

struct OpenDDS_Dcps_Export BuiltinTopicKey_tKeyLessThan {
  bool operator()(const DDS::BuiltinTopicKey_t& v1,
                  const DDS::BuiltinTopicKey_t& v2) const
  {
    return mem_less(v1, v2);
  }
};

typedef OPENDDS_SET_CMP(GUID_t, GUID_tKeyLessThan) GuidSet;
typedef GuidSet RepoIdSet;

const size_t guid_cdr_size = 16;

#ifndef OPENDDS_SAFETY_PROFILE
inline bool
operator==(const GUID_t& lhs, const GUID_t& rhs)
{
  return mem_eq(lhs, rhs);
}

inline bool
operator!=(const GUID_t& lhs, const GUID_t& rhs)
{
  return !(lhs == rhs);
}
#endif

OpenDDS_Dcps_Export inline
bool equal_guid_prefixes(const GuidPrefix_t& lhs, const GuidPrefix_t& rhs)
{
  return mem_eq(lhs, rhs);
}

OpenDDS_Dcps_Export inline
bool equal_guid_prefixes(const GUID_t& lhs, const GUID_t& rhs)
{
  return equal_guid_prefixes(lhs.guidPrefix, rhs.guidPrefix);
}

#ifndef OPENDDS_SAFETY_PROFILE
inline bool
operator==(const EntityId_t& lhs, const EntityId_t& rhs)
{
  return mem_eq(lhs, rhs);
}

inline bool
operator!=(const EntityId_t& lhs, const EntityId_t& rhs)
{
  return !(lhs == rhs);
}
#endif

inline void assign(EntityId_t& dest, const EntityId_t& src)
{
  std::memcpy(&dest, &src, sizeof(EntityId_t));
}

inline void assign(EntityKey_t& dest, unsigned src)
{
  dest[0] = static_cast<CORBA::Octet>(src);
  dest[1] = static_cast<CORBA::Octet>(src >> 8);
  dest[2] = static_cast<CORBA::Octet>(src >> 16);
}

OpenDDS_Dcps_Export OPENDDS_STRING
to_string(const GUID_t& guid);

OpenDDS_Dcps_Export OPENDDS_STRING
to_string(const EntityId_t& entityId);

#ifndef OPENDDS_SAFETY_PROFILE
// Serialize to ASCII Hex string: "xxxx.xxxx.xxxx.xxxx"
OpenDDS_Dcps_Export std::ostream&
operator<<(std::ostream& os, const GUID_t& rhs);

// Deserialize from ASCII Hex string: "xxxx.xxxx.xxxx.xxxx"
OpenDDS_Dcps_Export std::istream&
operator>>(std::istream& is, GUID_t& rhs);
#endif

inline void assign(GuidPrefix_t& dest, const GuidPrefix_t& src)
{
  std::memcpy(&dest[0], &src[0], sizeof(GuidPrefix_t));
}

inline void assign(GUID_t& dest, const GUID_t& src)
{
  std::memcpy(&dest, &src, sizeof(GUID_t));
}

inline void assign(DDS::OctetArray16& dest, const GUID_t& src)
{
  std::memcpy(&dest[0], &src, sizeof(src));
}

inline GUID_t make_id(const GuidPrefix_t& prefix, const EntityId_t& entity)
{
  GUID_t id;
  assign(id.guidPrefix, prefix);
  id.entityId = entity;
  return id;
}

inline GUID_t make_id(const GUID_t& participant_id, const EntityId_t& entity)
{
  GUID_t id = participant_id;
  id.entityId = entity;
  return id;
}

OpenDDS_Dcps_Export inline
GUID_t make_part_guid(const GuidPrefix_t& prefix)
{
  return make_id(prefix, ENTITYID_PARTICIPANT);
}

OpenDDS_Dcps_Export inline
GUID_t make_part_guid(const GUID_t& guid)
{
  return make_part_guid(guid.guidPrefix);
}

OpenDDS_Dcps_Export inline
GUID_t make_unknown_guid(const GuidPrefix_t& prefix)
{
  return make_id(prefix, ENTITYID_UNKNOWN);
}

OpenDDS_Dcps_Export inline
GUID_t make_unknown_guid(const GUID_t& guid)
{
  return make_unknown_guid(guid.guidPrefix);
}

OpenDDS_Dcps_Export
void intersect(const GuidSet& a, const GuidSet& b, GuidSet& result);

OpenDDS_Dcps_Export inline
DDS::BuiltinTopicKey_t guid_to_bit_key(const GUID_t& guid)
{
  DDS::BuiltinTopicKey_t key;
  std::memcpy(key.value, &guid, sizeof(key.value));
  return key;
}

OpenDDS_Dcps_Export inline
GUID_t bit_key_to_guid(const DDS::BuiltinTopicKey_t& key)
{
  GUID_t id;
  std::memcpy(&id, key.value, sizeof(id));
  return id;
}

struct OpenDDS_Dcps_Export GuidPair {
  GUID_t local;
  GUID_t remote;

  GuidPair(const GUID_t& local, const GUID_t& remote)
  : local(local)
  , remote(remote)
  {
  }

  int cmp(const GuidPair& other) const
  {
    const int local_cmp = mem_cmp(local, other.local);
    return local_cmp == 0 ? mem_cmp(remote, other.remote) : local_cmp;
  }

  bool operator<(const GuidPair& other) const
  {
    return cmp(other) < 0;
  }
};

} // namespace DCPS
} // namespace OpenDDS

OPENDDS_END_VERSIONED_NAMESPACE_DECL

#if defined ACE_HAS_CPP11
OPENDDS_OOAT_STD_HASH(OpenDDS::DCPS::GUID_t, OpenDDS_Dcps_Export);
#endif

#endif /* OPENDDS_DDS_DCPS_GUIDUTILS_H */
