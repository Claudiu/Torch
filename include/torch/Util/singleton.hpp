
#ifndef __SINGLETON_HPP__
#define __SINGLETON_HPP__

namespace Torch
{
	template <class T>
	class singleton_holder
	{
		T * instance;

		public:
		singleton_holder() : instance(NULL) { }
		virtual ~singleton_holder() 
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
	class singleton
	{
		static T * instance;
		static singleton_holder<T> holder;

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
		singleton() { }
		singleton(const singleton& s);
		singleton& operator = (singleton& rhs);
	};

	template<class T> singleton_holder<T> singleton<T>::holder;
	template<class T> T* singleton<T>::instance = NULL;
}

#endif /*__SINGLETON_HPP__*/