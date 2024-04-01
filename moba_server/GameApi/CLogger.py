import logging, time
import Singleton

@Singleton.singleton
class CLogger():
    def __init__(self):
        '''添加一个日志器'''
        print('init_logger_success')
        self.logger=logging.getLogger()
        self.logger.setLevel(logging.DEBUG)

    def fomt(self):
        '''设置格式器'''
        self.for1=logging.Formatter(fmt='[%(levelname)s]  [%(asctime)s] ------->>>>[%(message)s ]')
        self.for2=logging.Formatter(fmt='[%(levelname)s]  [%(asctime)s] [%(filename)s:%(lineno)d] ------->>>>[%(message)s ]')
        return self.for1,self.for2

    def hand(self):
        '''添加一个处理器'''
        self.h=logging.StreamHandler()
        self.h.setLevel(logging.WARNING)
        self.h.setFormatter(self.fomt()[0])#将格式器添加到处理器
        self.logger.addHandler(self.h) #将处理器添加到格式器

    def add_file(self,file):
        '''设置文件输出控制台'''
        self.f=logging.FileHandler(filename=file,mode='a',encoding='UTF-8')
        self.f.setLevel(logging.DEBUG)
        self.f.setFormatter(self.fomt()[1])
        self.logger.addHandler(self.f)#将文件输出控制台添加到日志器

    def info(self,file):
        self.hand()
        self.add_file(file)
        return self.logger
    
def GetDBLog():
    logger = CLogger()
    date = time.strftime("%Y_%m_%d",time.localtime())
    strFile = './log/db_' + date + '_log'
    log = logger.info(strFile)
    return log

def GetApiLog():
    logger = CLogger()
    date = time.strftime("%Y_%m_%d",time.localtime())
    strFile = './log/api_' + date + '_log'
    log = logger.info(strFile)
    return log

#logger=Log()
#a=logger.info('日志')

#a.debug('日志级别1')
#a.info('日志级别2')
#a.warning('日志级别3')
#a.error('日志级别4')
#a.critical('日志级别5')
