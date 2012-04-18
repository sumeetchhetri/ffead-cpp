/*
	Copyright 2010, Sumeet Chhetri 
  
    Licensed under the Apache License, Version 2.0 (the "License"); 
    you may not use this file except in compliance with the License. 
    You may obtain a copy of the License at 
  
        http://www.apache.org/licenses/LICENSE-2.0 
  
    Unless required by applicable law or agreed to in writing, software 
    distributed under the License is distributed on an "AS IS" BASIS, 
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
    See the License for the specific language governing permissions and 
    limitations under the License.  
*/
#ifndef JASON_SPIRIT_VALUE
#define JASON_SPIRIT_VALUE

//          Copyright John W. Wilkinson 2007 - 2009.
// Distributed under the MIT License, see accompanying file LICENSE.txt

// json spirit version 4.01

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include <vector>
#include <map>
#include <string>
#include <cassert>
#include <boost/config.hpp> 
#include <boost/cstdint.hpp> 
#include <boost/shared_ptr.hpp> 

namespace json_spirit
{
    enum Value_type{ obj_type, array_type, str_type, bool_type, int_type, real_type, null_type };

    template< class Config >    // Config determines whether the value uses std::string or std::wstring and
                                // whether JSON Objects are represented as vectors or maps
    class Value_impl
    {
    public:

        typedef Config Config_type;
        typedef typename Config::String_type String_type;
        typedef typename Config::Object_type Object;
        typedef typename Config::Array_type Array;
        typedef typename String_type::const_pointer Const_str_ptr;  // eg const char*

        Value_impl();  // creates null value
        Value_impl( Const_str_ptr      value ); 
        Value_impl( const String_type& value );
        Value_impl( const Object&      value );
        Value_impl( const Array&       value );
        Value_impl( bool               value );
        Value_impl( int                value );
        Value_impl( boost::int64_t     value );
        Value_impl( boost::uint64_t    value );
        Value_impl( double             value );

        Value_impl( const Value_impl& other );

        bool operator==( const Value_impl& lhs ) const;

        Value_impl& operator=( const Value_impl& lhs );

        Value_type type() const;

        bool is_uint64() const;
        bool is_null() const;

        const String_type& get_str()    const;
        const Object&      get_obj()    const;
        const Array&       get_array()  const;
        bool               get_bool()   const;
        int                get_int()    const;
        boost::int64_t     get_int64()  const;
        boost::uint64_t    get_uint64() const;
        double             get_real()   const;

        Object& get_obj();
        Array&  get_array();

        template< typename T > T get_value() const;  // example usage: int    i = value.get_value< int >();
                                                     // or             double d = value.get_value< double >();

        static const Value_impl null;

    private:

        Value_type type_;

        typedef boost::shared_ptr< Object > Object_ptr;
        typedef boost::shared_ptr< Array > Array_ptr;

        String_type str_;
        Object_ptr obj_p_;
        Array_ptr array_p_;
        union
        {
           bool bool_;
           boost::int64_t i_;
           double d_;
        };
        bool is_uint64_;
    };

    // vector objects

    template< class Config >
    struct Pair_impl
    {
        typedef typename Config::String_type String_type;
        typedef typename Config::Value_type Value_type;

        Pair_impl( const String_type& name, const Value_type& value );

        bool operator==( const Pair_impl& lhs ) const;

        String_type name_;
        Value_type value_;
    };

    template< class String >
    struct Config_vector
    {
        typedef String String_type;
        typedef Value_impl< Config_vector > Value_type;
        typedef Pair_impl < Config_vector > Pair_type;
        typedef std::vector< Value_type > Array_type;
        typedef std::vector< Pair_type > Object_type;

        static Value_type& add( Object_type& obj, const String_type& name, const Value_type& value )
        {
            obj.push_back( Pair_type( name , value ) );

            return obj.back().value_;
        }
                
        static String_type get_name( const Pair_type& pair )
        {
            return pair.name_;
        }
                
        static Value_type get_value( const Pair_type& pair )
        {
            return pair.value_;
        }
    };

    // typedefs for ASCII

    typedef Config_vector< std::string > Config;

    typedef Config::Value_type  Value;
    typedef Config::Pair_type   Pair;
    typedef Config::Object_type Object;
    typedef Config::Array_type  Array;

    // typedefs for Unicode

#ifndef BOOST_NO_STD_WSTRING

    typedef Config_vector< std::wstring > wConfig;

    typedef wConfig::Value_type  wValue;
    typedef wConfig::Pair_type   wPair;
    typedef wConfig::Object_type wObject;
    typedef wConfig::Array_type  wArray;
#endif

    // map objects

    template< class String >
    struct Config_map
    {
        typedef String String_type;
        typedef Value_impl< Config_map > Value_type;
        typedef std::vector< Value_type > Array_type;
        typedef std::map< String_type, Value_type > Object_type;
        typedef typename Object_type::value_type Pair_type;

        static Value_type& add( Object_type& obj, const String_type& name, const Value_type& value )
        {
            return obj[ name ] = value;
        }
                
        static String_type get_name( const Pair_type& pair )
        {
            return pair.first;
        }
                
        static Value_type get_value( const Pair_type& pair )
        {
            return pair.second;
        }
    };

    // typedefs for ASCII

    typedef Config_map< std::string > mConfig;

    typedef mConfig::Value_type  mValue;
    typedef mConfig::Object_type mObject;
    typedef mConfig::Array_type  mArray;

    // typedefs for Unicode

#ifndef BOOST_NO_STD_WSTRING

    typedef Config_map< std::wstring > wmConfig;

    typedef wmConfig::Value_type  wmValue;
    typedef wmConfig::Object_type wmObject;
    typedef wmConfig::Array_type  wmArray;

#endif

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //
    // implementation

    template< class Config >
    const Value_impl< Config > Value_impl< Config >::null;

    template< class Config >
    Value_impl< Config >::Value_impl()
    :   type_( null_type )
    ,   is_uint64_( false )
    {
    }

    template< class Config >
    Value_impl< Config >::Value_impl( const Const_str_ptr value )
    :   type_( str_type )
    ,   str_( value )
    ,   is_uint64_( false )
    {
    }

    template< class Config >
    Value_impl< Config >::Value_impl( const String_type& value )
    :   type_( str_type )
    ,   str_( value )
    ,   is_uint64_( false )
    {
    }

    template< class Config >
    Value_impl< Config >::Value_impl( const Object& value )
    :   type_( obj_type )
    ,   obj_p_( new Object( value ) )
    ,   is_uint64_( false )
    {
    }

    template< class Config >
    Value_impl< Config >::Value_impl( const Array& value )
    :   type_( array_type )
    ,   array_p_( new Array( value ) )
    ,   is_uint64_( false )
    {
    }

    template< class Config >
    Value_impl< Config >::Value_impl( bool value )
    :   type_( bool_type )
    ,   bool_( value )
    ,   is_uint64_( false )
    {
    }

    template< class Config >
    Value_impl< Config >::Value_impl( int value )
    :   type_( int_type )
    ,   i_( value )
    ,   is_uint64_( false )
    {
    }

    template< class Config >
    Value_impl< Config >::Value_impl( boost::int64_t value )
    :   type_( int_type )
    ,   i_( value )
    ,   is_uint64_( false )
    {
    }

    template< class Config >
    Value_impl< Config >::Value_impl( boost::uint64_t value )
    :   type_( int_type )
    ,   i_( static_cast< boost::int64_t >( value ) )
    ,   is_uint64_( true )
    {
    }

    template< class Config >
    Value_impl< Config >::Value_impl( double value )
    :   type_( real_type )
    ,   d_( value )
    ,   is_uint64_( false )
    {
    }

    template< class Config >
    Value_impl< Config >::Value_impl( const Value_impl< Config >& other )
    :   type_( other.type() )
    ,   is_uint64_( other.is_uint64_ )
    {
        switch( type_ )
        {
            case str_type:   str_     = other.get_str();                               break;
            case obj_type:   obj_p_   = Object_ptr( new Object( other.get_obj() ) );   break;
            case array_type: array_p_ = Array_ptr ( new Array ( other.get_array() ) ); break;
            case bool_type:  bool_    = other.get_bool();                              break;
            case int_type:   i_       = other.get_int64();                             break;
            case real_type:  d_       = other.get_real();                              break;
            case null_type:                                                            break;
            default: assert( false );
        };
    }

    template< class Config >
    Value_impl< Config >& Value_impl< Config >::operator=( const Value_impl& lhs )
    {
        Value_impl tmp( lhs );

        std::swap( type_, tmp.type_ );
        str_    .swap( tmp.str_ );
        obj_p_  .swap( tmp.obj_p_ );
        array_p_.swap( tmp.array_p_ );
        std::swap( is_uint64_, tmp.is_uint64_ );

        switch( type_ )
        {
            case bool_type:  std::swap( bool_, tmp.bool_ ); break;
            case int_type:   std::swap( i_,    tmp.i_ );    break;
            case real_type:  std::swap( d_,    tmp.d_ );    break;
            default: break;
        };

        return *this;
    }

    template< class Config >
    bool Value_impl< Config >::operator==( const Value_impl& lhs ) const
    {
        if( this == &lhs ) return true;

        if( type() != lhs.type() ) return false;

        switch( type_ )
        {
            case str_type:   return get_str()   == lhs.get_str();
            case obj_type:   return get_obj()   == lhs.get_obj();
            case array_type: return get_array() == lhs.get_array();
            case bool_type:  return get_bool()  == lhs.get_bool();
            case int_type:   return ( get_int64() == lhs.get_int64() ) &&
                                    ( is_uint64() == lhs.is_uint64() );
            case real_type:  return get_real()  == lhs.get_real();
            case null_type:  return true;
        };

        assert( false );

        return false; 
    }

    template< class Config >
    Value_type Value_impl< Config >::type() const
    {
        return type_;
    }

    template< class Config >
    bool Value_impl< Config >::is_uint64() const
    {
        return is_uint64_;
    }

    template< class Config >
    bool Value_impl< Config >::is_null() const
    {
        return type() == null_type;
    }

    template< class Config >
    const typename Config::String_type& Value_impl< Config >::get_str() const
    {
        assert( type() == str_type );

        return str_;
    }

    template< class Config >
    const typename Value_impl< Config >::Object& Value_impl< Config >::get_obj() const
    {
        assert( type() == obj_type );

        return *obj_p_;
    }
     
    template< class Config >
    const typename Value_impl< Config >::Array& Value_impl< Config >::get_array() const
    {
        assert( type() == array_type );

        return *array_p_;
    }
     
    template< class Config >
    bool Value_impl< Config >::get_bool() const
    {
        assert( type() == bool_type );

        return bool_;
    }
     
    template< class Config >
    int Value_impl< Config >::get_int() const
    {
        assert( type() == int_type );

        return static_cast< int >( i_ );
    }
    
    template< class Config >
    boost::int64_t Value_impl< Config >::get_int64() const
    {
        assert( type() == int_type );

        return i_;
    }
    
    template< class Config >
    boost::uint64_t Value_impl< Config >::get_uint64() const
    {
        assert( type() == int_type );

        return static_cast< boost::uint64_t >( i_ );
    }

    template< class Config >
    double Value_impl< Config >::get_real() const
    {
        if( type() == int_type )
        {
            return is_uint64() ? static_cast< double >( get_uint64() )
                               : static_cast< double >( get_int64() );
        }

        assert( type() == real_type );

        return d_;
    }

    template< class Config >
    typename Value_impl< Config >::Object& Value_impl< Config >::get_obj()
    {
        assert( type() == obj_type );

        return *obj_p_;
    }

    template< class Config >
    typename Value_impl< Config >::Array& Value_impl< Config >::get_array()
    {
        assert( type() == array_type );

        return *array_p_;
    }

    template< class Config >
    Pair_impl< Config >::Pair_impl( const String_type& name, const Value_type& value )
    :   name_( name )
    ,   value_( value )
    {
    }

    template< class Config >
    bool Pair_impl< Config >::operator==( const Pair_impl< Config >& lhs ) const
    {
        if( this == &lhs ) return true;

        return ( name_ == lhs.name_ ) && ( value_ == lhs.value_ );
    }

    // converts a C string, ie. 8 bit char array, to a string object
    //
    template < class String_type >
    String_type to_str( const char* c_str )
    {
        String_type result;

        for( const char* p = c_str; *p != 0; ++p )
        {
            result += *p;
        }

        return result;
    }

    //

    namespace internal_
    {
        template< typename T >
        struct Type_to_type
        {
        };

        template< class Value > 
        int get_value( const Value& value, Type_to_type< int > )
        {
            return value.get_int();
        }
       
        template< class Value > 
        boost::int64_t get_value( const Value& value, Type_to_type< boost::int64_t > )
        {
            return value.get_int64();
        }
       
        template< class Value > 
        boost::uint64_t get_value( const Value& value, Type_to_type< boost::uint64_t > )
        {
            return value.get_uint64();
        }
       
        template< class Value > 
        double get_value( const Value& value, Type_to_type< double > )
        {
            return value.get_real();
        }
       
        template< class Value > 
        typename Value::String_type get_value( const Value& value, Type_to_type< typename Value::String_type > )
        {
            return value.get_str();
        }
       
        template< class Value > 
        typename Value::Array get_value( const Value& value, Type_to_type< typename Value::Array > )
        {
            return value.get_array();
        }
       
        template< class Value > 
        typename Value::Object get_value( const Value& value, Type_to_type< typename Value::Object > )
        {
            return value.get_obj();
        }
       
        template< class Value > 
        bool get_value( const Value& value, Type_to_type< bool > )
        {
            return value.get_bool();
        }
    }

    template< class Config >
    template< typename T > 
    T Value_impl< Config >::get_value() const
    {
        return internal_::get_value( *this, internal_::Type_to_type< T >() );
    }
}

#endif
