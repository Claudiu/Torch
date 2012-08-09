
#ifndef __SINGLETON_HPP__
#define __SINGLETON_HPP__

namespace Torch
{
	template <class T>
	class SingletonHolder
	{
		T * instance;

		public:
		SingletonHolder() : instance(NULL) { }
		virtual ~SingletonHolder() 
		{ 
			if (instance) 
			{
				delete instance;
			}
		}

		void setInstance(T * inst)
		{
			instance = inst;
		}

	};

	template <class T>
	class Singleton
	{
		static T * instance;
		static SingletonHolder<T> holder;

		public:
		static T& inst()
		{
			if (instance == static_cast<T*> (0))
			{
				instance = new T();
				holder.setInstance(instance);
			}

			return *instance;
		}

		static T* instPtr()
		{
			if (instance == static_cast<T*> (0))
			{
				instance = new T();
				holder.setInstance(instance);
			}

			return *instance;
		}

		protected:
		Singleton() { }
		Singleton(const Singleton & s);
		Singleton& operator = (Singleton & rhs);
	};

	template<class T> SingletonHolder<T> Singleton<T>::holder;
	template<class T> T* Singleton<T>::instance = NULL;
}

#endif /*__SINGLETON_HPP__*/
