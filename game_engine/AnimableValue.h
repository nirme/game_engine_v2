#pragma once

#ifndef _ANIMABLE_VALUE
#define _ANIMABLE_VALUE


#include "utility.h"
#include "Core_Defines.h"
#include "Base_Exception.h"
#include "Basic_Strings.h"



namespace GameEngine
{

	class Any
    {

	protected:
		Placeholder* mContent;



	protected:

		class Placeholder 
		{
		public:
			virtual ~Placeholder() {};
            virtual const std::type_info& getType() const = 0;
            virtual Placeholder* clone() const = 0;
			virtual void writeToStream( std::ostream& _o ) = 0;
        };


		template<typename ValueType>
		friend ValueType * any_cast(Any *);


		template<typename ValueType>
		class holder : public placeholder
		{
		public:

			holder(const ValueType & value) : held(value)
			{};

		public:

		virtual const std::type_info & getType() const
		{
			return typeid(ValueType);
		};

		virtual placeholder * clone() const
		{
			return OGRE_NEW_T(holder, MEMCATEGORY_GENERAL)(held);
		};

		virtual void writeToStream(std::ostream& o)
		{
			o << held;
		};


		public:

			ValueType held;

		};







	public: // constructors

        Any()
          : mContent(0)
        {
        }

        template<typename ValueType>
        explicit Any(const ValueType & value)
          : mContent(OGRE_NEW_T(holder<ValueType>, MEMCATEGORY_GENERAL)(value))
        {
        }

        Any(const Any & other)
          : mContent(other.mContent ? other.mContent->clone() : 0)
        {
        }

        virtual ~Any()
        {
            destroy();
        }

    public: // modifiers

        Any& swap(Any & rhs)
        {
            std::swap(mContent, rhs.mContent);
            return *this;
        }

        template<typename ValueType>
        Any& operator=(const ValueType & rhs)
        {
            Any(rhs).swap(*this);
            return *this;
        }

        Any & operator=(const Any & rhs)
        {
            Any(rhs).swap(*this);
            return *this;
        }

    public: // queries

        bool isEmpty() const
        {
            return !mContent;
        }

        const std::type_info& getType() const
        {
            return mContent ? mContent->getType() : typeid(void);
        }

		inline friend std::ostream& operator <<
			( std::ostream& o, const Any& v )
		{
			if (v.mContent)
				v.mContent->writeToStream(o);
			return o;
		}

		void destroy()
		{
			OGRE_DELETE_T(mContent, placeholder, MEMCATEGORY_GENERAL);
			mContent = NULL;
		}

    public: 

	    template<typename ValueType>
    	ValueType operator()() const
    	{
			if (!mContent) 
			{
				OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS,
					"Bad cast from uninitialised Any", 
					"Any::operator()");
			}
			else if(getType() == typeid(ValueType))
			{
             	return static_cast<Any::holder<ValueType> *>(mContent)->held;
			}
			else
			{
				StringUtil::StrStreamType str;
				str << "Bad cast from type '" << getType().name() << "' "
					<< "to '" << typeid(ValueType).name() << "'";
				OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS,
					 str.str(), 
					"Any::operator()");
			}
		}

		template <typename ValueType>
		ValueType get(void) const
		{
			if (!mContent) 
			{
				OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS,
					"Bad cast from uninitialised Any", 
					"Any::operator()");
			}
			else if(getType() == typeid(ValueType))
			{
             	return static_cast<Any::holder<ValueType> *>(mContent)->held;
			}
			else
			{
				StringUtil::StrStreamType str;
				str << "Bad cast from type '" << getType().name() << "' "
					<< "to '" << typeid(ValueType).name() << "'";
				OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS,
					 str.str(), 
					"Any::operator()");
			}
		}

    };












































	class AnimableValue
	{
	public:
		enum ValueType
		{
			INT,
			FLOAT,
			VECTOR2,
			VECTOR3,
			VECTOR4,
			QUATERNION,
			COLOUR,
			//RADIAN, 
			//DEGREE 
		};



	};

}

#endif// _ANIMABLE_VALUE
