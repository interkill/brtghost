/*
    Copyright [2011] [Igor Bygaev] [http://brt.org.ua]
	
	email: avon.dn.ua@gmail.com
	
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>
*/

#include "util.h"

#include <stdio.h>

#include "stdlib.h"
#include <sys/stat.h>
#include "sha1.h"
#include <memory>
#include <boost/shared_array.hpp>
#include "zlib/zlib.h"

CSHA1 SHA1;

string sha1(const string& input)
{
    SHA1.Reset();
    SHA1.Update((unsigned char *)input.c_str(), input.size());
    SHA1.Final();
    unsigned char uResult[20];
    SHA1.GetHash(uResult);

    BYTEARRAY MapSHA1 = UTIL_CreateByteArray( uResult, 20 );
    string sResult = UTIL_ByteArrayToHexStringWithoutSpaces(MapSHA1);

    return sResult;
}

BYTEARRAY UTIL_CreateByteArray( char *a, int size )
{
	BYTEARRAY result( size );
	copy( a, a+size, result.begin() );
	return result;
}


BYTEARRAY UTIL_CreateByteArray( unsigned char *a, int size )
{
	BYTEARRAY result( size );
	copy( a, a+size, result.begin() );
	return result;
}

BYTEARRAY UTIL_CreateByteArray( unsigned char c )
{
	BYTEARRAY result( 1 );
	result[0] = c;
	return result;
}

BYTEARRAY UTIL_CreateByteArray( uint16_t i, bool reverse )
{
	BYTEARRAY result( 2 );

	if( reverse )
	{
		result[0] = i >> 8;
		result[1] = (unsigned char)i;
	}
	else
		*(uint16_t*)&result[0] = i;

	return result;
}

BYTEARRAY UTIL_CreateByteArray( uint32_t i, bool reverse )
{
	BYTEARRAY result( 4 );

	if( reverse )
	{
		result[0] = i >> 24;
		result[1] = i >> 16;
		result[2] = i >> 8;
		result[3] = i;
	}
	else
		*(uint32_t*)&result[0] = i;

	return result;
}

uint16_t UTIL_ByteArrayToUInt16( const BYTEARRAY& b, bool reverse, unsigned int start )
{
	if( b.size( ) < start + 2 )
		return 0;

	if ( reverse )
		return (uint16_t)( b[start] << 8 | b[start + 1] );
	else
		return *(uint16_t*)&b[start];
}

uint32_t UTIL_ByteArrayToUInt32( const BYTEARRAY& b, bool reverse, unsigned int start )
{
	if( b.size( ) < start + 4 )
		return 0;

	if ( reverse )
		return (uint32_t)( b[start] << 24 | b[start + 1] << 16 | b[start + 2] << 8 | b[start + 3] );
	else
		return *(uint32_t*)&b[start];
}

string UTIL_ByteArrayToDecString( const BYTEARRAY& b )
{
	if( b.empty( ) )
		return string( );

	string result;

	for( BYTEARRAY :: const_iterator i = b.begin( ); i != b.end( ); ++i )
		result += " " + UTIL_ToString( (uint32_t)(*i) );

	return result;
}

string UTIL_ByteArrayToHexString( const BYTEARRAY& b )
{
	if( b.empty( ) )
		return string( );

	string result;

	for( BYTEARRAY :: const_iterator i = b.begin( ); i != b.end( ); ++i )
	{
		if( *i < 16 )
			result += " 0" + UTIL_ToHexString( *i );
		else
			result += " " + UTIL_ToHexString( *i );
	}

	return result;
}



void UTIL_AppendByteArray( BYTEARRAY& b, const BYTEARRAY& append )
{
	b.insert( b.end( ), append.begin( ), append.end( ) );
}

void UTIL_AppendByteArray( BYTEARRAY& b, unsigned char *a, int size )
{
	UTIL_AppendByteArray( b, UTIL_CreateByteArray( a, size ) );
}

void UTIL_AppendByteArray( BYTEARRAY& b, const string& append, bool terminator )
{
	// append the string plus a null terminator

	b.insert( b.end( ), append.begin( ), append.end( ) );

	if( terminator )
		b.push_back( 0 );
}

void UTIL_AppendByteArray( BYTEARRAY& b, uint16_t i, bool reverse )
{
	UTIL_AppendByteArray( b, UTIL_CreateByteArray( i, reverse ) );
}

void UTIL_AppendByteArray( BYTEARRAY& b, uint32_t i, bool reverse )
{
	UTIL_AppendByteArray( b, UTIL_CreateByteArray( i, reverse ) );
}

string UTIL_ExtractCString( const BYTEARRAY &b, unsigned int start )
{
	// start searching the byte array at position 'start' for the first null value
	// if found, return the subarray from 'start' to the null value but not including the null value

	if( start < b.size( ) )
	{
		for( unsigned int i = start; i < b.size( ); i++ )
		{
			if( b[i] == 0 )
				return string( b.begin( ) + start, b.begin( ) + i );
		}

		// no null value found, return the rest of the byte array

		return string( b.begin( ) + start, b.end( ) );
	}

	return string( );
}

BYTEARRAY UTIL_ExtractCStringBytes( const BYTEARRAY& b, unsigned int start )
{
	// start searching the byte array at position 'start' for the first null value
	// if found, return the subarray from 'start' to the null value but not including the null value

	if( start < b.size( ) )
	{
		for( unsigned int i = start; i < b.size( ); i++ )
		{
			if( b[i] == 0 )
				return BYTEARRAY( b.begin( ) + start, b.begin( ) + i );
		}

		// no null value found, return the rest of the byte array

		return BYTEARRAY( b.begin( ) + start, b.end( ) );
	}

	return BYTEARRAY( );
}

string UTIL_ByteArrayToHexStringWithoutSpaces( const BYTEARRAY &b )
{
	if( b.empty( ) )
		return string( );

	string result = string();

	for( BYTEARRAY :: const_iterator i = b.begin( ); i != b.end( ); i++ )
	{
		if( *i < 16 )
			result += "0" + UTIL_ToHexString( *i );
		else
			result += UTIL_ToHexString( *i );
	}

	return result;
}

unsigned char UTIL_ExtractHex( BYTEARRAY &b, unsigned int start, bool reverse )
{
	// consider the byte array to contain a 2 character ASCII encoded hex value at b[start] and b[start + 1] e.g. "FF"
	// extract it as a single decoded byte

	if( start + 1 < b.size( ) )
	{
		unsigned int c;
		string temp = string( b.begin( ) + start, b.begin( ) + start + 2 );

		if( reverse )
			temp = string( temp.rend( ), temp.rbegin( ) );

		stringstream SS;
		SS << temp;
		SS >> hex >> c;
		return c;
	}

	return 0;
}

void UTIL_AddStrings( vector<string> &dest, vector<string> sourc )
{
	bool n;
	for(uint32_t o=0; o<sourc.size(); o++)
	{
		n = true;
		for(uint32_t p=0; p<dest.size(); p++)
		{
			if (sourc[o]==dest[p])
				n = false;
		}
		if (n)
			dest.push_back(sourc[o]);
	}
}

void UTIL_ExtractStrings( string s, vector<string> &v )
{
	// consider the string to contain strings, e.g. "one two three"
	v.clear();
	string istr;
	stringstream SS;
	SS << s;
	while( SS >> istr ) v.push_back(istr);

	return;
}

BYTEARRAY UTIL_ExtractNumbers( string s, unsigned int count )
{
	// consider the string to contain a bytearray in dec-text form, e.g. "52 99 128 1"

	BYTEARRAY result;
	unsigned int c;
	stringstream SS;
	SS << s;

	for( unsigned int i = 0; i < count; i++ )
	{
		if( SS.eof( ) )
			break;

		SS >> c;

		// todotodo: if c > 255 handle the error instead of truncating

		result.push_back( (unsigned char)c );
	}

	return result;
}

BYTEARRAY UTIL_ExtractHexNumbers( string s )
{
	// consider the string to contain a bytearray in hex-text form, e.g. "4e 17 b7 e6"

	BYTEARRAY result;
	unsigned int c;
	stringstream SS;
	SS << s;

	while( !SS.eof( ) )
	{
		SS >> hex >> c;

		// todotodo: if c > 255 handle the error instead of truncating

		result.push_back( (unsigned char)c );
	}

	return result;
}

void Replace( string &Text, string Key, string Value )
{
	// don't allow any infinite loops

	if( Value.find( Key ) != string :: npos )
		return;

	string :: size_type KeyStart = Text.find( Key );

	while( KeyStart != string :: npos )
	{
		Text.replace( KeyStart, Key.size( ), Value );
		KeyStart = Text.find( Key );
	}
}

string UTIL_ToString2 ( double d)
{
	char buffer[75];
#ifdef WIN32
	sprintf(buffer, "%.2f",d);
#else
	sprintf(buffer, "%.2f",d);
#endif
	string result = buffer;
	return result;
}

string UTIL_ToHexString( uint32_t i )
{
	string result;
	stringstream SS;
	SS << std :: hex << i;
	SS >> result;
	return result;
}

// todotodo: these UTIL_ToXXX functions don't fail gracefully, they just return garbage (in the uint case usually just -1 casted to an unsigned type it looks like)

uint16_t UTIL_ToUInt16( string &s )
{
	uint16_t result;
	stringstream SS;
	SS << s;
	SS >> result;
	return result;
}

uint32_t UTIL_ToUInt32( string &s )
{
	uint32_t result;
	stringstream SS;
	SS << s;
	SS >> result;
	return result;
}

int16_t UTIL_ToInt16( string &s )
{
	int16_t result;
	stringstream SS;
	SS << s;
	SS >> result;
	return result;
}

int32_t UTIL_ToInt32( string &s )
{
	int32_t result;
	stringstream SS;
	SS << s;
	SS >> result;
	return result;
}

double UTIL_ToDouble( string &s )
{
	double result;
	stringstream SS;
	SS << s;
	SS >> result;
	return result;
}

static inline int UTIL_ToInt(double d)
{
  const double t = d + 6755399441055744.0;
  return *((int *)(&t));
}

string UTIL_MSToString( uint32_t ms )
{
	string MinString = UTIL_ToString( ( ms / 1000 ) / 60 );
	string SecString = UTIL_ToString( ( ms / 1000 ) % 60 );

	if( MinString.size( ) == 1 )
		MinString.insert( 0, "0" );

	if( SecString.size( ) == 1 )
		SecString.insert( 0, "0" );

	return MinString + "m" + SecString + "s";
}

bool UTIL_FileExists( string file )
{
	struct stat fileinfo;

	if( stat( file.c_str( ), &fileinfo ) == 0 )
		return true;

	return false;
}

string UTIL_FileRead( string file, uint32_t start, uint32_t length )
{
	ifstream IS;
	IS.open( file.c_str( ), ios :: binary );

	if( IS.fail( ) )
	{
		CONSOLE_Print( "[UTIL] warning - unable to read file part [" + file + "]" );
		return string( );
	}

	// get length of file

	IS.seekg( 0, ios :: end );
	uint32_t FileLength = IS.tellg( );

	if( start > FileLength )
	{
		IS.close( );
		return string( );
	}

	IS.seekg( start, ios :: beg );

	// read data

	char *Buffer = new char[length];
	IS.read( Buffer, length );
	string BufferString = string( Buffer, IS.gcount( ) );
	IS.close( );
	delete [] Buffer;
	return BufferString;
}

string UTIL_FileRead( string file )
{
	ifstream IS;
	IS.open( file.c_str( ), ios :: binary );

	if( IS.fail( ) )
	{
		CONSOLE_Print( "[UTIL] warning - unable to read file [" + file + "]" );
		return string( );
	}

	// get length of file

	IS.seekg( 0, ios :: end );
	uint32_t FileLength = IS.tellg( );
	IS.seekg( 0, ios :: beg );

	// read data

	char *Buffer = new char[FileLength];
	IS.read( Buffer, FileLength );
	string BufferString = string( Buffer, IS.gcount( ) );
	IS.close( );
	delete [] Buffer;

	if( BufferString.size( ) == FileLength )
		return BufferString;
	else
		return string( );
}

bool UTIL_FileWrite( string file, unsigned char *data, uint32_t length )
{
	ofstream OS;
	OS.open( file.c_str( ), ios :: binary );

	if( OS.fail( ) )
	{
		CONSOLE_Print( "[UTIL] warning - unable to write file [" + file + "]" );
		return false;
	}

	// write data

	OS.write( (const char *)data, length );
	OS.close( );
	return true;
}

string UTIL_FileSafeName( string fileName )
{
	string :: size_type BadStart = fileName.find_first_of( "\\/:*?<>|" );

	while( BadStart != string :: npos )
	{
		fileName.replace( BadStart, 1, 1, '_' );
		BadStart = fileName.find_first_of( "\\/:*?<>|" );
	}

	return fileName;
}

string UTIL_AddPathSeperator( string path )
{
	if( path.empty( ) )
		return string( );

#ifdef WIN32
	char Seperator = '\\';
#else
	char Seperator = '/';
#endif

	if( *(path.end( ) - 1) == Seperator )
		return path;
	else
		return path + string( 1, Seperator );
}

BYTEARRAY UTIL_EncodeStatString( BYTEARRAY &data )
{
	unsigned char Mask = 1;
	BYTEARRAY Result;

	for( unsigned int i = 0; i < data.size( ); i++ )
	{
		if( ( data[i] % 2 ) == 0 )
			Result.push_back( data[i] + 1 );
		else
		{
			Result.push_back( data[i] );
			Mask |= 1 << ( ( i % 7 ) + 1 );
		}

		if( i % 7 == 6 || i == data.size( ) - 1 )
		{
			Result.insert( Result.end( ) - 1 - ( i % 7 ), Mask );
			Mask = 1;
		}
	}

	return Result;
}

BYTEARRAY UTIL_DecodeStatString( BYTEARRAY &data )
{
	unsigned char Mask = 0;
	BYTEARRAY Result;

	for( unsigned int i = 0; i < data.size( ); i++ )
	{
		if( ( i % 8 ) == 0 )
			Mask = data[i];
		else
		{
			if( ( Mask & ( 1 << ( i % 8 ) ) ) == 0 )
				Result.push_back( data[i] - 1 );
			else
				Result.push_back( data[i] );
		}
	}

	return Result;
}

bool UTIL_IsLanIP( BYTEARRAY ip )
{
	if( ip.size( ) != 4 )
		return false;

	// thanks to LuCasn for this function

	// 127.0.0.1
	if( ip[0] == 127 && ip[1] == 0 && ip[2] == 0 && ip[3] == 1 )
		return true;

	// 10.x.x.x
	if( ip[0] == 10 )
		return true;

	// 172.16.0.0-172.31.255.255
	if( ip[0] == 172 && ip[1] >= 16 && ip[1] <= 31 )
		return true;

	// 192.168.x.x
	if( ip[0] == 192 && ip[1] == 168 )
		return true;

	// RFC 3330 and RFC 3927 automatic address range
	if( ip[0] == 169 && ip[1] == 254 )
		return true;

	return false;
}

bool UTIL_IsLocalIP( BYTEARRAY ip, vector<BYTEARRAY> &localIPs )
{
	if( ip.size( ) != 4 )
		return false;

	for( vector<BYTEARRAY> :: iterator i = localIPs.begin( ); i != localIPs.end( ); i++ )
	{
		if( (*i).size( ) != 4 )
			continue;

		if( ip[0] == (*i)[0] && ip[1] == (*i)[1] && ip[2] == (*i)[2] && ip[3] == (*i)[3] )
			return true;
	}

	return false;
}

void UTIL_Replace( string &Text, string Key, string Value )
{
	// don't allow any infinite loops

	if( Value.find( Key ) != string :: npos )
		return;

	string :: size_type KeyStart = Text.find( Key );

	while( KeyStart != string :: npos )
	{
		Text.replace( KeyStart, Key.size( ), Value );
		KeyStart = Text.find( Key );
	}
}

vector<string> UTIL_Tokenize( string s, char delim )
{
	vector<string> Tokens;
	string Token;

	for( string :: iterator i = s.begin( ); i != s.end( ); i++ )
	{
		if( *i == delim )
		{
			if( Token.empty( ) )
				continue;

			Tokens.push_back( Token );
			Token.clear( );
		}
		else
			Token += *i;
	}

	if( !Token.empty( ) )
		Tokens.push_back( Token );

	return Tokens;
}

uint32_t UTIL_Factorial( uint32_t x )
{
	uint32_t Factorial = 1;

	for( uint32_t i = 2; i <= x; i++ )
		Factorial *= i;

	return Factorial;
}


string UTIL_ToBinaryString( uint32_t data)
{
	string Result;
	uint32_t mask = 1;
	for (unsigned int i=1;i<=8;i++)
	{
		if (i>1)
			Result +=" ";
		if (data & mask)
			Result +="1";
		else
			Result +="0";
		if (mask==1)
			mask = 2;
		else
			mask =mask*2;
	}
	return Result;
}

/* ValidHostChar: Return TRUE if the specified character is valid
                  for a host name, i.e. A-Z or 0-9 or -.: */
bool ValidHostChar(char ch)
{
	return( isalpha(ch) || isdigit(ch) || ch=='-' || ch=='.' || ch==':' );
}


/* ParseURL: Used to break apart a URL such as
             http://www.wdj.com:80/index.htm into protocol, port, host
             and request. */
void ParseURL(char* url,char* protocol,int lprotocol, char* host,int lhost,char* request,int lrequest,int *port)
{
	char *work,*ptr,*ptr2;

	*protocol = *host = *request = 0;
	*port=80;

	work = strdup(url);
//	work = strupr(work);

	/* find protocol if any */
	ptr = strchr(work,':');
	if(ptr!=NULL)
	{
		*(ptr++) = 0;
		strncpy(protocol,work,lprotocol);
	}
	else
	{
		strncpy(protocol,"HTTP",lprotocol);
		ptr = work;
	}

	/* skip past opening /'s */
	if( (*ptr=='/') && (*(ptr+1)=='/') )
		ptr+=2;

	/* find host */
	ptr2 = ptr;
	while( ValidHostChar(*ptr2) && *ptr2 )
		ptr2++;

	*ptr2=0;
	strncpy(host,ptr,lhost);

	/* find the request */
	strncpy(request,url + (ptr2-work),lrequest);

	/* find the port number, if any */
	ptr = strchr(host,':');
	if(ptr!=NULL)
	{
		*ptr=0;
		*port = atoi(ptr+1);
	}

	free(work);
}

BYTEARRAY UTIL_Uncompress( const BYTEARRAY& data )
{
    uLong uncomprLen = UTIL_ByteArrayToUInt32( data, true, 0);

    boost::shared_array<Byte> uncomprBuf( new Byte[uncomprLen] );

    if ( uncompress(uncomprBuf.get(), &uncomprLen, (const Bytef*)&data[4], data.size() - 4 ) != Z_OK )
        return BYTEARRAY();
    else
        return UTIL_CreateByteArray( (unsigned char*)uncomprBuf.get(), uncomprLen );
}

BYTEARRAY UTIL_Compress( const BYTEARRAY& data )
{
    uLong comprLen = data.size();

    boost::shared_array<Byte> uncomprBuf( new Byte[comprLen] );

    if ( compress( uncomprBuf.get(), &comprLen, (const Bytef*)&data[0], data.size() ) != Z_OK )
        return BYTEARRAY();
    else
        return UTIL_CreateByteArray( (unsigned char*)uncomprBuf.get(), comprLen );
}

BYTEARRAY UTIL_Compress( const string& data )
{
    uLong comprLen = data.size();

    boost::shared_array<Byte> uncomprBuf( new Byte[comprLen] );

    if ( compress( uncomprBuf.get(), &comprLen, (const Bytef*)&data[0], data.size() ) != Z_OK )
        return BYTEARRAY();
    else
        return UTIL_CreateByteArray( (unsigned char*)uncomprBuf.get(), comprLen );
}
