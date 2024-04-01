import hashlib
import Config as config

#根据某字段进行hash求余分表
def GetTableHash(table, strName, mod = 20):
    lowerStr = strName.lower()
    unsign = hash(lowerStr)
    if unsign > 2147483647:
        unsign -= 4294967296
    no = abs(unsign) % mod
    tableName = table + '_' + str(no)
    return tableName

#获取账号信息表名
def GetAccountInfoTable(accountId, mod = 5):
    no = int(accountId) % mod
    tableName = 'account_info_' + str(no)
    return tableName

#检查签名是否正确
def CheckClientSign(params):
    keyStr = ''
    sign = ''
    for key,value in params.items():
        if key == 'sign':
            sign = params['sign']
            continue
        keyStr += value
    keyStr += config.PRIVATE_KEY
    return True
