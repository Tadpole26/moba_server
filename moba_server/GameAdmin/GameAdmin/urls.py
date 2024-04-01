"""
URL configuration for GameAdmin project.

The `urlpatterns` list routes URLs to views. For more information please see:
    https://docs.djangoproject.com/en/4.2/topics/http/urls/
Examples:
Function views
    1. Add an import:  from my_app import views
    2. Add a URL to urlpatterns:  path('', views.home, name='home')
Class-based views
    1. Add an import:  from other_app.views import Home
    2. Add a URL to urlpatterns:  path('', Home.as_view(), name='home')
Including another URLconf
    1. Import the include() function: from django.urls import include, path
    2. Add a URL to urlpatterns:  path('blog/', include('blog.urls'))
"""
from django.contrib import admin
from django.urls import path

from account import views as accviews
from server import views as svrviews
from mail import views as mailviews

urlpatterns = [
    path('admin/', admin.site.urls),
    path('test/', svrviews.test, name='test'),
    #账号登录功能
    path('account_login/', accviews.account_login, name='account_login'),
    #账号信息展示
    path('account_show/', accviews.account_show, name='account_show'),
    #账号详细信息展示
    path('account_detail/', accviews.account_detail, name='account_detail'),
    #账号创建功能
    path('account_create/', accviews.account_create, name="account_create"),
    #修改账号密码
    path('account_update/', accviews.account_update, name="account_update"),
    #删除账号
    path('account_delete/', accviews.account_delete, name="account_delete"),

    #游戏配置展示
    path('server_showcfg/', svrviews.server_showcfg, name="server_showcfg"),
    #配置内容展示
    path('server_getcfg/', svrviews.server_getcfg, name="server_getcfg"),
    #添加游戏配置
    path('server_addcfg/', svrviews.server_addcfg, name="server_addcfg"),
    #下载游戏配置
    path('server_downcfg/', svrviews.server_downcfg, name="server_downcfg"),
    #删除游戏配置
    path('server_delcfg/', svrviews.server_delcfg, name="server_delcfg"),
    #配置操作记录
    path('server_logcfg/', svrviews.server_logcfg, name="server_logcfg"),

    #邮件展示
    path('mail_show/', mailviews.mail_show, name="mail_show"),
    #新增邮件
    path('mail_create/', mailviews.mail_create, name="mail_create"),
    #邮件日志
    path('mail_log/', mailviews.mail_log, name="mail_log"),

]
