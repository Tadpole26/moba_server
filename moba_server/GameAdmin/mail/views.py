from django.shortcuts import render
from django.http import JsonResponse
from django.contrib import auth
from account import models
from account.models import Account
from GameAdmin import mypage
from GameAdmin.settings import MAIL_SELECT_LIST
from datetime import datetime
import pymongo
import datetime, os
from mail.models import mailUniqueId

# Create your views here.
def mail_show(request):
    conn = pymongo.MongoClient('mongodb://admin:123456@localhost:27017')
    database = conn.u_group_1
    print(database)
    return render(request, 'mail_show.html', {'select_list' : MAIL_SELECT_LIST})

def mail_create(request):
    back_dict = {'code': 10000, 'msg': ''}
    today= datetime.datetime.now().strftime('%Y-%m-%d')
    if request.method == 'POST':
        title = request.POST.get('title')
        send = request.POST.get('send')
        content = request.POST.get('content')
        beginDate = request.POST.get('start')
        endDate = request.POST.get('end')
       
        if title == '' or send == '' or content == '':
            back_dict['code'] = 10101
            back_dict['msg'] = '创建邮件数据错误'
        else:
            mailId = mailUniqueId()
            mailId.save()
            uniqueId = mailId.mail_id*10000000000
            begin = datetime.datetime.strptime(beginDate, "%Y-%m-%d")
            end = datetime.datetime.strptime(endDate, "%Y-%m-%d")
            conn = pymongo.MongoClient('mongodb://admin:123456@localhost:27017')
            database = conn.db_global_mail
            table = database.g_mail_list
            table.insert_one({'_id': uniqueId,'title': title, 'sender': send, 'content': content, 'begin_date':begin, 'end_date':end})
            back_dict['code'] = 10000
            back_dict['url'] = '/mail_show/'
        return JsonResponse(back_dict)

    return render(request, 'mail_create.html',
                  {
                      'today' : today,
                  })

def mail_log(request):
    return render(request, 'mail_show.html', 
                  {
                    'select_list' : MAIL_SELECT_LIST,
                   })
