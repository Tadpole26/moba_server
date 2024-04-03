
# -*- coding: utf8 -*-

import sys, os, time
sys.dont_write_bytecode = True          #不生成字节码

import urllib, json, socket, struct, select, base64
import msg_module_pb2
import msg_module_login_pb2
import msgpack
import ctypes
import re
kernel32 = ctypes.windll.kernel32
kernel32.SetConsoleMode(kernel32.GetStdHandle(-11),7)


class session(object):
	def __init__(self, platform, uid, group):  #constructor
		self.platform = platform
		self.uid = uid
		self.group = group
		self.errinfo = {}
		self.sendcount = 0

	@staticmethod #static func
	def perror(msg):
		ts = time.time()
		ms = int((ts - int(ts)) * 1000)
		print ('\033[31m' + '[%s.%03d] %s' % (time.strftime("%m/%d %T", time.localtime(ts)), ms, msg) + '\033[0m')

	@staticmethod #static func
	def pwarning(msg):
		ts = time.time()
		ms = int((ts - int(ts)) * 1000)
		print ('\033[33m' + '[%s.%03d] %s' % (time.strftime("%m/%d %T", time.localtime(ts)), ms, msg) + '\033[0m')

	def connect(self, host, port):
		self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		self.socket.connect((host, port))  #连接网关
		self.resp = bytes()
        #header = struct.pack
		#self.log('%s找不到服务器信息%s' % (color.red, color.normal))

	#uint32_t    uiLen = 0;                     //uint32_t wLen;
    #uint16_t    uiFlag = GAME_BASE_FLAG;       //什么类型的包
    #uint8_t     usModuleId = 0;                //主类型  uint8 is ok
    #uint32_t    uiCmdId = 0;                   //次类型
    #uint32_t    uiCode = 0;                    //code值,错误码
    #uint32_t    uiSeqid = 0;                   //Sequence Id
    #uint32_t    uiCrc = 0;                     //校验
	def LoginVerityAccount(self):
		header = struct.pack('<HBIIII', 32768, 2, 5, 0, 1, 12345)
		proto_body = msg_module_login_pb2.Msg_Login_VerifyAccount_Req()
		proto_body.llUserId = 100054002
		proto_body.strUserName = 'rise_girl'
		proto_body.strPassword = 'Ws123456'
		body = proto_body.SerializeToString()
		request = struct.pack('<I', 4 + len(header) + len(body)) + header + body
		self.socket.send(request)
		self.pwarning(proto_body)
		self.rspPb = msg_module_login_pb2.Msg_Login_VerifyAccount_Rsp()


	def LoginCreateRole(self):
		header = struct.pack('<HBIIII', 32768, 2, 9, 0, 1, 12345)
		proto_body = msg_module_login_pb2.Msg_Login_CreateUser_Req()
		proto_body.uiGender = 0
		proto_body.strUserName = 'bbbbbb'
		proto_body.uiIconId = 12580
		body = proto_body.SerializeToString()
		request = struct.pack('<I', 4 + len(header) + len(body)) + header + body
		self.socket.send(request)
		self.pwarning(proto_body)
		self.rspPb = msg_module_login_pb2.Msg_Login_CreateUser_Rsp()

	def AwardItem(self):
		if self.sendcount != 0 :
			return
		header = struct.pack('<HBIIII', 32768, 2, 15, 0, 1, 12345)
		proto_body = msg_module_login_pb2.Msg_Login_AwardItem_Req()
		proto_body.uiItemId = 1
		proto_body.uiCount = 150000
		body = proto_body.SerializeToString()
		request = struct.pack('<I', 4 + len(header) + len(body)) + header + body
		self.socket.send(request)
		self.pwarning("send to award item cmd, packet head:" + str(4 + len(header)) + "|body:" + str(len(body)))
		self.rspPb = msg_module_login_pb2.Msg_Login_AwardItem_Rsp()
		self.sendcount += 1


	def loop(self):
		while True:
			rd, _, _ = select.select([self.socket], [], [], 30)
			if self.socket not in rd:
				self.pwarning('wait recive....')
				continue

			n = self.socket.recv(1024)
			if not n:
				self.perror('connection lost.')
				break

			self.resp += n
			while len(self.resp) > 4:
				plen = struct.unpack('<i', self.resp[:4])[0]
				if len(self.resp) < plen:
					self.perror('resp len size error! size:' + str(plen))
					break

				self.respose(self.resp[:plen], plen)
				self.resp = self.resp[plen:]

	def load(self):
		errfile = "../../server/logic_server/service/logic_errcode.h"
		with open(errfile,'r',encoding='utf-8') as f:
			#for line in f:
			errs = re.findall(r'\s*(\w+)\s*=\s*(-?\d+)(\s*,?\s*//\s*(.*))?', f.read())
			for err in errs :
				self.errinfo[int(err[1])] = err[3]
				self.pwarning(err[3])

	def respose(self, msg, size):
		#command, code = struct.unpack('<Hh', msg[10:14])	#获取包头协议号和错误码
		#self.pwarning('cammand:' + str(command) + '|code:'+str(code))
		len, flag, module, cmd, code, sid, crc = struct.unpack('<IHBIIII', msg[:23])
		self.pwarning('len:' + str(len) + '|flag:'+str(flag)+'|module:'+str(module)+'|cmd:'+str(cmd)+'|code:'+str(code)+'|sid:'+str(sid)+'|crc:'+str(crc))
		payload = msg[23:]
		if code == 1000 : 											#gate回复1000登录成功
			self.pwarning('login success.')
		elif code ==  1002:
			self.pwarning('user not exist, please to create user.')
			self.LoginCreateRole()
		elif code != 1000 and code != 1002 and code != 0:
			self.perror('rsp error! code:' + str(code))				#logic回复小于0错误码
		else :
			self.rspPb.ParseFromString(msg[23:])
			self.pwarning(self.rspPb)


sess = session(1, 10004, 1)	# plat_id uid, gourp_id
sess.connect('127.0.0.1', 18201)	#  host port 
sess.LoginVerityAccount()
#sess.LoginCreateRole()
sess.loop()