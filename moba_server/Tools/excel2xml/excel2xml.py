import sys
import os
import xlrd
from xml.dom import minidom

class Excel2Xml:
    def __init__(self):
        self.m_strDirPath = '../../Config/game_config'
        self.m_stEndVec = ['.xls', '.xlsx']

    #是否是excel文件
    def IsExcelFile(self, strFile):
        for end in self.m_stEndVec:
            if strFile.endswith(end) == True:
                return end
        return False

    #打开excel文件
    def OpenExcel(self, strFile):
        try:
            stData = xlrd.open_workbook(strFile, formatting_info=True)
            return stData
        except Exception as e:
            print("\033[31m" + e + "\033[0m")

    #读取单个excel分页
    def ReadSheet(self, stSheet, stXmlDoc):
        if (stSheet.nrows < 3 or stSheet.ncols <= 0):
            strWarning = 'sheet is empty or not name/type/comment, please check: ' + str(stSheet.name)
            print("\033[33m" + strWarning + "\033[0m")

        #从第四行开始循环每行
        root = stXmlDoc.documentElement
        for r in range(3, stSheet.nrows):
            child = stXmlDoc.createElement(stSheet.name)
            if stSheet.cell_value(r, 0) == '':
                strError = 'sheet:' + stSheet.name + '|row:' + str(r) + ' is empty, please check!'
                print("\033[31m" + strError + "\033[0m")
                break
            
            #循环每列
            for c in range(0, stSheet.ncols):
                strName = stSheet.cell_value(0, c)          #第一行第c列(字段名字)
                strType = stSheet.cell_value(1, c)          #第二行第c列(字段类型)
                strData = stSheet.cell_value(r, c)          #第r行(大于3)第c列
                if strType == '':
                    if strName != '' or strData != '':
                        strError = 'sheet:' + stSheet.name + '|name:' + strName + '|data:' + strData + ' type is empty, plase check!'
                        print("\033[31m" + strError + "\033[0m")
                    break
                
                if strType == 'int' or strType == 'uint' or type == 'bool':
                    if strData == '':
                        strData = 0
                elif strType == 'string':
                    if strData == 0.0:
                        strData = ''
                
                

                node = stXmlDoc.createElement(strName)
                if strType == 'string':
                    try:
                        text = stXmlDoc.createTextNode(str(strData))
                    except:
                        strError = 'sheet:' + stSheet.name + '|name:' + strName + '|data:' + strData + ' to xml fail!'
                else:
                    text = stXmlDoc.createTextNode(str(strData))

                node.appendChild(text)
                child.appendChild(node)
            root.appendChild(child)


    #读取所有excel分页
    def ReadAllSheet(self, stExcel, stXmlDoc):
        stSheets = stExcel.sheets()
        for sheet in stSheets:
            self.ReadSheet(sheet, stXmlDoc)

    def GetXmlDoc(self):
        doc = minidom.Document()
        xml = doc.createElement('xml')
        doc.appendChild(xml)
        return doc

    def WriteXml(self, strFile, doc):
        fd = open(strFile, 'wb')
        fd.write(doc.toprettyxml(indent = '\t', newl = '\n', encoding = 'utf-8'))
        fd.close()

    def StartTransform(self):
        print('start excel to xml.......')
        for mainDir, subDir, nameList in os.walk('../../Config/game_config'):
            for name in nameList:
                strFilePath = mainDir + "/" + name
                bEnd = self.IsExcelFile(strFilePath)
                if (bEnd == False):
                    continue
                print('transform excel start, file: ' + strFilePath)
                excel = self.OpenExcel(strFilePath)
                xmlDoc = self.GetXmlDoc()
                self.ReadAllSheet(excel, xmlDoc)
                strOutFile = strFilePath.replace(bEnd, '.xml')
                self.WriteXml(strOutFile, xmlDoc)
                print('transform excel end, file: ' + strOutFile)


def main(argv):
    stE2X = Excel2Xml()
    stE2X.StartTransform()


if __name__ == "__main__":
    main(sys.argv)