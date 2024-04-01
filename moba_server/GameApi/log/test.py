

def process():
    print('hello world')
def inf_N():
    num = 1
    while num <= 100:
        temp = yield process
        num += 1
        print('fff' + str(temp))
    print('数据已经生成并拿走完毕!')
    return None

#当函数中有yield，则这个函数就是生成器
#生成器返回类型是types.GeneratorType 不会去执行生成器主模块中的代码
#每次调用GnerratorType对象的next方法,生成器函数执行到下一个yield语句
#函数执行中遇到yield会把num返回出去,则temp就是None,yield和return的区别是yield不会结束函数,而是阻塞在yield这行


if __name__ == '__main__':
    inf_N_generate = inf_N()
    while True:
        print(type(next(inf_N_generate)))