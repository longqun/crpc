// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: rpcmeta.proto

#include "rpcmeta.pb.h"

#include <algorithm>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/stubs/port.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// This is a temporary google only hack
#ifdef GOOGLE_PROTOBUF_ENFORCE_UNIQUENESS
#include "third_party/protobuf/version.h"
#endif
// @@protoc_insertion_point(includes)

namespace crpc {
class RpcMetaDefaultTypeInternal {
 public:
  ::google::protobuf::internal::ExplicitlyConstructed<RpcMeta>
      _instance;
} _RpcMeta_default_instance_;
}  // namespace crpc
namespace protobuf_rpcmeta_2eproto {
static void InitDefaultsRpcMeta() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  {
    void* ptr = &::crpc::_RpcMeta_default_instance_;
    new (ptr) ::crpc::RpcMeta();
    ::google::protobuf::internal::OnShutdownDestroyMessage(ptr);
  }
  ::crpc::RpcMeta::InitAsDefaultInstance();
}

::google::protobuf::internal::SCCInfo<0> scc_info_RpcMeta =
    {{ATOMIC_VAR_INIT(::google::protobuf::internal::SCCInfoBase::kUninitialized), 0, InitDefaultsRpcMeta}, {}};

void InitDefaults() {
  ::google::protobuf::internal::InitSCC(&scc_info_RpcMeta.base);
}

::google::protobuf::Metadata file_level_metadata[1];

const ::google::protobuf::uint32 TableStruct::offsets[] GOOGLE_PROTOBUF_ATTRIBUTE_SECTION_VARIABLE(protodesc_cold) = {
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(::crpc::RpcMeta, _has_bits_),
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(::crpc::RpcMeta, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(::crpc::RpcMeta, service_name_),
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(::crpc::RpcMeta, method_name_),
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(::crpc::RpcMeta, data_size_),
  0,
  1,
  2,
};
static const ::google::protobuf::internal::MigrationSchema schemas[] GOOGLE_PROTOBUF_ATTRIBUTE_SECTION_VARIABLE(protodesc_cold) = {
  { 0, 8, sizeof(::crpc::RpcMeta)},
};

static ::google::protobuf::Message const * const file_default_instances[] = {
  reinterpret_cast<const ::google::protobuf::Message*>(&::crpc::_RpcMeta_default_instance_),
};

void protobuf_AssignDescriptors() {
  AddDescriptors();
  AssignDescriptors(
      "rpcmeta.proto", schemas, file_default_instances, TableStruct::offsets,
      file_level_metadata, NULL, NULL);
}

void protobuf_AssignDescriptorsOnce() {
  static ::google::protobuf::internal::once_flag once;
  ::google::protobuf::internal::call_once(once, protobuf_AssignDescriptors);
}

void protobuf_RegisterTypes(const ::std::string&) GOOGLE_PROTOBUF_ATTRIBUTE_COLD;
void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::internal::RegisterAllTypes(file_level_metadata, 1);
}

void AddDescriptorsImpl() {
  InitDefaults();
  static const char descriptor[] GOOGLE_PROTOBUF_ATTRIBUTE_SECTION_VARIABLE(protodesc_cold) = {
      "\n\rrpcmeta.proto\022\004crpc\"G\n\007RpcMeta\022\024\n\014serv"
      "ice_name\030\001 \001(\t\022\023\n\013method_name\030\002 \001(\t\022\021\n\td"
      "ata_size\030\003 \001(\005"
  };
  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
      descriptor, 94);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "rpcmeta.proto", &protobuf_RegisterTypes);
}

void AddDescriptors() {
  static ::google::protobuf::internal::once_flag once;
  ::google::protobuf::internal::call_once(once, AddDescriptorsImpl);
}
// Force AddDescriptors() to be called at dynamic initialization time.
struct StaticDescriptorInitializer {
  StaticDescriptorInitializer() {
    AddDescriptors();
  }
} static_descriptor_initializer;
}  // namespace protobuf_rpcmeta_2eproto
namespace crpc {

// ===================================================================

void RpcMeta::InitAsDefaultInstance() {
}
#if !defined(_MSC_VER) || _MSC_VER >= 1900
const int RpcMeta::kServiceNameFieldNumber;
const int RpcMeta::kMethodNameFieldNumber;
const int RpcMeta::kDataSizeFieldNumber;
#endif  // !defined(_MSC_VER) || _MSC_VER >= 1900

RpcMeta::RpcMeta()
  : ::google::protobuf::Message(), _internal_metadata_(NULL) {
  ::google::protobuf::internal::InitSCC(
      &protobuf_rpcmeta_2eproto::scc_info_RpcMeta.base);
  SharedCtor();
  // @@protoc_insertion_point(constructor:crpc.RpcMeta)
}
RpcMeta::RpcMeta(const RpcMeta& from)
  : ::google::protobuf::Message(),
      _internal_metadata_(NULL),
      _has_bits_(from._has_bits_) {
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  service_name_.UnsafeSetDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  if (from.has_service_name()) {
    service_name_.AssignWithDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), from.service_name_);
  }
  method_name_.UnsafeSetDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  if (from.has_method_name()) {
    method_name_.AssignWithDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), from.method_name_);
  }
  data_size_ = from.data_size_;
  // @@protoc_insertion_point(copy_constructor:crpc.RpcMeta)
}

void RpcMeta::SharedCtor() {
  service_name_.UnsafeSetDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  method_name_.UnsafeSetDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  data_size_ = 0;
}

RpcMeta::~RpcMeta() {
  // @@protoc_insertion_point(destructor:crpc.RpcMeta)
  SharedDtor();
}

void RpcMeta::SharedDtor() {
  service_name_.DestroyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  method_name_.DestroyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}

void RpcMeta::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}
const ::google::protobuf::Descriptor* RpcMeta::descriptor() {
  ::protobuf_rpcmeta_2eproto::protobuf_AssignDescriptorsOnce();
  return ::protobuf_rpcmeta_2eproto::file_level_metadata[kIndexInFileMessages].descriptor;
}

const RpcMeta& RpcMeta::default_instance() {
  ::google::protobuf::internal::InitSCC(&protobuf_rpcmeta_2eproto::scc_info_RpcMeta.base);
  return *internal_default_instance();
}


void RpcMeta::Clear() {
// @@protoc_insertion_point(message_clear_start:crpc.RpcMeta)
  ::google::protobuf::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 3u) {
    if (cached_has_bits & 0x00000001u) {
      service_name_.ClearNonDefaultToEmptyNoArena();
    }
    if (cached_has_bits & 0x00000002u) {
      method_name_.ClearNonDefaultToEmptyNoArena();
    }
  }
  data_size_ = 0;
  _has_bits_.Clear();
  _internal_metadata_.Clear();
}

bool RpcMeta::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!GOOGLE_PREDICT_TRUE(EXPRESSION)) goto failure
  ::google::protobuf::uint32 tag;
  // @@protoc_insertion_point(parse_start:crpc.RpcMeta)
  for (;;) {
    ::std::pair<::google::protobuf::uint32, bool> p = input->ReadTagWithCutoffNoLastTag(127u);
    tag = p.first;
    if (!p.second) goto handle_unusual;
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // optional string service_name = 1;
      case 1: {
        if (static_cast< ::google::protobuf::uint8>(tag) ==
            static_cast< ::google::protobuf::uint8>(10u /* 10 & 0xFF */)) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_service_name()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8StringNamedField(
            this->service_name().data(), static_cast<int>(this->service_name().length()),
            ::google::protobuf::internal::WireFormat::PARSE,
            "crpc.RpcMeta.service_name");
        } else {
          goto handle_unusual;
        }
        break;
      }

      // optional string method_name = 2;
      case 2: {
        if (static_cast< ::google::protobuf::uint8>(tag) ==
            static_cast< ::google::protobuf::uint8>(18u /* 18 & 0xFF */)) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_method_name()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8StringNamedField(
            this->method_name().data(), static_cast<int>(this->method_name().length()),
            ::google::protobuf::internal::WireFormat::PARSE,
            "crpc.RpcMeta.method_name");
        } else {
          goto handle_unusual;
        }
        break;
      }

      // optional int32 data_size = 3;
      case 3: {
        if (static_cast< ::google::protobuf::uint8>(tag) ==
            static_cast< ::google::protobuf::uint8>(24u /* 24 & 0xFF */)) {
          set_has_data_size();
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &data_size_)));
        } else {
          goto handle_unusual;
        }
        break;
      }

      default: {
      handle_unusual:
        if (tag == 0) {
          goto success;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, _internal_metadata_.mutable_unknown_fields()));
        break;
      }
    }
  }
success:
  // @@protoc_insertion_point(parse_success:crpc.RpcMeta)
  return true;
failure:
  // @@protoc_insertion_point(parse_failure:crpc.RpcMeta)
  return false;
#undef DO_
}

void RpcMeta::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // @@protoc_insertion_point(serialize_start:crpc.RpcMeta)
  ::google::protobuf::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  // optional string service_name = 1;
  if (cached_has_bits & 0x00000001u) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8StringNamedField(
      this->service_name().data(), static_cast<int>(this->service_name().length()),
      ::google::protobuf::internal::WireFormat::SERIALIZE,
      "crpc.RpcMeta.service_name");
    ::google::protobuf::internal::WireFormatLite::WriteStringMaybeAliased(
      1, this->service_name(), output);
  }

  // optional string method_name = 2;
  if (cached_has_bits & 0x00000002u) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8StringNamedField(
      this->method_name().data(), static_cast<int>(this->method_name().length()),
      ::google::protobuf::internal::WireFormat::SERIALIZE,
      "crpc.RpcMeta.method_name");
    ::google::protobuf::internal::WireFormatLite::WriteStringMaybeAliased(
      2, this->method_name(), output);
  }

  // optional int32 data_size = 3;
  if (cached_has_bits & 0x00000004u) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(3, this->data_size(), output);
  }

  if (_internal_metadata_.have_unknown_fields()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        _internal_metadata_.unknown_fields(), output);
  }
  // @@protoc_insertion_point(serialize_end:crpc.RpcMeta)
}

::google::protobuf::uint8* RpcMeta::InternalSerializeWithCachedSizesToArray(
    bool deterministic, ::google::protobuf::uint8* target) const {
  (void)deterministic; // Unused
  // @@protoc_insertion_point(serialize_to_array_start:crpc.RpcMeta)
  ::google::protobuf::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  // optional string service_name = 1;
  if (cached_has_bits & 0x00000001u) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8StringNamedField(
      this->service_name().data(), static_cast<int>(this->service_name().length()),
      ::google::protobuf::internal::WireFormat::SERIALIZE,
      "crpc.RpcMeta.service_name");
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        1, this->service_name(), target);
  }

  // optional string method_name = 2;
  if (cached_has_bits & 0x00000002u) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8StringNamedField(
      this->method_name().data(), static_cast<int>(this->method_name().length()),
      ::google::protobuf::internal::WireFormat::SERIALIZE,
      "crpc.RpcMeta.method_name");
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        2, this->method_name(), target);
  }

  // optional int32 data_size = 3;
  if (cached_has_bits & 0x00000004u) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(3, this->data_size(), target);
  }

  if (_internal_metadata_.have_unknown_fields()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields(), target);
  }
  // @@protoc_insertion_point(serialize_to_array_end:crpc.RpcMeta)
  return target;
}

size_t RpcMeta::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:crpc.RpcMeta)
  size_t total_size = 0;

  if (_internal_metadata_.have_unknown_fields()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        _internal_metadata_.unknown_fields());
  }
  if (_has_bits_[0 / 32] & 7u) {
    // optional string service_name = 1;
    if (has_service_name()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->service_name());
    }

    // optional string method_name = 2;
    if (has_method_name()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->method_name());
    }

    // optional int32 data_size = 3;
    if (has_data_size()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(
          this->data_size());
    }

  }
  int cached_size = ::google::protobuf::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void RpcMeta::MergeFrom(const ::google::protobuf::Message& from) {
// @@protoc_insertion_point(generalized_merge_from_start:crpc.RpcMeta)
  GOOGLE_DCHECK_NE(&from, this);
  const RpcMeta* source =
      ::google::protobuf::internal::DynamicCastToGenerated<const RpcMeta>(
          &from);
  if (source == NULL) {
  // @@protoc_insertion_point(generalized_merge_from_cast_fail:crpc.RpcMeta)
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
  // @@protoc_insertion_point(generalized_merge_from_cast_success:crpc.RpcMeta)
    MergeFrom(*source);
  }
}

void RpcMeta::MergeFrom(const RpcMeta& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:crpc.RpcMeta)
  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  ::google::protobuf::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = from._has_bits_[0];
  if (cached_has_bits & 7u) {
    if (cached_has_bits & 0x00000001u) {
      set_has_service_name();
      service_name_.AssignWithDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), from.service_name_);
    }
    if (cached_has_bits & 0x00000002u) {
      set_has_method_name();
      method_name_.AssignWithDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), from.method_name_);
    }
    if (cached_has_bits & 0x00000004u) {
      data_size_ = from.data_size_;
    }
    _has_bits_[0] |= cached_has_bits;
  }
}

void RpcMeta::CopyFrom(const ::google::protobuf::Message& from) {
// @@protoc_insertion_point(generalized_copy_from_start:crpc.RpcMeta)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void RpcMeta::CopyFrom(const RpcMeta& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:crpc.RpcMeta)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool RpcMeta::IsInitialized() const {
  return true;
}

void RpcMeta::Swap(RpcMeta* other) {
  if (other == this) return;
  InternalSwap(other);
}
void RpcMeta::InternalSwap(RpcMeta* other) {
  using std::swap;
  service_name_.Swap(&other->service_name_, &::google::protobuf::internal::GetEmptyStringAlreadyInited(),
    GetArenaNoVirtual());
  method_name_.Swap(&other->method_name_, &::google::protobuf::internal::GetEmptyStringAlreadyInited(),
    GetArenaNoVirtual());
  swap(data_size_, other->data_size_);
  swap(_has_bits_[0], other->_has_bits_[0]);
  _internal_metadata_.Swap(&other->_internal_metadata_);
}

::google::protobuf::Metadata RpcMeta::GetMetadata() const {
  protobuf_rpcmeta_2eproto::protobuf_AssignDescriptorsOnce();
  return ::protobuf_rpcmeta_2eproto::file_level_metadata[kIndexInFileMessages];
}


// @@protoc_insertion_point(namespace_scope)
}  // namespace crpc
namespace google {
namespace protobuf {
template<> GOOGLE_PROTOBUF_ATTRIBUTE_NOINLINE ::crpc::RpcMeta* Arena::CreateMaybeMessage< ::crpc::RpcMeta >(Arena* arena) {
  return Arena::CreateInternal< ::crpc::RpcMeta >(arena);
}
}  // namespace protobuf
}  // namespace google

// @@protoc_insertion_point(global_scope)
