import time
from tornado.ioloop import IOLoop
from functools import partial
import Utility
from CAioMysqlClient import AioMysqlClient
from CMysqlClient import CMysqlClient
from CLogger import GetApiLog
log = GetApiLog()
#spawn_callback不会等到携程完成,直接返回,所以没有返回值

class CAccount:
    def __init__(self, accountId, curTime, aioMysql):
        self.m_accountId = accountId                         #账号id
        self.m_curTime = 0 if curTime == 0 else curTime      #请求时间
        self.m_aioMysql = aioMysql                           #mysql异步操作

    #玩家注册账号
    def AccountRegister(self, deviceId, channel, platform, acc_name, acc_password):
        if acc_name == '' or acc_password == '':
            return 'REGISTER_ACCOUNT_ERROR', 500, acc_name, acc_password
        
        loginRows = self.SelectAccountLoginByName(acc_name)
        if len(loginRows) > 0:
            return 'REGISTER_ACCOUNT_EXIST', 500, acc_name, acc_password
        
        cursor, conn, counts = self.GetUniqueID()
        self.m_accountId = cursor.lastrowid
        if self.m_accountId == 0 :
            return 'REGISTER_ACCOUNT_UID_ERROR', 500, acc_name, acc_password
        
        self.InsertLoginAccountInfo(acc_name, acc_password, self.m_curTime, '127:0:0:1', deviceId, channel, platform)

        return 'SUCCESS', 200, acc_name, acc_password

    #玩家账号登录
    def SdkCheck(self, accName, channel, deviceId, os, model, idfa_imei):
        result = {
            'msg' : 'SUCCESS',
            'code' : 200,
            'acc_id' : '',
            'acc_password' : ''
        }
        #loginTabName = Utility.GetTableHash('account', accName)
        loginRows = self.SelectAccountLoginByName(accName)
        if loginRows == None or len(loginRows) == 0:
            cursor, conn, counts = self.GetUniqueID()
            self.m_accountId = cursor.lastrowid
            if self.m_accountId == 0 :
                result['msg'] = 'REGISTER_ACCOUNT_UID_ERROR'
                result['code'] = 500
                return result
            
            self.InsertLoginAccountInfo(accName, '123456', self.m_curTime, '127:0:0:1', deviceId, channel, channel)
            result['acc_id'] = self.m_accountId
            result['acc_password'] = '123456'
            return result
        else:   #第一次登录
            self.m_accountId = loginRows[1]
            infoRow = self.SelectAccountInfo()
            if infoRow == None or len(infoRow) == 0:
                result['msg'] = 'LOGIN_ACCINFO_NOT_EXIST'
                result['code'] = 500
                return result
            result['acc_id'] = self.m_accountId
            result['acc_password'] = loginRows[2].decode('utf-8')
            return result
           


##############################数据库操作函数########################################  
    #根据账号名字获取登录账号信息
    def SelectAccountLoginByName(self, accName):
        sql = 'select * from account where acc_name = \"' + accName + '\";'
        db = CMysqlClient()
        return db.fetch_one(sql)
    
    #通过账号ID提取用户信息
    def SelectAccountInfo(self):
        tablName = Utility.GetAccountInfoTable(self.m_accountId)
        sql = 'select * from ' + tablName + ' where acc_id = ' + str(self.m_accountId)
        db = CMysqlClient()
        return db.fetch_one(sql)
    
    #插入登录表信息、账号表信息
    def InsertLoginAccountInfo(self, accName, accPwd, curTime, accIp, deviceId, channel, platform):
        tablName = Utility.GetAccountInfoTable(self.m_accountId)
        strAccId = str(self.m_accountId)
        sqlLogin = 'insert into account(acc_id, acc_name, acc_password) values(' + strAccId + ',\"' + accName + '\",\"' + accPwd + '\");'
        sqlInfo = 'insert into ' + tablName + '(acc_id, acc_first_login, dev_uid, acc_register_ip, channel, platform) values(' + strAccId + ',' + str(curTime) + ',\"' + deviceId + '\",\"' + accIp + '\",\"' + channel + '\",\"' + platform + '\");'
        sqls = {
            sqlLogin,
            sqlInfo
        }
        db = CMysqlClient()
        return db.execute_trans(sqls)

    #获取唯一账号
    def GetUniqueID(self):
        sql = 'replace into account_id (stub) values (\'a\');'
        db = CMysqlClient()
        return db.execute(sql)
    
    #插入账号登陆日志
    async def InsertLoginLog(self, accId, accName, channel, deviceId, clientVer, os, model, date):
        sql = 'insert into account_login_log(acc_id, acc_name, channel, device, client_ver, os, model, log_time)values(' 
        + accId + ', \"' + accName + '\", ' + channel + ', ' + deviceId + ', ' + clientVer + ', ' 
        + os + ', ' + model + ', ' + date + ')'
        cur = await self.m_mysqlClient.insert(sql)

    def InsertTest(self):
        i = 0
        while i < 1000:
            sql = 'insert into account(acc_id, acc_name, acc_password) values(' + str(i) + ',\"kingnet_' + str(i) + '\", \"123456\");'
            db = CMysqlClient()
            db.insert_one(sql)
            i += 1
    
