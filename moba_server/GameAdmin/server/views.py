from django.shortcuts import render, redirect
from django.http import JsonResponse, FileResponse
import django.utils.timezone as timezone
from GameAdmin.settings import MEDIA_ROOT
from GameAdmin import mypage
import os, datetime
from xml.sax.handler import ContentHandler
import xml.sax
from kazoo.client import KazooClient
from server.models import CfgOptLog
from GameAdmin.settings import SERVER_SELECT_LIST
from server import models

def test(request):
    model = CfgOptLog
    template_name = 'test.html'
    return render(request, 'test.html')

def server_showcfg(request):
    #back_dict = {'code': 10000, 'msg': ''}
    zk = KazooClient(hosts='vcs.wozip.com:2181')
    zk.start()
    node = zk.get_children('/normal_config')
    node_list = []
    for data in node:
        node_info = {}
        node_info['name'] = data
        node_list.append(node_info)
    zk.stop()
    return render(request, 'server_showcfg.html', {'zkcfg_list' : node_list, 'select_list' : SERVER_SELECT_LIST})

def server_getcfg(request):
    cfgname = request.GET.get('name')
    cfgpath = MEDIA_ROOT + '/' + cfgname + '.xml'
    
    response = FileResponse(open(cfgpath, 'rb'))
    return response

def server_downcfg(request):
    cfgname = request.GET.get('name')
    cfgpath = MEDIA_ROOT + '/' + cfgname + '.xml'
    response = FileResponse(open(cfgpath, 'rb'))
    response['content_type'] ="application/octet-stream"
    response['Content-Disposition'] ='attachment; filename='+ os.path.basename(cfgpath)
    return response

def server_delcfg(request):
    cfgname = request.GET.get('name')
    #删除本地文件
    cfgpath = MEDIA_ROOT + '/' + cfgname + '.xml'
    isexist = os.path.exists(cfgpath)
    if isexist == True:
        os.remove(cfgpath)
    zk = KazooClient(hosts='vcs.wozip.com:2181')
    zk.start()
    #删除zk内容
    nodepath = '/normal_config/' + cfgname
    bexist = zk.exists(nodepath)
    if bexist != None:
        zk.delete(nodepath,recursive=True)
    zk.stop()
     #上传成功记录操作日志
    accname = request.user.username
    log = CfgOptLog(opt_acc_name = accname, opt_name = cfgname, opt_type = 'delete')
    log.save()
    return redirect('/server_showcfg/')

def server_addcfg(request):
    back_dict = {'code': 10000, 'msg': ''}
    if request.method == "POST":
        zk = KazooClient(hosts='vcs.wozip.com:2181')
        zk.start()
        files = request.FILES.getlist('upload_files')
        for f in files:
            filename = f.name
            lst = filename.split('.')
            if len(lst) != 2:
                back_dict['msg'] = '上传文件名错误'
                back_dict['code'] = 10010
                zk.stop()
                return JsonResponse(back_dict)
            #判断上传文件后缀
            if lst[1] != 'xml':
                back_dict['msg'] = '上传文件非xml文件'
                back_dict['code'] = 10010
                zk.stop()
                return JsonResponse(back_dict)
            
            bdata = f.read()
            strdata = bdata.decode('utf-8')
            try:
                xml.sax.parseString(strdata, ContentHandler())
            except Exception as e:
                msg = f.name + ' 格式错误'
                back_dict['code'] = 10010
                back_dict['msg'] = msg
                zk.stop()
                return JsonResponse(back_dict)
            #插入或更新节点
            nodepath = '/normal_config/' + lst[0]
            bexist = zk.exists(nodepath)
            if bexist == None:
                zk.create(nodepath, bdata, makepath=True)
            else:
                zk.set(nodepath, bdata)
            # 上传到本地
            destination = open(os.path.join(MEDIA_ROOT, filename), 'wb+')
            for chunk in f.chunks():
                destination.write(chunk)
            destination.close()
            #上传成功记录操作日志
            accname = request.user.username
            log = CfgOptLog(opt_acc_name = accname, opt_name = lst[0], opt_type = 'add')
            log.save()
        zk.stop()
        back_dict['code'] = 10000
        back_dict['url'] = '/server_showcfg/'
        return JsonResponse(back_dict)
    return render(request, 'server_addcfg.html')

def server_logcfg(request):
    accname = request.GET.get('accname')
    startdate = request.GET.get('startdate')
    enddate = request.GET.get('enddate')
    
    logset = []
    if accname == None or accname == '':
        logset = models.CfgOptLog.objects.filter(opt_time__range=(startdate, enddate))
    else :
        logset = models.CfgOptLog.objects.filter(opt_acc_name=accname,
                                                opt_time__range=(startdate, enddate))

    page_obj = mypage.Pagination(current_page=request.GET.get('page'), all_count=logset.count())
    page_queryset = logset[page_obj.start:page_obj.end]
    today= datetime.datetime.now().strftime('%Y-%m-%d')
    params = ''
    if accname != None:
        params += '&accname=' + accname 
    if startdate != None:
        params += '&startdate=' + startdate
    if enddate != None:
        params += '&enddate=' + enddate
    return render(request, 'server_logcfg.html', 
    { 
        'today' : today,
        'page_obj' : page_obj.page_html(params),
        'page_queryset' : page_queryset, 
        'select_list' : SERVER_SELECT_LIST
    })



