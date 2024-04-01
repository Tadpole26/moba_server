from django.db import models

#服务器配置操作日志
class CfgOptLog(models.Model):
    opt_acc_name = models.CharField(verbose_name='操作账号名', max_length=128)
    opt_time = models.DateTimeField(verbose_name='操作时间', auto_now_add=True)
    opt_name = models.CharField(verbose_name='操作配置名', max_length=128)
    opt_type = models.CharField(verbose_name='操作类型', max_length=128)

