# -*- coding: utf-8 -*-
# Generated by the protocol buffer compiler.  DO NOT EDIT!
# source: msg_module_http.proto
"""Generated protocol buffer code."""
from google.protobuf.internal import builder as _builder
from google.protobuf import descriptor as _descriptor
from google.protobuf import descriptor_pool as _descriptor_pool
from google.protobuf import symbol_database as _symbol_database
# @@protoc_insertion_point(imports)

_sym_db = _symbol_database.Default()


import result_code_pb2 as result__code__pb2


DESCRIPTOR = _descriptor_pool.Default().AddSerializedFile(b'\n\x15msg_module_http.proto\x12\x08ProtoMsg\x1a\x11result_code.proto\"-\n\x0eMsgModule_Http\"\x1b\n\x06MsgCmd\x12\x11\n\rMsg_Http_None\x10\x00\"-\n\nHttpRetMsg\x12\x0f\n\x07uiMsgId\x18\x01 \x01(\r\x12\x0e\n\x06strMsg\x18\x02 \x01(\x0c\"b\n\x10HttpGetUniqRolId\x12\x12\n\nllPlayerId\x18\x01 \x01(\x03\x12\x0f\n\x07uiState\x18\x02 \x01(\r\x12\x12\n\nuiServerId\x18\x03 \x01(\r\x12\x15\n\rstrPlayerName\x18\x04 \x01(\t\"\xd9\x01\n\x10HttpUserLoginRet\x12\x12\n\nllPlayerId\x18\x01 \x01(\x03\x12%\n\x07\x65Result\x18\x02 \x01(\x0e\x32\x14.ProtoMsg.ResultCode\x12\x10\n\x08uiGateId\x18\x03 \x01(\r\x12\x11\n\tiProvince\x18\x04 \x01(\x05\x12\x17\n\x0fstrProvinceName\x18\x05 \x01(\t\x12\x14\n\x0cstrSessionId\x18\x06 \x01(\t\x12\x11\n\tuiGmLevel\x18\x07 \x01(\r\x12\x10\n\x08strDevId\x18\x08 \x01(\t\x12\x11\n\tuiProArea\x18\t \x01(\r\"y\n\x11HttpModifyNameRet\x12\x12\n\nllPlayerId\x18\x01 \x01(\x03\x12%\n\x07\x65Result\x18\x02 \x01(\x0e\x32\x14.ProtoMsg.ResultCode\x12\x12\n\nuiServerId\x18\x03 \x01(\r\x12\x15\n\rstrPlayerName\x18\x04 \x01(\tb\x06proto3')

_builder.BuildMessageAndEnumDescriptors(DESCRIPTOR, globals())
_builder.BuildTopDescriptorsAndMessages(DESCRIPTOR, 'msg_module_http_pb2', globals())
if _descriptor._USE_C_DESCRIPTORS == False:

  DESCRIPTOR._options = None
  _MSGMODULE_HTTP._serialized_start=54
  _MSGMODULE_HTTP._serialized_end=99
  _MSGMODULE_HTTP_MSGCMD._serialized_start=72
  _MSGMODULE_HTTP_MSGCMD._serialized_end=99
  _HTTPRETMSG._serialized_start=101
  _HTTPRETMSG._serialized_end=146
  _HTTPGETUNIQROLID._serialized_start=148
  _HTTPGETUNIQROLID._serialized_end=246
  _HTTPUSERLOGINRET._serialized_start=249
  _HTTPUSERLOGINRET._serialized_end=466
  _HTTPMODIFYNAMERET._serialized_start=468
  _HTTPMODIFYNAMERET._serialized_end=589
# @@protoc_insertion_point(module_scope)