from tornado.web import RequestHandler, Application
from tornado.options import options, define
from tornado.ioloop import IOLoop
import tornado.ioloop
from CRegisterHandler import CRegisterHandler
from CLoginHandler import CLoginHandler
from CGroupListHandler import CGroupListHandler
from CTestHandler import CTestHandler
from CAioMysqlClient import AioMysqlClient
from CMysqlClient import CMysqlClient
import Config as config
from CAccount import CAccount

async def ConnetMysql():
    mysql_client = AioMysqlClient(config.DB_TEST_HOST, config.DB_TEST_PORT,
                                  config.DB_TEST_USER, config.DB_TEST_PASSWORD, 
                                  config.DB_TEST_DBNAME)
    await mysql_client.init_pool()
    print("init_aiomysql_pool_success")
    return mysql_client

#设置路由配置
def MakeApp(config):
    options.parse_command_line
    handlers_routes = [
        ('/Register', CRegisterHandler),
        ('/Login', CLoginHandler),
        ('/GroupList', CGroupListHandler),
        ('/Test', CTestHandler)
    ] 
    #在RequestHandler中可以通过self.settings['mysql']获得CAioMysqlClient实例对象
    app = Application(handlers=handlers_routes, **config)
    return app

                
if __name__ == '__main__':
    config = {
        'aiomysql' : IOLoop.current().run_sync(ConnetMysql),
    }
   
    app = MakeApp(config)
    httpServer = tornado.httpserver.HTTPServer(app)
 
    httpServer.bind(9999)      #绑定在指定端口
    httpServer.start(1)        #开启进程数量
    #启动服务器
    tornado.ioloop.IOLoop.instance().start()