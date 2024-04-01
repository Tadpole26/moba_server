from django.shortcuts import render
from django.http import JsonResponse
from django.contrib import auth
from account import models
from account.models import Account
from GameAdmin import mypage
from GameAdmin.settings import ACC_SELECT_LIST

# Create your views here.
def account_login(request):
    #return render(request, 'account_show.html', {'status' : '200'})
    back_dict = {'code': 10000, 'msg': ''}
    if request.method == 'POST':
        username = request.POST.get('username')
        password = request.POST.get('password')
        print(username)
        print(password)       
        user_obj = auth.authenticate(request, username=username, password=password)
        if user_obj:
            # 保存用户登录状态
            auth.login(request, user_obj)  # 执行之后就可以使用request.user获取登录用户对象
            back_dict['code'] = 10000
            back_dict['url'] = '/account_show/'
        else:
            back_dict['code'] = 10001
            back_dict['msg'] = '用户名或密码错误'
        return JsonResponse(back_dict)
    return render(request, 'account_login.html')

def account_show(request):
    accname = request.GET.get('accname')
    accset = []
    params = ''
    if accname == None or accname == '':
        accset = models.Account.objects.all()
    else :
        accset = models.Account.objects.filter(username=accname)
        params = '&' + accname
    acc_obj = mypage.Pagination(current_page=request.GET.get('page'), all_count=accset.count())
    page_queryset = accset[acc_obj.start:acc_obj.end]
    return render(request, 'account_show.html', 
                  {'page_obj' : acc_obj.page_html(params),
                   'page_queryset' : page_queryset,
                   'select_list' : ACC_SELECT_LIST
                   })

#账号详细信息展示
def account_detail(request):
    return render(request, 'test.html')

#账号创建功能
def account_create(request):
    back_dict = {'code': 10000, 'msg': ''}
    if request.method == 'POST':
        newname = request.POST.get('newname')
        newpwd = request.POST.get('newpwd')
        newpwdcheck = request.POST.get('newpwdcheck')
        print(newname)
        print(newpwd)
        print(newpwdcheck)
        if newpwd != newpwdcheck:
            back_dict['code'] = 10002
            back_dict['msg'] = '创建新号密码确认错误'
        else:
            user = Account.objects.create_user(username=newname, password=newpwd)
            user.save()
            back_dict['code'] = 10000
            back_dict['url'] = '/account_show/'
        return JsonResponse(back_dict)

    return render(request, 'account_create.html')

#账号修改信息功能
def account_update(request):
    back_dict = {'code': 10000, 'msg': ''}
    if request.method == 'POST':
        newpwd = request.POST.get('newpwd')
        newpwdcheck = request.POST.get('newpwdcheck')
        if newpwd != newpwdcheck:
            back_dict['code'] = 10005
            back_dict['msg'] = '修改新密码确认错误'
            return JsonResponse(back_dict)
        user = request.user
        user.set_password(newpwd)
        user.save()
        back_dict['code'] = 10000
        back_dict['url'] = '/account_login/'
        auth.logout(request)
        return JsonResponse(back_dict)

    return render(request, 'account_update.html')

#账号删除
def account_delete(request):
    back_dict = {'code': 10000, 'msg': ''}
    if request.method == 'POST':
        delname = request.POST.get('delname')
        delnamecheck = request.POST.get('delnamecheck')
        if delname != delnamecheck:
            back_dict['code'] = 10006
            back_dict['msg'] = '删除账号确认错误'
            return JsonResponse(back_dict)
        
        accinfo = Account.objects.get(username = delname)
        if accinfo == None:
            back_dict['code'] = 10007
            back_dict['msg'] = '删除账号不存在'
            return JsonResponse(back_dict)
        
        if request.user.is_superuser == False:
            back_dict['code'] = 10008
            back_dict['msg'] = '非超级用户无法删除账号'
            return JsonResponse(back_dict)

        models.Account.objects.filter(username=delname).delete() 
        back_dict['code'] = 10000
        back_dict['url'] = '/account_show/'
        return JsonResponse(back_dict)
        
    return render(request, 'account_delete.html')