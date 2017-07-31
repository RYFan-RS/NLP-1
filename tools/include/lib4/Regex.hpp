/*
  ATLAN C++ Library: Regex.hpp
  
  ATLAN C++ Library is a library for personal studying.
  No licence is annouced.
  You can use or change any part of it.
  When you using it, inform the author is hoped but not required.
  
  Author: atlantise522@gmail.com
*/
#ifndef __ACL__REGEX_H__
#define __ACL__REGEX_H__
#include <stdio.h>
#include <stdlib.h>
#include <regex.h>
#include <iostream>
#include <vector>
#include <string.h>

namespace acl{

class Regex
{
public:
    Regex() {
        cflags = REG_EXTENDED;
        eflags = 0;
        regex = strdup( "" );
        regerr = regcomp( &preg, regex, 0 );
    }
    ~Regex() {
        regfree( &preg );
        free( regex );
    }

    // Match std::string "str" with regular expression "regexp".
    // flag = 'i' to ignore case (see REG_ICASE in regex.h)
    // Optional "result" contain matched std::strings. Index 0 whole match, 1-9 substd::string from ()
    // return REG_... code from regex.h. REG_NOERROR is 0.
    int match( std::string regexp, std::string str, std::vector<std::string> *result = NULL, char flag = REG_EXTENDED ) {
        regmatch_t match[ 10 ];
        int cf = cflags;
        if ( flag == 'i' ) set_cflags( REG_ICASE );
        else               reset_cflags( REG_ICASE );

        if ( set_regexp( regexp.c_str(), cf != cflags ) != REG_NOERROR ) return regerr;

        int regres = regexec( &preg, str.c_str(), 10, match, eflags );

        if ( regres == REG_NOERROR && result != NULL ) {
            result->clear();
            for ( int i = 0; match[ i ].rm_so != -1 && i < 10; i++ )
                result->push_back( str.substr( match[ i ].rm_so, match[ i ].rm_eo - match[ i ].rm_so ) );
        }
        return regres;
    }

    // Sed command s///.
    // Shortest help :-)  result=`echo -n "str" | sed 's/regexp/newstr/'`
    // "newstr" can contain &, \1..9 - see man sed
    int sub( std::string regexp, std::string str, std::string newstr, std::string *result = NULL, char flag = REG_EXTENDED ) {
        std::vector<std::string> res;

        int regres = match( regexp, str, &res, flag );

        if ( regres == REG_NOERROR && result != NULL ) {
            int pos = 0;
            // replace all & with res[ 0 ]. Do not replace "\&"
            while ( ( pos = newstr.find( '&', pos ) ) != std::string::npos ) {
                if ( pos > 0 )
                    if ( newstr[ pos - 1 ] == '\\' ) {
                        newstr.erase( pos - 1, 1 );
                        continue;
                    }
                newstr.replace( pos, 1, res[ 0 ] );
                pos += res[ 0 ].size();
            }

            std::string substr( "\\0" );
            // replace all \1..9 with res[ 1..9 ]. Do not replace "\\N"
            for ( int i = 1; i < res.size(); i++ ) {
                substr[ 1 ] = '0' + i;
                pos = 0;
                while ( ( pos = newstr.find( substr, pos )  ) != std::string::npos ) {
                    if ( pos > 0 )
                        if ( newstr[ pos - 1 ] == '\\' ) {
                            newstr.erase( pos, 1 );
                            continue;
                        }
                    newstr.replace( pos, 2, res[ i ] );
                    pos += res[ i ].size();
                }
            }
            // replace matched std::string in str with newstr
            pos = str.find( res[ 0 ] );
            str.replace( pos, res[ 0 ].size(), newstr );
            *result = str;
        }

        return regres;
    }

    // cflags for regcomp and eflags for regexec (see man regex)
    int set_cflags( int mask )   {
        cflags |= mask;
        return cflags;
    }
    int reset_cflags( int mask ) {
        cflags &= ~mask;
        return cflags;
    }
    int set_eflags( int mask )   {
        eflags |= mask;
        return eflags;
    }
    int reset_eflags( int mask ) {
        eflags &= ~mask;
        return eflags;
    }

private:
    int set_regexp( const char *regexp, int recompile = 0 ) {
        if ( !regexp || !*regexp ) return -1;

        if ( strcmp( regexp, regex ) ) {
            recompile = 1;
            free( regex );
            regex = strdup( regexp );
        }

        if ( recompile ) {
            regfree( &preg );
            regerr = regcomp( &preg, regex, cflags );
        }

        return regerr;
    }

private:
    int cflags;
    int eflags;
    char *regex;
    regex_t preg;
    int regerr;
};

}//namespace acl

#endif // __ACL__REGEX_H__

