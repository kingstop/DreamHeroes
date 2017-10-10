// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: battle.proto

#ifndef PROTOBUF_battle_2eproto__INCLUDED
#define PROTOBUF_battle_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 3000000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 3000000 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/metadata.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/generated_enum_reflection.h>
// @@protoc_insertion_point(includes)

namespace message {

// Internal implementation detail -- do not call these.
void protobuf_AddDesc_battle_2eproto();
void protobuf_AssignDesc_battle_2eproto();
void protobuf_ShutdownFile_battle_2eproto();


enum AliveState {
  AliveState_Alive = 1,
  AliveState_Dead = 2
};
bool AliveState_IsValid(int value);
const AliveState AliveState_MIN = AliveState_Alive;
const AliveState AliveState_MAX = AliveState_Dead;
const int AliveState_ARRAYSIZE = AliveState_MAX + 1;

const ::google::protobuf::EnumDescriptor* AliveState_descriptor();
inline const ::std::string& AliveState_Name(AliveState value) {
  return ::google::protobuf::internal::NameOfEnum(
    AliveState_descriptor(), value);
}
inline bool AliveState_Parse(
    const ::std::string& name, AliveState* value) {
  return ::google::protobuf::internal::ParseNamedEnum<AliveState>(
    AliveState_descriptor(), name, value);
}
enum ObjAttributeInt {
  ALiveState = 0,
  AFaction = 1,
  AObjMax = 2
};
bool ObjAttributeInt_IsValid(int value);
const ObjAttributeInt ObjAttributeInt_MIN = ALiveState;
const ObjAttributeInt ObjAttributeInt_MAX = AObjMax;
const int ObjAttributeInt_ARRAYSIZE = ObjAttributeInt_MAX + 1;

const ::google::protobuf::EnumDescriptor* ObjAttributeInt_descriptor();
inline const ::std::string& ObjAttributeInt_Name(ObjAttributeInt value) {
  return ::google::protobuf::internal::NameOfEnum(
    ObjAttributeInt_descriptor(), value);
}
inline bool ObjAttributeInt_Parse(
    const ::std::string& name, ObjAttributeInt* value) {
  return ::google::protobuf::internal::ParseNamedEnum<ObjAttributeInt>(
    ObjAttributeInt_descriptor(), name, value);
}
enum MoveObjAttributeInt {
  AMoveSpeed = 0,
  AMoveObjMax = 1
};
bool MoveObjAttributeInt_IsValid(int value);
const MoveObjAttributeInt MoveObjAttributeInt_MIN = AMoveSpeed;
const MoveObjAttributeInt MoveObjAttributeInt_MAX = AMoveObjMax;
const int MoveObjAttributeInt_ARRAYSIZE = MoveObjAttributeInt_MAX + 1;

const ::google::protobuf::EnumDescriptor* MoveObjAttributeInt_descriptor();
inline const ::std::string& MoveObjAttributeInt_Name(MoveObjAttributeInt value) {
  return ::google::protobuf::internal::NameOfEnum(
    MoveObjAttributeInt_descriptor(), value);
}
inline bool MoveObjAttributeInt_Parse(
    const ::std::string& name, MoveObjAttributeInt* value) {
  return ::google::protobuf::internal::ParseNamedEnum<MoveObjAttributeInt>(
    MoveObjAttributeInt_descriptor(), name, value);
}
enum UnitAttributeInt {
  AMaxHp = 0,
  AHp = 1,
  AManaType = 2,
  AMaxMana = 3,
  AMana = 4,
  ARunSpeed = 5,
  AWalkSpeed = 6,
  ADefence = 7,
  AShardCD = 8,
  AUnitMax = 9
};
bool UnitAttributeInt_IsValid(int value);
const UnitAttributeInt UnitAttributeInt_MIN = AMaxHp;
const UnitAttributeInt UnitAttributeInt_MAX = AUnitMax;
const int UnitAttributeInt_ARRAYSIZE = UnitAttributeInt_MAX + 1;

const ::google::protobuf::EnumDescriptor* UnitAttributeInt_descriptor();
inline const ::std::string& UnitAttributeInt_Name(UnitAttributeInt value) {
  return ::google::protobuf::internal::NameOfEnum(
    UnitAttributeInt_descriptor(), value);
}
inline bool UnitAttributeInt_Parse(
    const ::std::string& name, UnitAttributeInt* value) {
  return ::google::protobuf::internal::ParseNamedEnum<UnitAttributeInt>(
    UnitAttributeInt_descriptor(), name, value);
}
// ===================================================================


// ===================================================================


// ===================================================================

#if !PROTOBUF_INLINE_NOT_IN_HEADERS
#endif  // !PROTOBUF_INLINE_NOT_IN_HEADERS

// @@protoc_insertion_point(namespace_scope)

}  // namespace message

#ifndef SWIG
namespace google {
namespace protobuf {

template <> struct is_proto_enum< ::message::AliveState> : ::google::protobuf::internal::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::message::AliveState>() {
  return ::message::AliveState_descriptor();
}
template <> struct is_proto_enum< ::message::ObjAttributeInt> : ::google::protobuf::internal::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::message::ObjAttributeInt>() {
  return ::message::ObjAttributeInt_descriptor();
}
template <> struct is_proto_enum< ::message::MoveObjAttributeInt> : ::google::protobuf::internal::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::message::MoveObjAttributeInt>() {
  return ::message::MoveObjAttributeInt_descriptor();
}
template <> struct is_proto_enum< ::message::UnitAttributeInt> : ::google::protobuf::internal::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::message::UnitAttributeInt>() {
  return ::message::UnitAttributeInt_descriptor();
}

}  // namespace protobuf
}  // namespace google
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_battle_2eproto__INCLUDED