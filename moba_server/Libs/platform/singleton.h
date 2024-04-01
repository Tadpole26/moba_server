#pragma once
#include <cassert>
#include <cstdlib>
#include <mutex>

    /////////////////////////////////////////////////
    // ˵��: ������           
    /////////////////////////////////////////////////
    // 
    /**
     * ����ʵ����, �ο�loki�е�SingletonHolderʵ��
     * û��ʵ�ֶԵ����������ڵĹ���
     * ʹ��ʾ����������:
     class A : public TC_Singleton<A, CreateStatic,  DefaultLifetime>
     {
     public:
         A(){cout << "A" << endl;}
         ~A()
         {
             cout << "~A" << endl;
         }

         void test(){cout << "test A" << endl;}
     };
     *
     * ����Ĵ�����ʽ��CreatePolicyָ��, �����·�ʽ:
     * CreateUsingNew: �ڶ��в���new����
     * CreateStatic`: ��ջ�в���static����
     *
     * �����������ڹ�����LifetimePolicyָ��, �����·�ʽ:
     * DefaultLifetime:ȱʡ�������ڹ���
     *      ������������Ѿ�����, ���ǻ��е���, �ᴥ���쳣
     * PhoneixLifetime:������������
     *      ������������Ѿ�����, ���ǻ��е���, ���ٴ���һ��
     * NoDestroyLifetime:������
     *      ���󴴽��󲻻����������������
     * ͨ������ʵ���еķ�ʽ�Ϳ�����
     */

     ////////////////////////////////////////////////////////////////
     // ����CreatePolicy:������󴴽�����:
     // 
    template<typename T>
    class CreateUsingNew
    {
    public:
        /**
         * ����
         *
         * @return T*
         */
        static T* create()
        {
            return new T;
        }

        /**
         * �ͷ�
         * @param t
         */
        static void destroy(T* t)
        {
            delete t;
        }
    };

    template<typename T>
    class CreateStatic
    {
    public:
        //���Ŀռ�
        union MaxAlign
        {
            char t_[sizeof(T)];
            long double longDouble_;
        };

        /**
         * ����
         *
         * @return T*
         */
        static T* create()
        {
            static MaxAlign t;
            return new(&t) T;
        }

        /**
         * �ͷ�
         * @param t
         */
        static void destroy(T* t)
        {
            t->~T();
        }
    };

    ////////////////////////////////////////////////////////////////
    // ����LifetimePolicy:���������������ڹ���
    // 

    template<typename T>
    class DefaultLifetime
    {
    public:
        static void deadReference()
        {
            throw std::logic_error("singleton object has dead.");
        }

        static void scheduleDestruction(T*, void (*pFun)())
        {
            std::atexit(pFun);
        }
    };

    template<typename T>
    class PhoneixLifetime
    {
    public:
        static void deadReference()
        {
            _bDestroyedOnce = true;
        }

        static void scheduleDestruction(T*, void (*pFun)())
        {
            if (!_bDestroyedOnce)
                std::atexit(pFun);
        }
    private:
        static bool _bDestroyedOnce;
    };
    template <class T>
    bool PhoneixLifetime<T>::_bDestroyedOnce = false;

    template <typename T>
    struct NoDestroyLifetime
    {
        static void scheduleDestruction(T*, void (*)())
        {
        }

        static void deadReference()
        {
        }
    };

    //////////////////////////////////////////////////////////////////////
    // Singleton
    template
        <
        typename T,
        template<class> class CreatePolicy = CreateUsingNew,
        template<class> class LifetimePolicy = DefaultLifetime
        >
        class Singleton
    {
    public:
        typedef T  instance_type;
        typedef volatile T volatile_type;

        /**
         * ��ȡʵ��
         *
         * @return T*
         */
        static T* getInstance()
        {
            //����, ˫check����, ��֤��ȷ��Ч��
            if (!_pInstance)
            {
                _mutex.lock();
                if (!_pInstance)
                {
                    if (_destroyed)
                    {
                        LifetimePolicy<T>::deadReference();
                        _destroyed = false;
                    }
                    _pInstance = CreatePolicy<T>::create();
                    LifetimePolicy<T>::scheduleDestruction((T*)_pInstance, &destroySingleton);
                }
                _mutex.unlock();
            }

            return (T*)_pInstance;
        }

        virtual ~Singleton() {};

    protected:

        static void destroySingleton()
        {
            assert(!_destroyed);
            CreatePolicy<T>::destroy((T*)_pInstance);
            _pInstance = NULL;
            _destroyed = true;
        }
    protected:

        static std::mutex       _mutex;
        static volatile T*      _pInstance;
        static bool             _destroyed;

    protected:
        Singleton() {}
        Singleton(const Singleton&);
        Singleton& operator=(const Singleton&);
    };

    template <class T, template<class> class CreatePolicy, template<class> class LifetimePolicy>
    std::mutex Singleton<T, CreatePolicy, LifetimePolicy>::_mutex;

    template <class T, template<class> class CreatePolicy, template<class> class LifetimePolicy>
    bool Singleton<T, CreatePolicy, LifetimePolicy>::_destroyed = false;

    template <class T, template<class> class CreatePolicy, template<class> class LifetimePolicy>
    volatile T* Singleton<T, CreatePolicy, LifetimePolicy>::_pInstance = nullptr;



