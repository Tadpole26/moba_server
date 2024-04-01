from django.db import models
from django.contrib.auth.models import AbstractUser

class Account(AbstractUser):
    """用户表"""
    phone = models.BigIntegerField(verbose_name='手机号', null=True, blank=True)  # blank参数用于控制admin后台管理 与数据库无关
    avatar = models.FileField(upload_to='avatar/', default='avatar/default.jpg', verbose_name='用户头像')
    register_time = models.DateTimeField(verbose_name='注册时间', auto_now_add=True)

    # 修改admin后台管理的表名
    class Meta:
        verbose_name_plural = '用户表'

    def __str__(self):
        return f'用户对象:{self.username}'
