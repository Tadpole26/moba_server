import aiomysql.cursors
import traceback
from tornado.ioloop import IOLoop


class AioMysqlClient():
    def __init__(self, host, port, username, password, db_name, **kwargs):
        self.host = host
        self.port = port
        self.username = username
        self.password = password
        self.db_name = db_name
        self.kwargs = kwargs            # 其他参数
        self.conn_pool = None           # 连接池
        self.is_connected = False       # 是否处于连接状态
        #self.lock = asyncio.Lock()      # 异步锁

    async def init_pool(self):
        """
        创建数据库连接
        :return:
        """
        try:
            self.conn_pool = await aiomysql.create_pool(
                host=self.host,
                port=self.port,
                user=self.username,
                password=self.password,
                db=self.db_name,
                **self.kwargs
            )
            self.is_connected = True
        except:
            print(traceback.format_exc())
            self.is_connected = False

        return self

    async def insert(self, sql, args= None):
        conn = await self.conn_pool.acquire()
        cur = await self.execute(conn, sql, args)
        await conn.commit()
        conn.close()            # 不是关闭连接，而是还到连接池中
        return cur

    async def fetch_one(self, sql, args=None):
        conn = await self.conn_pool.acquire()
        cur = await self.execute(conn, sql, args)
        if cur.rowcount == 0:
            return None
        return await cur.fetchone()

    async def fetch_all(self, sql, args=None):
        conn = await self.conn_pool.acquire()
        cur = await self.execute(conn, sql, args)
        if cur.rowcount == 0:
            return None
        return await cur.fetchall()

    async def execute(self, conn, sql, args=None):
        async with conn.cursor(aiomysql.DictCursor) as cur:
            await cur.execute(sql, args)            # 执行sql
            return cur
        