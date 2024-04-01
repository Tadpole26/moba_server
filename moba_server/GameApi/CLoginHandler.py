from tornado.web import RequestHandler
from tornado.gen import coroutine
from tornado.concurrent import Future
import json
import Utility
from CAccount import CAccount
from CSdk import GetSdk

"""
用户登录
请求格式:http://127.0.0.1:9999/Login?sid=huawei_10086&token=123456&dev=afgrthffht&channel=apple&os=fegrrr&model=iphon13promax&t=1678689223&sub_channel=vx&idfa_imei=ffeevbefe&sign=argfbfhgyg
*       n:账号名称
*       p:账号密码
*       dev:设备号
*       k:验证串
*       t:请求时间
返回格式:json数据
*       code:错误码(200正常)
*       msg:错误信息
*       acc_id:账号id
"""

class CLoginHandler(RequestHandler):
    @coroutine
    def get(self):
        """ get 接口封装 """
        # 可以同时获取POST和GET请求参数
        content = yield self._process()

        if not content:
            self.write_error(404)
        else:
            self.write(content)

    def _process(self):
        params = {
            'sid' : self.get_argument('sid'),                       #第三方账号id
            'token' : self.get_argument('token'),                   
            'timestamp' : self.get_argument('t'),                   #请求时间
            'dev' : self.get_argument('dev'),                       #设备id
            'os' : self.get_argument('os'),                         #操作系统版本
            'channel' : self.get_argument('channel'),               #渠道
            'sub_channel' : self.get_argument('sub_channel'),       #子渠道 qq wx
            'idfa_imei' : self.get_argument('idfa_imei'),
            'model' : self.get_argument('model'),                   #手机型号
            'sign' : self.get_argument('sign'),                     #加密签名
        }

        result = {
            'msg'  : 'SUCCESS',
            'code' : 200
        }

        if Utility.CheckClientSign(params) == False:
            result['msg'] = 'LOGIN_SIGN_ERROR'
            result['code'] = 500
            future = Future()
            future.set_result(result)
            return future

        sdk = GetSdk(params['channel'])
        if sdk == None:
            result['msg'] = 'LOGIN_SDK_NOT_EXIST'
            result['code'] = 500
            future = Future()
            future.set_result(result)
            return future
        
        accName = sdk.Verify(params['sid'], params['token'], params['sub_channel'])
        if accName == '':
            result['msg'] = 'LOGIN_ACC_NOT_EXIST'
            result['code'] = 500
            future = Future()
            future.set_result(result)
            return future

        account = CAccount(0, 0, self.settings['aiomysql'])
        response = account.SdkCheck(
            accName, 
            params['channel'], 
            params['dev'], 
            params['os'], 
            params['model'], 
            params['idfa_imei'])
       
        jRespose = json.dumps(response)
       
        future = Future()
        future.set_result(jRespose)
        return future