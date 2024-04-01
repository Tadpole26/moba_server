from tornado.web import RequestHandler
from tornado.gen import coroutine
from tornado.concurrent import Future
import time
import Utility
from CAccount import CAccount

'''
Future类
class Future(object):
    def done(self):
        return self._done
    
    def result(self, timeout=None):
        self._clear_tb_log()
        if self._result is not None:
            return self._result
        if self._exc_info is not None:
            raise_exc_info(self._exc_info)
        self._check_done()
        return self._result
    
    def add_done_callback(self, fn):
    
    

    当一个函数中有yield,此时函数叫生成器
    调用生成器,返回得是生成器对象:Generator = get()
    可通过next(Generator) 或者Generator.send()来让生成器执行下一个yield语句
'''

#注册 http://127.0.0.1:9999/Test?
class CTestHandler(RequestHandler):
    @coroutine
    def get(self):
        timeBegin = time.time()
        print(timeBegin)
        print('\n')
        """ get 接口封装 """
        # 可以同时获取POST和GET请求参数
        content = yield self._process()
        #self._process()
        self.write(content)
        timeEnd = time.time()
        print(timeEnd)

    def _process(self):
        # 此处执行具体的任务
      
        account = CAccount(0, 0, self.settings['aiomysql'])
        account.InsertTest()
        future = Future()
        future.set_result("hello coreu")
        return future