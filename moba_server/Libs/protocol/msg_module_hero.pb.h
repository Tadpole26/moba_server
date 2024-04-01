// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: msg_module_hero.proto

#ifndef GOOGLE_PROTOBUF_INCLUDED_msg_5fmodule_5fhero_2eproto
#define GOOGLE_PROTOBUF_INCLUDED_msg_5fmodule_5fhero_2eproto

#include <limits>
#include <string>

#include <google/protobuf/port_def.inc>
#if PROTOBUF_VERSION < 3019000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers. Please update
#error your headers.
#endif
#if 3019004 < PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers. Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/port_undef.inc>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_bases.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/metadata_lite.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/generated_enum_reflection.h>
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
#define PROTOBUF_INTERNAL_EXPORT_msg_5fmodule_5fhero_2eproto
PROTOBUF_NAMESPACE_OPEN
namespace internal {
class AnyMetadata;
}  // namespace internal
PROTOBUF_NAMESPACE_CLOSE

// Internal implementation detail -- do not use these members.
struct TableStruct_msg_5fmodule_5fhero_2eproto {
  static const uint32_t offsets[];
};
extern const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_msg_5fmodule_5fhero_2eproto;
namespace ProtoMsg {
class MsgModule_Hero;
struct MsgModule_HeroDefaultTypeInternal;
extern MsgModule_HeroDefaultTypeInternal _MsgModule_Hero_default_instance_;
class Msg_Hero_UpHeroLevel_Req;
struct Msg_Hero_UpHeroLevel_ReqDefaultTypeInternal;
extern Msg_Hero_UpHeroLevel_ReqDefaultTypeInternal _Msg_Hero_UpHeroLevel_Req_default_instance_;
class Msg_Hero_UpHeroLevel_Rsp;
struct Msg_Hero_UpHeroLevel_RspDefaultTypeInternal;
extern Msg_Hero_UpHeroLevel_RspDefaultTypeInternal _Msg_Hero_UpHeroLevel_Rsp_default_instance_;
class Msg_Hero_UpHeroStar_Req;
struct Msg_Hero_UpHeroStar_ReqDefaultTypeInternal;
extern Msg_Hero_UpHeroStar_ReqDefaultTypeInternal _Msg_Hero_UpHeroStar_Req_default_instance_;
class Msg_Hero_UpHeroStar_Rsp;
struct Msg_Hero_UpHeroStar_RspDefaultTypeInternal;
extern Msg_Hero_UpHeroStar_RspDefaultTypeInternal _Msg_Hero_UpHeroStar_Rsp_default_instance_;
}  // namespace ProtoMsg
PROTOBUF_NAMESPACE_OPEN
template<> ::ProtoMsg::MsgModule_Hero* Arena::CreateMaybeMessage<::ProtoMsg::MsgModule_Hero>(Arena*);
template<> ::ProtoMsg::Msg_Hero_UpHeroLevel_Req* Arena::CreateMaybeMessage<::ProtoMsg::Msg_Hero_UpHeroLevel_Req>(Arena*);
template<> ::ProtoMsg::Msg_Hero_UpHeroLevel_Rsp* Arena::CreateMaybeMessage<::ProtoMsg::Msg_Hero_UpHeroLevel_Rsp>(Arena*);
template<> ::ProtoMsg::Msg_Hero_UpHeroStar_Req* Arena::CreateMaybeMessage<::ProtoMsg::Msg_Hero_UpHeroStar_Req>(Arena*);
template<> ::ProtoMsg::Msg_Hero_UpHeroStar_Rsp* Arena::CreateMaybeMessage<::ProtoMsg::Msg_Hero_UpHeroStar_Rsp>(Arena*);
PROTOBUF_NAMESPACE_CLOSE
namespace ProtoMsg {

enum MsgModule_Hero_MsgCmd : int {
  MsgModule_Hero_MsgCmd_Msg_Hero_None = 0,
  MsgModule_Hero_MsgCmd_Msg_Hero_UpHeroLevel_Req = 1,
  MsgModule_Hero_MsgCmd_Msg_Hero_UpHeroLevel_Rsp = 2,
  MsgModule_Hero_MsgCmd_Msg_Hero_UpHeroStar_Req = 3,
  MsgModule_Hero_MsgCmd_Msg_Hero_UpHeroStar_Rsp = 4,
  MsgModule_Hero_MsgCmd_MsgModule_Hero_MsgCmd_INT_MIN_SENTINEL_DO_NOT_USE_ = std::numeric_limits<int32_t>::min(),
  MsgModule_Hero_MsgCmd_MsgModule_Hero_MsgCmd_INT_MAX_SENTINEL_DO_NOT_USE_ = std::numeric_limits<int32_t>::max()
};
bool MsgModule_Hero_MsgCmd_IsValid(int value);
constexpr MsgModule_Hero_MsgCmd MsgModule_Hero_MsgCmd_MsgCmd_MIN = MsgModule_Hero_MsgCmd_Msg_Hero_None;
constexpr MsgModule_Hero_MsgCmd MsgModule_Hero_MsgCmd_MsgCmd_MAX = MsgModule_Hero_MsgCmd_Msg_Hero_UpHeroStar_Rsp;
constexpr int MsgModule_Hero_MsgCmd_MsgCmd_ARRAYSIZE = MsgModule_Hero_MsgCmd_MsgCmd_MAX + 1;

const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* MsgModule_Hero_MsgCmd_descriptor();
template<typename T>
inline const std::string& MsgModule_Hero_MsgCmd_Name(T enum_t_value) {
  static_assert(::std::is_same<T, MsgModule_Hero_MsgCmd>::value ||
    ::std::is_integral<T>::value,
    "Incorrect type passed to function MsgModule_Hero_MsgCmd_Name.");
  return ::PROTOBUF_NAMESPACE_ID::internal::NameOfEnum(
    MsgModule_Hero_MsgCmd_descriptor(), enum_t_value);
}
inline bool MsgModule_Hero_MsgCmd_Parse(
    ::PROTOBUF_NAMESPACE_ID::ConstStringParam name, MsgModule_Hero_MsgCmd* value) {
  return ::PROTOBUF_NAMESPACE_ID::internal::ParseNamedEnum<MsgModule_Hero_MsgCmd>(
    MsgModule_Hero_MsgCmd_descriptor(), name, value);
}
// ===================================================================

class MsgModule_Hero final :
    public ::PROTOBUF_NAMESPACE_ID::internal::ZeroFieldsBase /* @@protoc_insertion_point(class_definition:ProtoMsg.MsgModule_Hero) */ {
 public:
  inline MsgModule_Hero() : MsgModule_Hero(nullptr) {}
  explicit PROTOBUF_CONSTEXPR MsgModule_Hero(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized);

  MsgModule_Hero(const MsgModule_Hero& from);
  MsgModule_Hero(MsgModule_Hero&& from) noexcept
    : MsgModule_Hero() {
    *this = ::std::move(from);
  }

  inline MsgModule_Hero& operator=(const MsgModule_Hero& from) {
    CopyFrom(from);
    return *this;
  }
  inline MsgModule_Hero& operator=(MsgModule_Hero&& from) noexcept {
    if (this == &from) return *this;
    if (GetOwningArena() == from.GetOwningArena()
  #ifdef PROTOBUF_FORCE_COPY_IN_MOVE
        && GetOwningArena() != nullptr
  #endif  // !PROTOBUF_FORCE_COPY_IN_MOVE
    ) {
      InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* descriptor() {
    return GetDescriptor();
  }
  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* GetDescriptor() {
    return default_instance().GetMetadata().descriptor;
  }
  static const ::PROTOBUF_NAMESPACE_ID::Reflection* GetReflection() {
    return default_instance().GetMetadata().reflection;
  }
  static const MsgModule_Hero& default_instance() {
    return *internal_default_instance();
  }
  static inline const MsgModule_Hero* internal_default_instance() {
    return reinterpret_cast<const MsgModule_Hero*>(
               &_MsgModule_Hero_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    0;

  friend void swap(MsgModule_Hero& a, MsgModule_Hero& b) {
    a.Swap(&b);
  }
  inline void Swap(MsgModule_Hero* other) {
    if (other == this) return;
  #ifdef PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() != nullptr &&
        GetOwningArena() == other->GetOwningArena()) {
   #else  // PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() == other->GetOwningArena()) {
  #endif  // !PROTOBUF_FORCE_COPY_IN_SWAP
      InternalSwap(other);
    } else {
      ::PROTOBUF_NAMESPACE_ID::internal::GenericSwap(this, other);
    }
  }
  void UnsafeArenaSwap(MsgModule_Hero* other) {
    if (other == this) return;
    GOOGLE_DCHECK(GetOwningArena() == other->GetOwningArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  MsgModule_Hero* New(::PROTOBUF_NAMESPACE_ID::Arena* arena = nullptr) const final {
    return CreateMaybeMessage<MsgModule_Hero>(arena);
  }
  using ::PROTOBUF_NAMESPACE_ID::internal::ZeroFieldsBase::CopyFrom;
  inline void CopyFrom(const MsgModule_Hero& from) {
    ::PROTOBUF_NAMESPACE_ID::internal::ZeroFieldsBase::CopyImpl(this, from);
  }
  using ::PROTOBUF_NAMESPACE_ID::internal::ZeroFieldsBase::MergeFrom;
  void MergeFrom(const MsgModule_Hero& from) {
    ::PROTOBUF_NAMESPACE_ID::internal::ZeroFieldsBase::MergeImpl(this, from);
  }
  public:

  private:
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "ProtoMsg.MsgModule_Hero";
  }
  protected:
  explicit MsgModule_Hero(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                       bool is_message_owned = false);
  public:

  static const ClassData _class_data_;
  const ::PROTOBUF_NAMESPACE_ID::Message::ClassData*GetClassData() const final;

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;

  // nested types ----------------------------------------------------

  typedef MsgModule_Hero_MsgCmd MsgCmd;
  static constexpr MsgCmd Msg_Hero_None =
    MsgModule_Hero_MsgCmd_Msg_Hero_None;
  static constexpr MsgCmd Msg_Hero_UpHeroLevel_Req =
    MsgModule_Hero_MsgCmd_Msg_Hero_UpHeroLevel_Req;
  static constexpr MsgCmd Msg_Hero_UpHeroLevel_Rsp =
    MsgModule_Hero_MsgCmd_Msg_Hero_UpHeroLevel_Rsp;
  static constexpr MsgCmd Msg_Hero_UpHeroStar_Req =
    MsgModule_Hero_MsgCmd_Msg_Hero_UpHeroStar_Req;
  static constexpr MsgCmd Msg_Hero_UpHeroStar_Rsp =
    MsgModule_Hero_MsgCmd_Msg_Hero_UpHeroStar_Rsp;
  static inline bool MsgCmd_IsValid(int value) {
    return MsgModule_Hero_MsgCmd_IsValid(value);
  }
  static constexpr MsgCmd MsgCmd_MIN =
    MsgModule_Hero_MsgCmd_MsgCmd_MIN;
  static constexpr MsgCmd MsgCmd_MAX =
    MsgModule_Hero_MsgCmd_MsgCmd_MAX;
  static constexpr int MsgCmd_ARRAYSIZE =
    MsgModule_Hero_MsgCmd_MsgCmd_ARRAYSIZE;
  static inline const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor*
  MsgCmd_descriptor() {
    return MsgModule_Hero_MsgCmd_descriptor();
  }
  template<typename T>
  static inline const std::string& MsgCmd_Name(T enum_t_value) {
    static_assert(::std::is_same<T, MsgCmd>::value ||
      ::std::is_integral<T>::value,
      "Incorrect type passed to function MsgCmd_Name.");
    return MsgModule_Hero_MsgCmd_Name(enum_t_value);
  }
  static inline bool MsgCmd_Parse(::PROTOBUF_NAMESPACE_ID::ConstStringParam name,
      MsgCmd* value) {
    return MsgModule_Hero_MsgCmd_Parse(name, value);
  }

  // accessors -------------------------------------------------------

  // @@protoc_insertion_point(class_scope:ProtoMsg.MsgModule_Hero)
 private:
  class _Internal;

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  friend struct ::TableStruct_msg_5fmodule_5fhero_2eproto;
};
// -------------------------------------------------------------------

class Msg_Hero_UpHeroLevel_Req final :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:ProtoMsg.Msg_Hero_UpHeroLevel_Req) */ {
 public:
  inline Msg_Hero_UpHeroLevel_Req() : Msg_Hero_UpHeroLevel_Req(nullptr) {}
  ~Msg_Hero_UpHeroLevel_Req() override;
  explicit PROTOBUF_CONSTEXPR Msg_Hero_UpHeroLevel_Req(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized);

  Msg_Hero_UpHeroLevel_Req(const Msg_Hero_UpHeroLevel_Req& from);
  Msg_Hero_UpHeroLevel_Req(Msg_Hero_UpHeroLevel_Req&& from) noexcept
    : Msg_Hero_UpHeroLevel_Req() {
    *this = ::std::move(from);
  }

  inline Msg_Hero_UpHeroLevel_Req& operator=(const Msg_Hero_UpHeroLevel_Req& from) {
    CopyFrom(from);
    return *this;
  }
  inline Msg_Hero_UpHeroLevel_Req& operator=(Msg_Hero_UpHeroLevel_Req&& from) noexcept {
    if (this == &from) return *this;
    if (GetOwningArena() == from.GetOwningArena()
  #ifdef PROTOBUF_FORCE_COPY_IN_MOVE
        && GetOwningArena() != nullptr
  #endif  // !PROTOBUF_FORCE_COPY_IN_MOVE
    ) {
      InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* descriptor() {
    return GetDescriptor();
  }
  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* GetDescriptor() {
    return default_instance().GetMetadata().descriptor;
  }
  static const ::PROTOBUF_NAMESPACE_ID::Reflection* GetReflection() {
    return default_instance().GetMetadata().reflection;
  }
  static const Msg_Hero_UpHeroLevel_Req& default_instance() {
    return *internal_default_instance();
  }
  static inline const Msg_Hero_UpHeroLevel_Req* internal_default_instance() {
    return reinterpret_cast<const Msg_Hero_UpHeroLevel_Req*>(
               &_Msg_Hero_UpHeroLevel_Req_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    1;

  friend void swap(Msg_Hero_UpHeroLevel_Req& a, Msg_Hero_UpHeroLevel_Req& b) {
    a.Swap(&b);
  }
  inline void Swap(Msg_Hero_UpHeroLevel_Req* other) {
    if (other == this) return;
  #ifdef PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() != nullptr &&
        GetOwningArena() == other->GetOwningArena()) {
   #else  // PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() == other->GetOwningArena()) {
  #endif  // !PROTOBUF_FORCE_COPY_IN_SWAP
      InternalSwap(other);
    } else {
      ::PROTOBUF_NAMESPACE_ID::internal::GenericSwap(this, other);
    }
  }
  void UnsafeArenaSwap(Msg_Hero_UpHeroLevel_Req* other) {
    if (other == this) return;
    GOOGLE_DCHECK(GetOwningArena() == other->GetOwningArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  Msg_Hero_UpHeroLevel_Req* New(::PROTOBUF_NAMESPACE_ID::Arena* arena = nullptr) const final {
    return CreateMaybeMessage<Msg_Hero_UpHeroLevel_Req>(arena);
  }
  using ::PROTOBUF_NAMESPACE_ID::Message::CopyFrom;
  void CopyFrom(const Msg_Hero_UpHeroLevel_Req& from);
  using ::PROTOBUF_NAMESPACE_ID::Message::MergeFrom;
  void MergeFrom(const Msg_Hero_UpHeroLevel_Req& from);
  private:
  static void MergeImpl(::PROTOBUF_NAMESPACE_ID::Message* to, const ::PROTOBUF_NAMESPACE_ID::Message& from);
  public:
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  uint8_t* _InternalSerialize(
      uint8_t* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const final;
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(Msg_Hero_UpHeroLevel_Req* other);

  private:
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "ProtoMsg.Msg_Hero_UpHeroLevel_Req";
  }
  protected:
  explicit Msg_Hero_UpHeroLevel_Req(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                       bool is_message_owned = false);
  public:

  static const ClassData _class_data_;
  const ::PROTOBUF_NAMESPACE_ID::Message::ClassData*GetClassData() const final;

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kUiHeroIdFieldNumber = 1,
  };
  // uint32 uiHeroId = 1;
  void clear_uiheroid();
  uint32_t uiheroid() const;
  void set_uiheroid(uint32_t value);
  private:
  uint32_t _internal_uiheroid() const;
  void _internal_set_uiheroid(uint32_t value);
  public:

  // @@protoc_insertion_point(class_scope:ProtoMsg.Msg_Hero_UpHeroLevel_Req)
 private:
  class _Internal;

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  uint32_t uiheroid_;
  mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  friend struct ::TableStruct_msg_5fmodule_5fhero_2eproto;
};
// -------------------------------------------------------------------

class Msg_Hero_UpHeroLevel_Rsp final :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:ProtoMsg.Msg_Hero_UpHeroLevel_Rsp) */ {
 public:
  inline Msg_Hero_UpHeroLevel_Rsp() : Msg_Hero_UpHeroLevel_Rsp(nullptr) {}
  ~Msg_Hero_UpHeroLevel_Rsp() override;
  explicit PROTOBUF_CONSTEXPR Msg_Hero_UpHeroLevel_Rsp(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized);

  Msg_Hero_UpHeroLevel_Rsp(const Msg_Hero_UpHeroLevel_Rsp& from);
  Msg_Hero_UpHeroLevel_Rsp(Msg_Hero_UpHeroLevel_Rsp&& from) noexcept
    : Msg_Hero_UpHeroLevel_Rsp() {
    *this = ::std::move(from);
  }

  inline Msg_Hero_UpHeroLevel_Rsp& operator=(const Msg_Hero_UpHeroLevel_Rsp& from) {
    CopyFrom(from);
    return *this;
  }
  inline Msg_Hero_UpHeroLevel_Rsp& operator=(Msg_Hero_UpHeroLevel_Rsp&& from) noexcept {
    if (this == &from) return *this;
    if (GetOwningArena() == from.GetOwningArena()
  #ifdef PROTOBUF_FORCE_COPY_IN_MOVE
        && GetOwningArena() != nullptr
  #endif  // !PROTOBUF_FORCE_COPY_IN_MOVE
    ) {
      InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* descriptor() {
    return GetDescriptor();
  }
  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* GetDescriptor() {
    return default_instance().GetMetadata().descriptor;
  }
  static const ::PROTOBUF_NAMESPACE_ID::Reflection* GetReflection() {
    return default_instance().GetMetadata().reflection;
  }
  static const Msg_Hero_UpHeroLevel_Rsp& default_instance() {
    return *internal_default_instance();
  }
  static inline const Msg_Hero_UpHeroLevel_Rsp* internal_default_instance() {
    return reinterpret_cast<const Msg_Hero_UpHeroLevel_Rsp*>(
               &_Msg_Hero_UpHeroLevel_Rsp_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    2;

  friend void swap(Msg_Hero_UpHeroLevel_Rsp& a, Msg_Hero_UpHeroLevel_Rsp& b) {
    a.Swap(&b);
  }
  inline void Swap(Msg_Hero_UpHeroLevel_Rsp* other) {
    if (other == this) return;
  #ifdef PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() != nullptr &&
        GetOwningArena() == other->GetOwningArena()) {
   #else  // PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() == other->GetOwningArena()) {
  #endif  // !PROTOBUF_FORCE_COPY_IN_SWAP
      InternalSwap(other);
    } else {
      ::PROTOBUF_NAMESPACE_ID::internal::GenericSwap(this, other);
    }
  }
  void UnsafeArenaSwap(Msg_Hero_UpHeroLevel_Rsp* other) {
    if (other == this) return;
    GOOGLE_DCHECK(GetOwningArena() == other->GetOwningArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  Msg_Hero_UpHeroLevel_Rsp* New(::PROTOBUF_NAMESPACE_ID::Arena* arena = nullptr) const final {
    return CreateMaybeMessage<Msg_Hero_UpHeroLevel_Rsp>(arena);
  }
  using ::PROTOBUF_NAMESPACE_ID::Message::CopyFrom;
  void CopyFrom(const Msg_Hero_UpHeroLevel_Rsp& from);
  using ::PROTOBUF_NAMESPACE_ID::Message::MergeFrom;
  void MergeFrom(const Msg_Hero_UpHeroLevel_Rsp& from);
  private:
  static void MergeImpl(::PROTOBUF_NAMESPACE_ID::Message* to, const ::PROTOBUF_NAMESPACE_ID::Message& from);
  public:
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  uint8_t* _InternalSerialize(
      uint8_t* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const final;
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(Msg_Hero_UpHeroLevel_Rsp* other);

  private:
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "ProtoMsg.Msg_Hero_UpHeroLevel_Rsp";
  }
  protected:
  explicit Msg_Hero_UpHeroLevel_Rsp(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                       bool is_message_owned = false);
  public:

  static const ClassData _class_data_;
  const ::PROTOBUF_NAMESPACE_ID::Message::ClassData*GetClassData() const final;

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kUiCurLevelFieldNumber = 1,
  };
  // uint32 uiCurLevel = 1;
  void clear_uicurlevel();
  uint32_t uicurlevel() const;
  void set_uicurlevel(uint32_t value);
  private:
  uint32_t _internal_uicurlevel() const;
  void _internal_set_uicurlevel(uint32_t value);
  public:

  // @@protoc_insertion_point(class_scope:ProtoMsg.Msg_Hero_UpHeroLevel_Rsp)
 private:
  class _Internal;

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  uint32_t uicurlevel_;
  mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  friend struct ::TableStruct_msg_5fmodule_5fhero_2eproto;
};
// -------------------------------------------------------------------

class Msg_Hero_UpHeroStar_Req final :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:ProtoMsg.Msg_Hero_UpHeroStar_Req) */ {
 public:
  inline Msg_Hero_UpHeroStar_Req() : Msg_Hero_UpHeroStar_Req(nullptr) {}
  ~Msg_Hero_UpHeroStar_Req() override;
  explicit PROTOBUF_CONSTEXPR Msg_Hero_UpHeroStar_Req(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized);

  Msg_Hero_UpHeroStar_Req(const Msg_Hero_UpHeroStar_Req& from);
  Msg_Hero_UpHeroStar_Req(Msg_Hero_UpHeroStar_Req&& from) noexcept
    : Msg_Hero_UpHeroStar_Req() {
    *this = ::std::move(from);
  }

  inline Msg_Hero_UpHeroStar_Req& operator=(const Msg_Hero_UpHeroStar_Req& from) {
    CopyFrom(from);
    return *this;
  }
  inline Msg_Hero_UpHeroStar_Req& operator=(Msg_Hero_UpHeroStar_Req&& from) noexcept {
    if (this == &from) return *this;
    if (GetOwningArena() == from.GetOwningArena()
  #ifdef PROTOBUF_FORCE_COPY_IN_MOVE
        && GetOwningArena() != nullptr
  #endif  // !PROTOBUF_FORCE_COPY_IN_MOVE
    ) {
      InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* descriptor() {
    return GetDescriptor();
  }
  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* GetDescriptor() {
    return default_instance().GetMetadata().descriptor;
  }
  static const ::PROTOBUF_NAMESPACE_ID::Reflection* GetReflection() {
    return default_instance().GetMetadata().reflection;
  }
  static const Msg_Hero_UpHeroStar_Req& default_instance() {
    return *internal_default_instance();
  }
  static inline const Msg_Hero_UpHeroStar_Req* internal_default_instance() {
    return reinterpret_cast<const Msg_Hero_UpHeroStar_Req*>(
               &_Msg_Hero_UpHeroStar_Req_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    3;

  friend void swap(Msg_Hero_UpHeroStar_Req& a, Msg_Hero_UpHeroStar_Req& b) {
    a.Swap(&b);
  }
  inline void Swap(Msg_Hero_UpHeroStar_Req* other) {
    if (other == this) return;
  #ifdef PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() != nullptr &&
        GetOwningArena() == other->GetOwningArena()) {
   #else  // PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() == other->GetOwningArena()) {
  #endif  // !PROTOBUF_FORCE_COPY_IN_SWAP
      InternalSwap(other);
    } else {
      ::PROTOBUF_NAMESPACE_ID::internal::GenericSwap(this, other);
    }
  }
  void UnsafeArenaSwap(Msg_Hero_UpHeroStar_Req* other) {
    if (other == this) return;
    GOOGLE_DCHECK(GetOwningArena() == other->GetOwningArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  Msg_Hero_UpHeroStar_Req* New(::PROTOBUF_NAMESPACE_ID::Arena* arena = nullptr) const final {
    return CreateMaybeMessage<Msg_Hero_UpHeroStar_Req>(arena);
  }
  using ::PROTOBUF_NAMESPACE_ID::Message::CopyFrom;
  void CopyFrom(const Msg_Hero_UpHeroStar_Req& from);
  using ::PROTOBUF_NAMESPACE_ID::Message::MergeFrom;
  void MergeFrom(const Msg_Hero_UpHeroStar_Req& from);
  private:
  static void MergeImpl(::PROTOBUF_NAMESPACE_ID::Message* to, const ::PROTOBUF_NAMESPACE_ID::Message& from);
  public:
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  uint8_t* _InternalSerialize(
      uint8_t* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const final;
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(Msg_Hero_UpHeroStar_Req* other);

  private:
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "ProtoMsg.Msg_Hero_UpHeroStar_Req";
  }
  protected:
  explicit Msg_Hero_UpHeroStar_Req(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                       bool is_message_owned = false);
  public:

  static const ClassData _class_data_;
  const ::PROTOBUF_NAMESPACE_ID::Message::ClassData*GetClassData() const final;

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kUiHeroIdFieldNumber = 1,
  };
  // uint32 uiHeroId = 1;
  void clear_uiheroid();
  uint32_t uiheroid() const;
  void set_uiheroid(uint32_t value);
  private:
  uint32_t _internal_uiheroid() const;
  void _internal_set_uiheroid(uint32_t value);
  public:

  // @@protoc_insertion_point(class_scope:ProtoMsg.Msg_Hero_UpHeroStar_Req)
 private:
  class _Internal;

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  uint32_t uiheroid_;
  mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  friend struct ::TableStruct_msg_5fmodule_5fhero_2eproto;
};
// -------------------------------------------------------------------

class Msg_Hero_UpHeroStar_Rsp final :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:ProtoMsg.Msg_Hero_UpHeroStar_Rsp) */ {
 public:
  inline Msg_Hero_UpHeroStar_Rsp() : Msg_Hero_UpHeroStar_Rsp(nullptr) {}
  ~Msg_Hero_UpHeroStar_Rsp() override;
  explicit PROTOBUF_CONSTEXPR Msg_Hero_UpHeroStar_Rsp(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized);

  Msg_Hero_UpHeroStar_Rsp(const Msg_Hero_UpHeroStar_Rsp& from);
  Msg_Hero_UpHeroStar_Rsp(Msg_Hero_UpHeroStar_Rsp&& from) noexcept
    : Msg_Hero_UpHeroStar_Rsp() {
    *this = ::std::move(from);
  }

  inline Msg_Hero_UpHeroStar_Rsp& operator=(const Msg_Hero_UpHeroStar_Rsp& from) {
    CopyFrom(from);
    return *this;
  }
  inline Msg_Hero_UpHeroStar_Rsp& operator=(Msg_Hero_UpHeroStar_Rsp&& from) noexcept {
    if (this == &from) return *this;
    if (GetOwningArena() == from.GetOwningArena()
  #ifdef PROTOBUF_FORCE_COPY_IN_MOVE
        && GetOwningArena() != nullptr
  #endif  // !PROTOBUF_FORCE_COPY_IN_MOVE
    ) {
      InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* descriptor() {
    return GetDescriptor();
  }
  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* GetDescriptor() {
    return default_instance().GetMetadata().descriptor;
  }
  static const ::PROTOBUF_NAMESPACE_ID::Reflection* GetReflection() {
    return default_instance().GetMetadata().reflection;
  }
  static const Msg_Hero_UpHeroStar_Rsp& default_instance() {
    return *internal_default_instance();
  }
  static inline const Msg_Hero_UpHeroStar_Rsp* internal_default_instance() {
    return reinterpret_cast<const Msg_Hero_UpHeroStar_Rsp*>(
               &_Msg_Hero_UpHeroStar_Rsp_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    4;

  friend void swap(Msg_Hero_UpHeroStar_Rsp& a, Msg_Hero_UpHeroStar_Rsp& b) {
    a.Swap(&b);
  }
  inline void Swap(Msg_Hero_UpHeroStar_Rsp* other) {
    if (other == this) return;
  #ifdef PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() != nullptr &&
        GetOwningArena() == other->GetOwningArena()) {
   #else  // PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() == other->GetOwningArena()) {
  #endif  // !PROTOBUF_FORCE_COPY_IN_SWAP
      InternalSwap(other);
    } else {
      ::PROTOBUF_NAMESPACE_ID::internal::GenericSwap(this, other);
    }
  }
  void UnsafeArenaSwap(Msg_Hero_UpHeroStar_Rsp* other) {
    if (other == this) return;
    GOOGLE_DCHECK(GetOwningArena() == other->GetOwningArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  Msg_Hero_UpHeroStar_Rsp* New(::PROTOBUF_NAMESPACE_ID::Arena* arena = nullptr) const final {
    return CreateMaybeMessage<Msg_Hero_UpHeroStar_Rsp>(arena);
  }
  using ::PROTOBUF_NAMESPACE_ID::Message::CopyFrom;
  void CopyFrom(const Msg_Hero_UpHeroStar_Rsp& from);
  using ::PROTOBUF_NAMESPACE_ID::Message::MergeFrom;
  void MergeFrom(const Msg_Hero_UpHeroStar_Rsp& from);
  private:
  static void MergeImpl(::PROTOBUF_NAMESPACE_ID::Message* to, const ::PROTOBUF_NAMESPACE_ID::Message& from);
  public:
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  uint8_t* _InternalSerialize(
      uint8_t* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const final;
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(Msg_Hero_UpHeroStar_Rsp* other);

  private:
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "ProtoMsg.Msg_Hero_UpHeroStar_Rsp";
  }
  protected:
  explicit Msg_Hero_UpHeroStar_Rsp(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                       bool is_message_owned = false);
  public:

  static const ClassData _class_data_;
  const ::PROTOBUF_NAMESPACE_ID::Message::ClassData*GetClassData() const final;

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kUiCurStarFieldNumber = 1,
  };
  // uint32 uiCurStar = 1;
  void clear_uicurstar();
  uint32_t uicurstar() const;
  void set_uicurstar(uint32_t value);
  private:
  uint32_t _internal_uicurstar() const;
  void _internal_set_uicurstar(uint32_t value);
  public:

  // @@protoc_insertion_point(class_scope:ProtoMsg.Msg_Hero_UpHeroStar_Rsp)
 private:
  class _Internal;

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  uint32_t uicurstar_;
  mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  friend struct ::TableStruct_msg_5fmodule_5fhero_2eproto;
};
// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// MsgModule_Hero

// -------------------------------------------------------------------

// Msg_Hero_UpHeroLevel_Req

// uint32 uiHeroId = 1;
inline void Msg_Hero_UpHeroLevel_Req::clear_uiheroid() {
  uiheroid_ = 0u;
}
inline uint32_t Msg_Hero_UpHeroLevel_Req::_internal_uiheroid() const {
  return uiheroid_;
}
inline uint32_t Msg_Hero_UpHeroLevel_Req::uiheroid() const {
  // @@protoc_insertion_point(field_get:ProtoMsg.Msg_Hero_UpHeroLevel_Req.uiHeroId)
  return _internal_uiheroid();
}
inline void Msg_Hero_UpHeroLevel_Req::_internal_set_uiheroid(uint32_t value) {
  
  uiheroid_ = value;
}
inline void Msg_Hero_UpHeroLevel_Req::set_uiheroid(uint32_t value) {
  _internal_set_uiheroid(value);
  // @@protoc_insertion_point(field_set:ProtoMsg.Msg_Hero_UpHeroLevel_Req.uiHeroId)
}

// -------------------------------------------------------------------

// Msg_Hero_UpHeroLevel_Rsp

// uint32 uiCurLevel = 1;
inline void Msg_Hero_UpHeroLevel_Rsp::clear_uicurlevel() {
  uicurlevel_ = 0u;
}
inline uint32_t Msg_Hero_UpHeroLevel_Rsp::_internal_uicurlevel() const {
  return uicurlevel_;
}
inline uint32_t Msg_Hero_UpHeroLevel_Rsp::uicurlevel() const {
  // @@protoc_insertion_point(field_get:ProtoMsg.Msg_Hero_UpHeroLevel_Rsp.uiCurLevel)
  return _internal_uicurlevel();
}
inline void Msg_Hero_UpHeroLevel_Rsp::_internal_set_uicurlevel(uint32_t value) {
  
  uicurlevel_ = value;
}
inline void Msg_Hero_UpHeroLevel_Rsp::set_uicurlevel(uint32_t value) {
  _internal_set_uicurlevel(value);
  // @@protoc_insertion_point(field_set:ProtoMsg.Msg_Hero_UpHeroLevel_Rsp.uiCurLevel)
}

// -------------------------------------------------------------------

// Msg_Hero_UpHeroStar_Req

// uint32 uiHeroId = 1;
inline void Msg_Hero_UpHeroStar_Req::clear_uiheroid() {
  uiheroid_ = 0u;
}
inline uint32_t Msg_Hero_UpHeroStar_Req::_internal_uiheroid() const {
  return uiheroid_;
}
inline uint32_t Msg_Hero_UpHeroStar_Req::uiheroid() const {
  // @@protoc_insertion_point(field_get:ProtoMsg.Msg_Hero_UpHeroStar_Req.uiHeroId)
  return _internal_uiheroid();
}
inline void Msg_Hero_UpHeroStar_Req::_internal_set_uiheroid(uint32_t value) {
  
  uiheroid_ = value;
}
inline void Msg_Hero_UpHeroStar_Req::set_uiheroid(uint32_t value) {
  _internal_set_uiheroid(value);
  // @@protoc_insertion_point(field_set:ProtoMsg.Msg_Hero_UpHeroStar_Req.uiHeroId)
}

// -------------------------------------------------------------------

// Msg_Hero_UpHeroStar_Rsp

// uint32 uiCurStar = 1;
inline void Msg_Hero_UpHeroStar_Rsp::clear_uicurstar() {
  uicurstar_ = 0u;
}
inline uint32_t Msg_Hero_UpHeroStar_Rsp::_internal_uicurstar() const {
  return uicurstar_;
}
inline uint32_t Msg_Hero_UpHeroStar_Rsp::uicurstar() const {
  // @@protoc_insertion_point(field_get:ProtoMsg.Msg_Hero_UpHeroStar_Rsp.uiCurStar)
  return _internal_uicurstar();
}
inline void Msg_Hero_UpHeroStar_Rsp::_internal_set_uicurstar(uint32_t value) {
  
  uicurstar_ = value;
}
inline void Msg_Hero_UpHeroStar_Rsp::set_uicurstar(uint32_t value) {
  _internal_set_uicurstar(value);
  // @@protoc_insertion_point(field_set:ProtoMsg.Msg_Hero_UpHeroStar_Rsp.uiCurStar)
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__
// -------------------------------------------------------------------

// -------------------------------------------------------------------

// -------------------------------------------------------------------

// -------------------------------------------------------------------


// @@protoc_insertion_point(namespace_scope)

}  // namespace ProtoMsg

PROTOBUF_NAMESPACE_OPEN

template <> struct is_proto_enum< ::ProtoMsg::MsgModule_Hero_MsgCmd> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::ProtoMsg::MsgModule_Hero_MsgCmd>() {
  return ::ProtoMsg::MsgModule_Hero_MsgCmd_descriptor();
}

PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)

#include <google/protobuf/port_undef.inc>
#endif  // GOOGLE_PROTOBUF_INCLUDED_GOOGLE_PROTOBUF_INCLUDED_msg_5fmodule_5fhero_2eproto