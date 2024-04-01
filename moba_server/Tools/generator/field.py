#!/usr/bin/python3

class FieldBase:
    def __init__(self, strName, strType, bKey, default, strComment):
        self.m_strName = strName         #原始字段名(aa_bb_cc)
        self.m_strType = strType         #字段类型(int|bool|uint64_t|std::string|userdata)
        self.m_strKey = True if bKey == 'true' else False
        self.m_strDft = default
        self.m_strComment = strComment
        #生成字段名
        nameList = self.m_strName.split('_')
        TmpName = ''
        for name in nameList:
            TmpName = TmpName + name[:1].upper() + name[1:]
        self.m_strBaseName = TmpName                    #基础名字(AaBbCc)
        self.m_strPartName = ''                         #局部变量名字(iAaBbCc)
        self.m_strMemName = ''                          #成员变量名字(m_iAaBbCc)
        self.m_strPbName = ''                           #字段内存名字(iaabbcc)
    
    def GenerateName(self):
        self.m_strPartName = self.m_strBaseName
        self.m_strMemName = self.m_strBaseName
        self.m_strPbName = self.m_strBaseName

######################################uint32#####################################
class FieldUint32(FieldBase):
    def GenerateName(self):
        self.m_strPartName = 'ui' + self.m_strBaseName
        self.m_strMemName = 'm_ui' + self.m_strBaseName
        self.m_strPbName  = 'ui' + self.m_strBaseName.lower()

######################################int32######################################
class FieldInt(FieldBase):
    def GenerateName(self):
        self.m_strPartName = 'i' + self.m_strBaseName
        self.m_strMemName = 'm_i' + self.m_strBaseName
        self.m_strPbName  = 'i' + self.m_strBaseName.lower()

######################################int64######################################
class FieldInt64(FieldBase):
    def GenerateName(self):
        self.m_strPartName = 'll' + self.m_strBaseName
        self.m_strMemName = 'm_ll' + self.m_strBaseName
        self.m_strPbName  = 'll' + self.m_strBaseName.lower()

######################################time_t######################################
class FieldTime(FieldBase):
    def GenerateName(self):
        self.m_strPartName = 'll' + self.m_strBaseName
        self.m_strMemName = 'm_ll' + self.m_strBaseName
        self.m_strPbName  = 'll' + self.m_strBaseName.lower()

######################################string######################################
class FieldString(FieldBase):
    def GenerateName(self):
        self.m_strPartName = 'str' + self.m_strBaseName
        self.m_strMemName = 'm_str' + self.m_strBaseName
        self.m_strPbName  = 'str' + self.m_strBaseName.lower()

#######################################bool########################################
class FieldBool(FieldBase):
    def GenerateName(self):
        self.m_strPartName = 'b' + self.m_strBaseName
        self.m_strMemName = 'm_b' + self.m_strBaseName
        self.m_strPbName  = 'b' + self.m_strBaseName.lower()