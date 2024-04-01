from tornado.web import RequestHandler
from tornado.gen import coroutine
from tornado.concurrent import Future
import json
import Utility
from CGroup import CGroup
from CSdk import GetSdk

"""
获取服务器列表
请求格式:http://127.0.0.1:9999/Session?acc_id=huawei_10086&sdk=123456&channel=apple&key=abcdkey

返回格式:json数据
*       code:错误码(200正常)
*       msg:错误信息
*       acc_id:账号id
"""

class CSessionHandler(RequestHandler):
    @coroutine
    def get(self):
        # 可以同时获取POST和GET请求参数
        content = yield self._process()

        if not content:
            self.write_error(404)
        else:
            self.write(content)

    def _process(self):
        params = {
            'accId' : self.get_argument('acc_id'),                          #账号id
            'accPwd' : self.get_argument('accPwd'),                         #账号密码                                               
            'id' : self.get_argument('id'),                                 #区服id                  
            'sdk' : self.get_argument('sdk'),   
            'dev' : self.get_argument('dev'),                               #设备id
            'channel' : self.get_argument('channel'),                       #渠道id
            'version' : self.get_argument('version'),                       #应用版本
            'baseVer' : self.get_argument('baseVer'),                       #资源版本
            'platform' : self.get_argument('platform'),                     #平台
            'adult' : self.get_argument('adult'),                           #是否成年
            'certification' : self.get_argument('certification'),           #是否实名
            'sign' : self.get_argument('sign'),                             #签名                   
        }

        result = {
            'msg' : 'SUCCESS',
            'code' : 200
        }

        if params['accId'] == '' or params['accPwd'] == '' or params['id'] == '' or params['cahnnel'] == '':
            result['msg'] = 'SESSION_PARAMS_ERROR'
            result['code'] = 500

        group = CGroup(self.settings['aiomysql'])
        response = group.GetGroupList(
            params['accId'], 
            params['sdk'], 
            params['channel'])
       
        jRespose = json.dumps(response)
       
        future = Future()
        future.set_result(jRespose)
        return future