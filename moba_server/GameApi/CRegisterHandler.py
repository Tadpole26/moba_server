from tornado.web import RequestHandler
from tornado.gen import coroutine
from tornado.concurrent import Future
import Utility
from CAccount import CAccount


#内部注册使用
#注册 http://127.0.0.1:9999/Register?device=ios&channel=1&platform=1&timestamp=54643533&sign=argfbfhgyg&acc_name=Duncan&acc_password=123456
class CRegisterHandler(RequestHandler):
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
        # 此处执行具体的任务
        params = {
            'device' : self.get_argument('device'),
            'channel' : self.get_argument('channel'),
            'platform' : self.get_argument('platform'),
            'timestamp' : self.get_argument('timestamp'),
            'sign' : self.get_argument('sign'),
            'acc_name' : self.get_argument('acc_name'),
            'acc_password' : self.get_argument('acc_password'),
        }

        result = {
            'msg'  : 'SUCCESS',
            'code' : 200
        }

        if Utility.CheckClientSign(params) == False:
            result['msg'] = 'REGISTER_SIGN_ERROR'
            result['code'] = 500
            future = Future()
            future.set_result(result)
            return future

        Account = CAccount(0, 0, self.settings['aiomysql'])
        msg, code, acc_name, acc_password = Account.AccountRegister(params['device'], 
                                                                    params['channel'], 
                                                                    params['platform'],
                                                                    params['acc_name'], 
                                                                    params['acc_password'])
        result['msg'] = msg
        result['code'] = code
        result['acc_name'] = acc_name
        result['acc_password'] = acc_password
        future = Future()
        future.set_result(result)
        return future