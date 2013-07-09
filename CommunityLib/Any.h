#pragma once
//Taken from NanaPro
//Link: http://nanapro.codeplex.com/
//Credits Jinhao(NanaPro creator)

#include <typeinfo>
#include <utility> //C++11 for std::move

namespace Util
{
	namespace nana
	{

		class any
		{
			struct super_type
			{
				virtual ~super_type(){}
				super_type& super_type::operator=(const super_type &rhs)
				{
					return assign(rhs);
				}
				virtual super_type& assign(const super_type&) = 0;
				virtual bool same(const super_type&) const = 0;
				virtual super_type* clone() const = 0;
			}; //end struct super_type

			template<typename T>
			struct object_type
				: public super_type
			{
				object_type(){}

				object_type(T const & obj)
					: object(obj)
				{}

				object_type(T && obj)
					: object(std::move(obj))
				{}

				object_type(const object_type& rhs)
					:object(rhs.object)
				{}

				virtual super_type& assign(const super_type& rhs)
				{
					if(this != &rhs)
					{
						auto other = dynamic_cast<const object_type*>(&rhs);
						if(other)
							object = other->object;	
					}
					return *this;	
				}

				virtual bool same(const super_type& rhs) const
				{
					return (dynamic_cast<const object_type*>(&rhs) != nullptr);
				}

				virtual super_type* clone() const
				{
					return new object_type(object);
				}

				T object;
			}; //end struct object_type
		public:
			template<typename T>
			any(const T & obj)
				: super_(new object_type<typename std::remove_reference<T>::type>(obj))
			{}

			template<typename T>
			any(T && obj)
				: super_(new object_type<typename std::remove_reference<T>::type>(std::move(obj)))
			{}

			any()
				:super_(nullptr)
			{}

			any(const any& rhs)
				:super_(rhs.super_ ? rhs.super_->clone() : nullptr)
			{}

			any(any&& r)
				:super_(r.super_)
			{
				r.super_ = nullptr;
			}

			~any()
			{
				delete super_;	
			}

			any& operator=(const any& rhs)
			{
				if(this != &rhs)
				{
					delete super_;
					super_ = (rhs.super_ ? rhs.super_->clone() : nullptr);
				}
				return *this;	
			}

			any& operator=(any&& r)
			{
				if(this != &r)
				{
					delete super_;
					super_ = r.super_;
					r.super_ = nullptr;
				}
				return *this;
			}

			bool same(const any &rhs) const
			{
				if(this != &rhs)
				{
					if(super_ && rhs.super_)
						return super_->same(*rhs.super_);
					else if(super_ || rhs.super_)
						return false;
				}
				return true;
			}

			template<typename T>
			any& operator=(T const &rhs)
			{
				T * obj = get<T>();
				if(nullptr == obj)
				{
					delete super_;
					super_ = new object_type<T>(rhs);
				}
				else
					*obj = rhs;
				return *this;
			}

			template<typename T>
			any & operator=(T && rhs)
			{
				typedef typename std::remove_const<T>::type type;
				type* obj = get<type>();
				if(nullptr == obj)
				{
					delete super_;
					super_ = new object_type<type>(std::move(rhs));
				}
				else
					*obj = std::move(rhs);
				return *this;
			}

			template<typename T>
			T * get() const
			{
				if(super_)
				{
					typedef typename std::remove_const<T>::type type;
					object_type<type>* obj = dynamic_cast<object_type<type>*>(super_);
					if(obj) return &(obj->object);
				}
				return nullptr;
			}

			template<typename T>
			operator T&() const
			{
				typedef typename std::remove_const<T>::type type;
				type *obj = get<type>();

				if(nullptr == obj)
					throw std::bad_cast();

				return *obj;
			}

			template<typename T>
			operator T*() const
			{
				typedef typename std::remove_const<T>::type type;
				return get<type>();
			}
		private:
			super_type * super_;
		};
	}//end namespace nana
}