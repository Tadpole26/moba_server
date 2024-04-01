# -*- coding: utf-8 -*-
# Generated by the protocol buffer compiler.  DO NOT EDIT!
# source: msg_module_serverdb.proto
"""Generated protocol buffer code."""
from google.protobuf.internal import builder as _builder
from google.protobuf import descriptor as _descriptor
from google.protobuf import descriptor_pool as _descriptor_pool
from google.protobuf import symbol_database as _symbol_database
# @@protoc_insertion_point(imports)

_sym_db = _symbol_database.Default()


import result_code_pb2 as result__code__pb2


DESCRIPTOR = _descriptor_pool.Default().AddSerializedFile(b'\n\x19msg_module_serverdb.proto\x12\x08ProtoMsg\x1a\x11result_code.proto\"\xef\x01\n\x12MsgModule_ServerDB\"\xd8\x01\n\x06MsgCmd\x12\x18\n\x14Msg_ServerDB_GD_None\x10\x00\x12#\n\x1fMsg_ServerDB_GD_GetUserInfo_Req\x10\x01\x12#\n\x1fMsg_ServerDB_GD_GetUserInfo_Rsp\x10\x02\x12\"\n\x1eMsg_ServerDB_GD_CreateUser_Req\x10\x03\x12\"\n\x1eMsg_ServerDB_GD_CreateUser_Rsp\x10\x04\x12\"\n\x1eMsg_ServerDB_GD_UpdateBaseInfo\x10\x05\"\xc6\x01\n\x11ServerDB_BaseInfo\x12\x10\n\x08llUserId\x18\x01 \x01(\x03\x12\x12\n\nstrAccount\x18\x02 \x01(\t\x12\x13\n\x0bstrUserNick\x18\x03 \x01(\t\x12\x0f\n\x07iGender\x18\x04 \x01(\x05\x12\x12\n\niUserLevel\x18\x05 \x01(\x05\x12\x10\n\x08iUserExp\x18\x06 \x01(\x05\x12\x14\n\x0cllCreateTime\x18\x07 \x01(\x03\x12\x13\n\x0bllLoginTime\x18\x08 \x01(\x03\x12\x14\n\x0cllLogoutTime\x18\t \x01(\x03\"R\n\x0euser_pb_data_t\x12\x10\n\x08llUserId\x18\x01 \x01(\x03\x12.\n\toBaseInfo\x18\x02 \x01(\x0b\x32\x1b.ProtoMsg.ServerDB_BaseInfo\"B\n\x0b\x62uff_data_t\x12\x13\n\x0bstrBuffData\x18\x01 \x01(\x0c\x12\x10\n\x08iDataLen\x18\x02 \x01(\x05\x12\x0c\n\x04\x62Zip\x18\x03 \x01(\x08\"3\n\x1fMsg_ServerDB_GD_GetUserInfo_Req\x12\x10\n\x08llUserId\x18\x01 \x01(\x03\"\x82\x01\n\x1fMsg_ServerDB_GD_GetUserInfo_Rsp\x12(\n\toUserData\x18\x01 \x01(\x0b\x32\x15.ProtoMsg.buff_data_t\x12\x10\n\x08llUserId\x18\x02 \x01(\x03\x12#\n\x05\x65\x43ode\x18\x03 \x01(\x0e\x32\x14.ProtoMsg.ResultCode\"2\n\x1eMsg_ServerDB_GD_CreateUser_Req\x12\x10\n\x08llUserId\x18\x01 \x01(\x03\"W\n\x1eMsg_ServerDB_GD_CreateUser_Rsp\x12#\n\x05\x65\x43ode\x18\x01 \x01(\x0e\x32\x14.ProtoMsg.ResultCode\x12\x10\n\x08llUserId\x18\x02 \x01(\x03\"^\n\x1eMsg_ServerDB_GD_UpdateBaseInfo\x12\x10\n\x08llUserId\x18\x01 \x01(\x03\x12*\n\x05oInfo\x18\x02 \x01(\x0b\x32\x1b.ProtoMsg.ServerDB_BaseInfob\x06proto3')

_builder.BuildMessageAndEnumDescriptors(DESCRIPTOR, globals())
_builder.BuildTopDescriptorsAndMessages(DESCRIPTOR, 'msg_module_serverdb_pb2', globals())
if _descriptor._USE_C_DESCRIPTORS == False:

  DESCRIPTOR._options = None
  _MSGMODULE_SERVERDB._serialized_start=59
  _MSGMODULE_SERVERDB._serialized_end=298
  _MSGMODULE_SERVERDB_MSGCMD._serialized_start=82
  _MSGMODULE_SERVERDB_MSGCMD._serialized_end=298
  _SERVERDB_BASEINFO._serialized_start=301
  _SERVERDB_BASEINFO._serialized_end=499
  _USER_PB_DATA_T._serialized_start=501
  _USER_PB_DATA_T._serialized_end=583
  _BUFF_DATA_T._serialized_start=585
  _BUFF_DATA_T._serialized_end=651
  _MSG_SERVERDB_GD_GETUSERINFO_REQ._serialized_start=653
  _MSG_SERVERDB_GD_GETUSERINFO_REQ._serialized_end=704
  _MSG_SERVERDB_GD_GETUSERINFO_RSP._serialized_start=707
  _MSG_SERVERDB_GD_GETUSERINFO_RSP._serialized_end=837
  _MSG_SERVERDB_GD_CREATEUSER_REQ._serialized_start=839
  _MSG_SERVERDB_GD_CREATEUSER_REQ._serialized_end=889
  _MSG_SERVERDB_GD_CREATEUSER_RSP._serialized_start=891
  _MSG_SERVERDB_GD_CREATEUSER_RSP._serialized_end=978
  _MSG_SERVERDB_GD_UPDATEBASEINFO._serialized_start=980
  _MSG_SERVERDB_GD_UPDATEBASEINFO._serialized_end=1074
# @@protoc_insertion_point(module_scope)
