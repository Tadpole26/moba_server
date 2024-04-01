
class CSdkBase(object):
    def Verify(self, sid, token, subChannel):
        print('login verify base')

class CIosSdk(CSdkBase):
    def Verify(self, sid, token, subChannel):
        if sid == '' or token == '':
            return ''
        
        #http 请求验证是否合法
        return ('ios'+sid)

def GetSdk(channel):
    if channel == 'apple':
        return CIosSdk()
    
    return None