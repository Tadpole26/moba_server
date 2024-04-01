import time
from CMysqlPool import GetMyConPool
from CLogger import GetDBLog
dbLog = GetDBLog()
"""执行语句查询有结果返回结果没有返回0,增/删/改返回变更数据条数,没有返回0"""
class CMysqlClient(object):
    def __init__(self):
        self.dbPoolIns = GetMyConPool()  # 从数据池中获取连接
 
    # 封装执行命令
    def execute(self, sql, autoclose=False):
        """
        【主要判断是否有参数和是否执行完就释放连接】
        :param sql: 字符串类型,sql语句
        :return: 返回连接conn和游标cursor
        """
        cursor, conn = self.dbPoolIns.GetConn()  # 从连接池获取连接
        count = 0
        try:
            dbLog.debug(sql)
            count = cursor.execute(sql)
            conn.commit()
            if autoclose:
                self.close(cursor, conn)
        except Exception as e:
            pass
        return cursor, conn, count
    
    #执行事务
    def execute_trans(self, sqls, autoclose=False):
        cursor, conn = self.dbPoolIns.GetConn()  # 从连接池获取连接
        count = 0
        try:
            for sql in sqls:
                dbLog.debug(sql)
                count = cursor.execute(sql)
            conn.commit()
            if autoclose:
                self.close(cursor, conn)
        except Exception as e:
            dbLog.error(e)
            conn.rollback()
        return cursor, conn, count
 
    # 释放连接
    def close(self, cursor, conn):
        """释放连接归还给连接池"""
        cursor.close()
        conn.close()
 
    # 查询所有
    def fetch_all(self, sql):
        cursor = None
        conn = None
        count = None
        try:
            cursor, conn, count = self.execute(sql)
            res = cursor.fetchall()
            return res
        except Exception as e:
            dbLog.error(e)
            self.close(cursor, conn)
            return count
 
    # 查询单条
    def fetch_one(self, sql, param=None):
        cursor = None
        conn = None
        count = None
        try:
            cursor, conn, count = self.execute(sql, param)
            res = cursor.fetchone()
            self.close(cursor, conn)
            return res
        except Exception as e:
            dbLog.error(e)
            self.close(cursor, conn)
            return count
 
    # 增加
    def insert_one(self, sql):
        cursor = None
        conn = None
        count = None
        try:
            cursor, conn, count = self.execute(sql)
            conn.commit()
            self.close(cursor, conn)
            return count
        except Exception as e:
            dbLog.error(e)
            conn.rollback()
            self.close(cursor, conn)
            return count

    # 增加多行
    def insert_many(self, sql, param):
        """
        :param sql:
        :return:
        """
        cursor, conn, count = self.dbPoolIns.GetConn()
        try:
            cursor.executemany(sql, param)
            conn.commit()
            return count
        except Exception as e:
            dbLog.error(e)
            conn.rollback()
            self.close(cursor, conn)
            return count
 
    # 删除
    def delete(self, sql):
        cursor = None
        conn = None
        count = None
        try:
            cursor, conn, count = self.execute(sql)
            self.close(cursor, conn)
            return count
        except Exception as e:
            dbLog.error(e)
            conn.rollback()
            self.close(cursor, conn)
            return count
 
    # 更新
    def update(self, sql):
        cursor = None
        conn = None
        count = None
        try:
            cursor, conn, count = self.execute(sql)
            conn.commit()
            self.close(cursor, conn)
            return count
        except Exception as e:
            dbLog.error(e)
            conn.rollback()
            self.close(cursor, conn)
            return count
            
# if __name__ == "__main__":
#     db = MySqLHelper()
#     sql = "SELECT SLEEP(10)"
#     db.execute(sql)
#     time.sleep(20)
 
 
    # TODO 查询单条
    # sql1 = "select * from userinfo where name=%s"
    # args = "python"
    # ret = db.selectone(sql=sql1, param=args)
    # print(ret)  # (None, b"python", b"123456", b"0")
 
    # TODO 增加单条
    # sql2 = "insert into hotel_urls(cname,hname,cid,hid,url) values(%s,%s,%s,%s,%s)"
    # ret = db.insertone(sql2, ("1", "2", "1", "2", "2"))
    # print(ret)
 
    # TODO 增加多条
    # sql3 = "insert into userinfo (name,password) VALUES (%s,%s)"
    # li = li = [
    #     ("分省", "123"),
    #     ("到达","456")
    # ]
    # ret = db.insertmany(sql3,li)
    # print(ret)
 
    # TODO 删除
    # sql4 = "delete from  userinfo WHERE name=%s"
    # args = "xxxx"
    # ret = db.delete(sql4, args)
    # print(ret)
 
    # TODO 更新
    # sql5 = r"update userinfo set password=%s WHERE name LIKE %s"
    # args = ("993333993", "%old%")
    # ret = db.update(sql5, args)
    # print(ret)