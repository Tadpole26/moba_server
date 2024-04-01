from django.db import models

# Create your models here.

#邮件唯一id
class mailUniqueId(models.Model):
    mail_id = models.BigAutoField(verbose_name='邮件唯一id', primary_key=True)
