from tornado.web import RequestHandler
from tornado.gen import coroutine
from tornado.concurrent import Future
import json
import Utility
from CGroup import CGroup
from CSdk import GetSdk

"""
获取服务器列表
请求格式:http://127.0.0.1:9999/GroupList?acc_id=huawei_10086&sdk=123456&channel=apple&key=abcdkey

返回格式:json数据
*       code:错误码(200正常)
*       msg:错误信息
*       acc_id:账号id
"""

class CGroupListHandler(RequestHandler):
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
            'accId' : self.get_argument('acc_id'),                       #账号id
            'sdk' : self.get_argument('sdk'),                          
            'channel' : self.get_argument('channel'),                  
            'key' : self.get_argument('key'),                              #验证key
        }

        group = CGroup(self.settings['aiomysql'])
        response = group.GetGroupList(
            params['accId'], 
            params['sdk'], 
            params['channel'])
       
        jRespose = json.dumps(response)
       
        future = Future()
        future.set_result(jRespose)
        return future