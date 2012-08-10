//
// Copyright (C) 2012 Claudiu Tasca <clawgm [at] gmail.com>, Marius Petcu <marius
// [at] porkholt.org>, Nandor Licker <licker.nandor [at] gmail.com>
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files (the "Software"), to deal in
// the Software without restriction, including without limitation the rights to
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
// the Software, and to permit persons to whom the Software is furnished to do so,
// subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//


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