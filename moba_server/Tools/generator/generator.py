#!/usr/bin/python3
import sys, os, time
import ctypes
import sys, os, time
from jinja2 import Template
from field import FieldBase
from field import FieldUint32
from field import FieldInt
from field import FieldInt64
from field import FieldTime
from field import FieldString
from field import FieldBool
import xml.dom.minidom as minidom

kernel32 = ctypes.windll.kernel32
kernel32.SetConsoleMode(kernel32.GetStdHandle(-11),7)
#encoding:utf-8

def pwarning(txt):
    print('\033[5;33m' + txt + '\033[0m')

def perror(txt):
    print('\033[1;31m' + txt + '\033[0m')

class Document:
    def __init__(self, strFileName):
        self.m_strDocName = strFileName.split('.')[0]     #文件名
        self.m_strColName = ''              #表名
        self.m_strLowColName = ''           #表名小写
        self.m_strColType = ''              #表类型
        self.m_strStFile = ''               #结构文件名
        self.m_strtName = ''                #结构名
        self.m_strPkgFile = ''              #game内存文件名
        self.m_strPkgName = ''              #game内存类名
        self.m_strDBPkgFile = ''              #game内存文件名
        self.m_strDBPkgName = ''              #game内存类名
        self.m_strDBName = ''               #mongo表名

        self.m_listField = []               #字段信息

    def GenerateName(self):
        self.m_strLowColName = self.m_strColName.lower()
        self.m_strStFile = self.m_strColName + 'Struct'
        self.m_strStName = 'st' + self.m_strColName
        self.m_strPkgFile = 'C' + self.m_strColName + 'Package'
        self.m_strPkgName = 'C' + self.m_strColName + 'Package'
        self.m_strDBPkgFile = 'C' + self.m_strColName + 'DBPkg'
        self.m_strDBPkgName = 'C' + self.m_strColName + 'DBPkg'
        if self.m_strColType == 'object':
            self.m_strDBName = 'o_db_' + self.m_strColName.lower()
        else:
            self.m_strDBName = 'u_db_' + self.m_strColName.lower()

    def InitFromXml(self, stXmlPath):                               #将xml数据load到内存中
        if not os.path.isfile(stXmlPath):
            print(stXmlPath,'集合配置文件路径不存在:%s'%(stXmlPath))
            sys.exit()
        dom = minidom.parse(stXmlPath)
        docInfo = dom.documentElement                       #获取文档信息节点(根节点)
        self.m_strColName = docInfo.getAttribute('col_name')
        self.m_strColType = docInfo.getAttribute('col_type')
        if self.m_strColName == '' or self.m_strColType == '':
            perror("配置文件无表名或表类型!请检查:" + stXmlPath)
            return
        #获取每个字段信息节点
        fieldList = docInfo.getElementsByTagName('field_info')
        for fieldInfo in fieldList:
            strName = fieldInfo.getAttribute('name')
            strType = fieldInfo.getAttribute('type')
            strKey = fieldInfo.getAttribute('key')
            strDft = fieldInfo.getAttribute('default')
            strComment = fieldInfo.getAttribute('comment')
            cFiledBase = FieldBase(strName, strType, strKey, strDft, strComment)
            if strType == 'uint32':
                cFiledBase = FieldUint32(strName, strType, strKey, strDft, strComment)
            elif strType == 'int':
                cFiledBase = FieldInt(strName, strType, strKey, strDft, strComment)
            elif strType == 'int64_t':
                cFiledBase = FieldInt64(strName, strType, strKey, strDft, strComment)
            elif strType == 'time_t':
                cFiledBase = FieldTime(strName, strType, strKey, strDft, strComment)
            elif strType == 'string':
                cFiledBase = FieldString(strName, strType, strKey, strDft, strComment)
            elif strType == 'bool':
                 cFiledBase = FieldBool(strName, strType, strKey, strDft, strComment)
            else:
                perror("错误的配置字段类型!请检查:" + stXmlPath)

            #初始化每个字段信息    
            cFiledBase.GenerateName()
            self.m_listField.append(cFiledBase)

    def GenStructH(self, strResultPath):
        with open('./tpl_struct_h.j2', 'r', encoding='utf-8') as file:
            buffer = file.read()
            template = Template(buffer)
    
            data = {
                'doc_name' : self.m_strDocName,
               'col_name' : self.m_strColName,
               'col_type' : self.m_strColType,
               'st_name' : self.m_strStName,
               'field_list' : self.m_listField
            }  
            wf = open(strResultPath + self.m_strStFile + ".h", 'w', encoding='utf-8') 
            wf.write(template.render(data)) 
            wf.close()

    def GenPkgH(self, strResultPath):
        with open('../../Tools/generator/tpl_pkg_h.j2', 'r', encoding='utf-8') as file:
            buffer = file.read()
            template = Template(buffer)

            data = {
                'col_name' : self.m_strColName,
                'col_type' : self.m_strColType,
                'st_file' : self.m_strStFile,
                'st_name' : self.m_strStName,
                'pkg_name' : self.m_strPkgName,
                'field_list' : self.m_listField,
                'doc_name' : self.m_strDocName
            }
            wf = open(strResultPath + self.m_strPkgFile + ".h", 'w', encoding='utf-8')
            wf.write(template.render(data)) 
            wf.close()

    def  GenPkgCpp(self, strResultPath):
        with open('../../Tools/generator/tpl_pkg_cpp.j2', 'r', encoding='utf-8') as file:
            buffer = file.read()
            template = Template(buffer)
    
            data = {
                'col_name' : self.m_strColName,
                'col_lower_name' : self.m_strLowColName,
                'col_type' : self.m_strColType,
                'st_file' : self.m_strStFile,
                'st_name' : self.m_strStName,
                'pkg_file' : self.m_strPkgFile,
                'pkg_name' : self.m_strPkgName,
                'field_list' : self.m_listField,
                'doc_name' : self.m_strDocName
            }  
            wf = open(strResultPath + self.m_strPkgFile + ".cpp", 'w', encoding='utf-8') 
            wf.write(template.render(data)) 
            wf.close()

    def GenDBPkgH(self, strResultPath):
        with open('../../Tools/generator/tpl_dbpkg_h.j2', 'r', encoding='utf-8') as file:
            buffer = file.read()
            template = Template(buffer)

            data = {
                'col_name' : self.m_strColName,
                'col_type' : self.m_strColType,
                'st_file' : self.m_strStFile,
                'st_name' : self.m_strStName,
                'pkg_name' : self.m_strPkgName,
                'db_name' : self.m_strDBName,
                'dbpkg_name' : self.m_strDBPkgName,
                'field_list' : self.m_listField,
                'doc_name' : self.m_strDocName
            }
            wf = open(strResultPath + self.m_strDBPkgFile + ".h", 'w', encoding='utf-8')
            wf.write(template.render(data)) 
            wf.close()

    def GenDBPkgCpp(self, strResultPath):
        with open('../../Tools/generator/tpl_dbpkg_cpp.j2', 'r', encoding='utf-8') as file:
            buffer = file.read()
            template = Template(buffer)

            data = {
                'col_name' : self.m_strColName,
                'col_lower_name' : self.m_strLowColName,
                'col_type' : self.m_strColType,
                'st_file' : self.m_strStFile,
                'st_name' : self.m_strStName,
                'pkg_name' : self.m_strPkgName,
                'db_name' : self.m_strDBName,
                'dbpkg_name' : self.m_strDBPkgName,
                'dbpkg_file' : self.m_strDBPkgFile,
                'field_list' : self.m_listField,
                'doc_name' : self.m_strDocName
            }
            wf = open(strResultPath + self.m_strDBPkgFile + ".cpp", 'w', encoding='utf-8')
            wf.write(template.render(data)) 
            wf.close()
    
def main(argv):
    strXmlPath = "./xml"
    strStructPath = "../../Libs/utility/"                #生成struct文件目录
    strPkgPath = "../../GameServer/"                   #生成GameServer内存文件目录
    strDBPkgPath = "../../DBServer/"                 #生成DBServer内存文件目录

    for mainDir, subDir, nameList in os.walk(strXmlPath):
        for name in nameList:
            strXmlPathName = mainDir + "/" + name
            pwarning("开始生成目标文件:" + strXmlPathName)
            stDocument = Document(name)
            stDocument.InitFromXml(strXmlPathName)
            stDocument.GenerateName()
            stDocument.GenStructH(strStructPath)
            stDocument.GenPkgH(strPkgPath)
            stDocument.GenPkgCpp(strPkgPath)
            stDocument.GenDBPkgH(strDBPkgPath)
            stDocument.GenDBPkgCpp(strDBPkgPath)
            pwarning("目标文件生成成功:" + strXmlPathName)

if __name__ == "__main__":
    main(sys.argv)
    time.sleep(5)