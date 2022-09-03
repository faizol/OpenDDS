/*
 * Distributed under the OpenDDS License.
 * See: http://www.opendds.org/license.html
 */

#ifndef OPENDDS_DCPS_ABSTRACT_SAMPLE_H
#define OPENDDS_DCPS_ABSTRACT_SAMPLE_H

#include "Serializer.h"
#include "TypeSupportImpl.h"
#include "RcHandle_T.h"

OPENDDS_BEGIN_VERSIONED_NAMESPACE_DECL

namespace OpenDDS {
namespace DCPS {

class AbstractSample;
typedef RcHandle<AbstractSample> AbstractSample_rch;

/**
 * Represents a sample that can either be an instance of a C++ generated Type
 * from opendds_idl or a DynamicData. This is meant to be used by
 * DataReaderImpl and DataWriterImpl.
 */
class OpenDDS_Dcps_Export AbstractSample : public virtual RcObject {
public:
  AbstractSample(bool owns_data, bool read_only, bool key_only)
  : owns_data_(owns_data)
  , read_only_(read_only)
  , key_only_(key_only)
  {
  }

  virtual ~AbstractSample() {}

  bool owns_data() const
  {
    return owns_data_;
  }

  bool read_only() const
  {
    return owns_data_;
  }

  bool key_only() const
  {
    return key_only_;
  }

  virtual bool serialize(Serializer& ser) const = 0;
  virtual bool deserialize(Serializer& ser) const = 0;
  virtual size_t serialized_size(const Encoding& enc) const = 0;
  virtual bool compare(const AbstractSample& other) const = 0;
  virtual bool to_message_block(ACE_Message_Block& mb) = 0;
  virtual bool from_message_block(const ACE_Message_Block& mb) = 0;
  virtual AbstractSample_rch copy(bool read_only) const = 0;

protected:
  const bool owns_data_;
  const bool read_only_;
  const bool key_only_;
};

inline bool operator<(const AbstractSample& lhs, const AbstractSample& rhs)
{
  return lhs.compare(rhs);
}

struct OpenDDS_Dcps_Export AbstractSampleRchCmp {
  bool operator()(const AbstractSample_rch& lhs, const AbstractSample_rch& rhs) const
  {
    return *lhs < *rhs;
  }
};

template <typename NativeType>
class NativeSample : public AbstractSample {
public:
  typedef DDSTraits<NativeType> TraitsType;
  typedef MarshalTraits<NativeType> MarshalTraitsType;
  typedef RcHandle<NativeSample<NativeType> > Rch;
  typedef KeyOnly<const NativeType> KeyOnlyType;
  typedef KeyOnly<NativeType> MutableKeyOnlyType;

  NativeSample(const NativeType& data, bool key_only = false)
  : AbstractSample(/* owns_data = */ false, /* read_only = */ true, key_only)
  , data_(&data)
  {
  }

  NativeSample(const NativeType* data, bool key_only = false)
  : AbstractSample(/* owns_data = */ false, /* read_only = */ true, key_only)
  , data_(data)
  {
  }

  NativeSample(NativeType& data, bool key_only = false)
  : AbstractSample(/* owns_data = */ false, /* read_only = */ false, key_only)
  , data_(const_cast<const NativeType*>(&data))
  {
  }

  NativeSample(NativeType* data, bool key_only = false)
  : AbstractSample(/* owns_data = */ true, /* read_only = */ false, key_only)
  , data_(const_cast<const NativeType*>(data))
  {
  }

  virtual ~NativeSample()
  {
    if (owns_data_) {
      delete data_;
    }
  }

  const NativeType& data() const
  {
    return *data_;
  }

  NativeType& mutable_data() const
  {
    OPENDDS_ASSERT(!read_only_);
    return *const_cast<NativeType*>(data_);
  }

  KeyOnlyType key_only_data() const
  {
    return KeyOnlyType(*data_);
  }

  MutableKeyOnlyType mutable_key_only_data() const
  {
    return MutableKeyOnlyType(mutable_data());
  }

  bool serialize(Serializer& ser) const
  {
    if (key_only_) {
      return ser << key_only_data();
    } else {
      return ser << data();
    }
  }

  bool deserialize(Serializer& ser) const
  {
    if (key_only_) {
      return ser >> mutable_key_only_data();
    } else {
      return ser >> mutable_data();
    }
  }

  size_t serialized_size(const Encoding& enc) const
  {
    if (key_only_) {
      return OpenDDS::DCPS::serialized_size(enc, key_only_data());
    } else {
      return OpenDDS::DCPS::serialized_size(enc, data());
    }
  }

  bool compare(const AbstractSample& other) const
  {
    const NativeSample<NativeType>* const other_same_kind =
      dynamic_cast<const NativeSample<NativeType>*>(&other);
    OPENDDS_ASSERT(other_same_kind);
    return typename TraitsType::LessThanType()(*data_, *other_same_kind->data_);
  }

  bool to_message_block(ACE_Message_Block& mb)
  {
    return MarshalTraitsType::to_message_block(mb, data());
  }

  bool from_message_block(const ACE_Message_Block& mb)
  {
    return MarshalTraitsType::from_message_block(mutable_data(), mb);
  }

  AbstractSample_rch copy(bool read_only) const
  {
    NativeType* new_data = new NativeType;
    *new_data = *data_;
    return dynamic_rchandle_cast<AbstractSample>(make_rch<NativeSample<NativeType> >(
      read_only ? const_cast<const NativeType*>(new_data) : new_data, key_only_));
  }

private:
  const NativeType* data_;
};

} // namespace DCPS
} // namespace OpenDDS

OPENDDS_END_VERSIONED_NAMESPACE_DECL

#endif
