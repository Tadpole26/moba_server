
from CAioMysqlClient import AioMysqlClient
from CMysqlClient import CMysqlClient
from CLogger import GetApiLog
log = GetApiLog()

class CGroup:
    def __init__(self, aioMysql):
        self.m_aioMysql = aioMysql                           #mysql异步操作

    def GetGroupList(self, accId, groupId, channel):
        result = {
            'msg' : 'SUCCESS',
            'code' : 200
        }
        ListRows = self.SelectGroupList()
        if ListRows == None or len(ListRows) == 0:
            result['msg'] = 'GROUPLIST_NOT_GROUP'
            result['code'] = 500
            return result
        
        for group in ListRows:
            result['group_list'] = {
                'id' : group[0],
                'name' : group[1].decode('utf-8')
            }
        return result



    ##############################数据库操作函数########################################  
    #根据账号名字获取登录账号信息
    def SelectGroupList(self):
        sql = 'select * from group_list;'
        db = CMysqlClient()
        return db.fetch_all(sql)