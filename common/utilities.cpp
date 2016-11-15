#include "utilities.h"
#include "crypt.h"
#include <boost//archive/iterators/base64_exception.hpp>
#include <boost//archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost//archive/iterators/transform_width.hpp>
#include <boost/interprocess/detail/atomic.hpp>
#include <string>
#include <iostream>
#include <sstream>
#include <boost/version.hpp>

#ifdef WIN32
#include <process.h>
#endif
#include "server_frame.h"

void signal_handle(int n)
{
	switch (n)
	{
	case 3:
		if (g_wait_stop == false)
		{
			g_wait_stop = true;
		}
		
		printf("Get a signal -- SIGQUIT \n");
		break;
	case 9:
		printf("Get a signal -- SIGKILL \n");
		break;
	}
	printf("catch signal %d\n", n);
	return;
}

void save_pid()
{
	remove("pid_temp.pid");
	FILE * fp = fopen("pid_temp.pid", "wt+");
	if (fp != NULL)
	{
		char sz[256];
		memset(sz, 0, sizeof(sz));
		fseek(fp, 0, SEEK_SET);
#ifdef WIN32
		sprintf(sz, "%d\n", _getpid());
#else
		sprintf(sz, "%d\n", getpid());		
#endif
		fwrite(sz, sizeof(char), strlen(sz), fp);
		fclose(fp);
	}
}



bool is_in_current_mon(const time_t * t)
{
	if (t != NULL)
	{
		time_t timep;
		struct tm *p;
		time(&timep);
		p = localtime(&timep); //取得当地时间
		int last_year = p->tm_year;
		int last_mon = p->tm_mon;
		time_t last_day_time = * t;
		if (last_day_time == 0)
		{
			return true;
		}
		struct tm *last_time = localtime(&last_day_time);
		if (last_time->tm_year == last_year && last_time->tm_mon == last_mon)
		{
			return true;
		}		
	}
	return false;
}



bool is_in_today(const time_t * t)
{
	if (t != NULL)
	{
		time_t timep;
		struct tm *p;
		time(&timep);
		p = localtime(&timep); //取得当地时间
		int last_year = p->tm_year;
		int last_day = p->tm_yday;		
		time_t last_day_time = * t;
		if (last_day_time == 0)
		{
			return false;
		}
		struct tm *last_time = localtime(&last_day_time);
		if (last_time->tm_year == last_year && last_time->tm_yday == last_day)
		{
			return true;
		}		
	}
	return false;
}



boost::uint32_t interlocked_increment( volatile boost::uint32_t* target )
{

#if (BOOST_VERSION >= 104800)

	return boost::interprocess::ipcdetail::atomic_inc32( target );

#else

	return boost::interprocess::detail::atomic_inc32( target );

#endif
}



boost::uint32_t interlocked_decrement( volatile boost::uint32_t* target )
{

#if (BOOST_VERSION >= 104800)

	return boost::interprocess::ipcdetail::atomic_dec32( target );

#else

	return boost::interprocess::detail::atomic_dec32( target );

#endif

}



void interlocked_write( volatile boost::uint32_t* target, boost::uint32_t value )
{

#if (BOOST_VERSION >= 104800)

	boost::interprocess::ipcdetail::atomic_write32( target, value );

#else

	boost::interprocess::detail::atomic_write32( target, value );

#endif

}



boost::uint32_t interlocked_read( volatile boost::uint32_t* target )
{

#if (BOOST_VERSION >= 104800)

	return boost::interprocess::ipcdetail::atomic_read32( target );

#else

	return boost::interprocess::detail::atomic_read32( target );

#endif

}



boost::uint32_t interlocked_compare_exchange( volatile boost::uint32_t* target, boost::uint32_t value, boost::uint32_t comp )
{

#if (BOOST_VERSION >= 104800)

	return boost::interprocess::ipcdetail::atomic_cas32( target, value, comp );

#else

	return boost::interprocess::detail::atomic_cas32( target, value, comp );

#endif

}



#ifndef _WIN32

void cpu_wait()
{

	usleep( 1 );

}

#else

void cpu_wait()
{

	Sleep( 1 );

}

#endif



static const char* character_original_table = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

static const char* character_encrypt_table =

	"h2OXAzK4ym3EFwJ5Gk7jCeRS8DcYMIPUQZHlq1oiNa6VgtfrxLvsn0uTdB9pbW"

	"F37VKZcUJdNpgljBnTG1SzWLYOs48vRutkPXDy9iMr0H5mhQexoabI6fAEq2wC"

	"zvFqehQwJB94ArsE51NL8fxbtSPGc0KOkIYylU67DodH2MCnVpgRWiaZjTXmu3"

	"X3RYHziS4JQ6ogLsNIpjPK5xdUlV27Otuc91CaDbveyZ0TB8WfFAwnhEGmMrkq"

	"zQ9gtdpAFlbI7OxnkrG06VsJhYcTXuWZD84Bq5R1fjeS2HLvmKywEoCi3MNaPU"

	"uHPt12gU3dKf8WmpERYhrCjInA6aLFN9JyMl0ZQvqbkTzoB75es4wxDXiGVOSc"

	"iqbYtXWrlp65RcS0ga3nyUZovHjzE1efBDI9TCL27NK4VwGdmMhskPuJxQFA8O"

	"ESmgflDu19qUjZLXnipzG5yakCMVoIr6QcRHvhWt8OxBdYA70bTsP243FeKJNw"

	"D6XdKbc8mxvYuzGAeWtfh2iE45PUnBoLg3HCRJjT9qwksFSVZpQ1MI7yOrN0al"

	"gw4Hi13ILuZtnm05pFoTz9MBy8RCfVJNjb2SOqEcUrPxvGA7eYldDWXQK6hska"

	"K4BgHUhPQJYt1Lrz97DCumwfNcp8WVSyMGeX3dAZixbTOknlF5v6a0sIjEqRo2"

	"PUpGbXWA6Nnm4KYu9t1T5zhcVqQiDjdEHLJ0OvZxwgsoBkIrl823yMaCeFR7Sf"

	"iBqCt1kzfayc2oervTPExw9ZhIJpuXY63MNKLQRnmVsH0b5W7jDGFUl8AdSgO4"

	"4OLnCqdZbyfExmX6a0Hvtrp9A8TGkzQWRPoJMViK1I2ghNlSDeuB7FjwcUs5Y3"

	"BIYrX96qVx3fwi5A2TlN4ZKDnERCt1byk0jJs8GWL7SchgpOFHPQvmoeMduazU"

	"3vVphnyJ4wQ0tO5R92YuAzbDHlC8fogqKG6SicBjmZaWTUMP7xeELsNk1XIFrd"

	"drOVYJg6AeznivK8BtW1pqGuTFXaIkbhUxLoRy43PZ9lC5wQcDNHS2sEMf7mj0"

	"XhPS2QwyUZiHWG8evLsblxFcjJrdROAmNq7Vk091DKzT5gEoI6tp4BCnYMua3f"

	"UvgNmKqMTF4WzAcaj6OP15rHGJV7S3ln0IZBYCeL8pRsDEyQk2fbhiXxudto9w"

	"8y7u3tJDYRxedKmH0WVbL9qlpGUrv2CTjQocInAZihXOB4SawN6P1fM5FgzkEs"

	"y6vpTgfkwLeBY28hlDjEqnKsVHbtuoQrMImX79xAZSa0JiFUOcR3GNd14PCW5z"

	"c4ABsWNdYEF9lODpgfzQxkPo3mUHeia0XrtvnJ7MTqj5G8uS6bVwCI2yZLK1hR"

	"Dz1rL3UgKxkvq7Qp289XbNVTcHlfWdYIBsRjnG6CEiSFoJtA5yaeZuPhO0m4Mw"

	"gFAOLYpSmKNQ0BJ26l8ykTubwUnMZXCIH1fDWqaidzE593VxjtPRos4rGh7vec"

	"NrzAmxfBWq6Dy3HJhnL7ubRT91MOtpcoPG2lgFCEwiIVaYveksSU085K4jZdXQ"

	"bmf6kl5CyE9r2uToL0Bzd3Vtqn1ZhpgUMXaKPSARHciOwQG78YWIvJsDj4xFNe"

	"4Qwrbuftad9OZ07RWTenjhA3BIcK2qEv8SJFLXNG15PiV6zgykmlxoDHpUYMCs"

	"YmoAFP3Dw0ZRcM724xJ9gzp8GjiHlXOkKEtNfqnUSr6bLsQ5u1ICBTVyhvdeaW"

	"6fw3KniqIx1PRtOF0U2CvbJTQehzMXcDZVyau5mGpsEdW9l7BHSNor8kLYA4gj"

	"uCITPp0DnbQrUlgRhS47X3dB1KcEeGMHyWANfiV2OmYv6txLFwkz8qa5osjZ9J"

	"N8mrGqe7JSgUE1ZVkozKDMjPQwTu92BtOFf0nbaxAcXH5siLdvyYC6hl3IWR4p"

	"r1XdREYSgWoThC4MjuJI3bUlfLmak2VNvc5sz9Z0tpBG8nwQFKH7eAPqDxi6Oy"

	"kCmIuVyoHrxgdbAZiS1tGpqJTK4zhWsULnvB7l3Mj6YQcPNE85ORwD02FefaX9"

	"n7OqFVPEW5KGctbUXk8Cy4YazI1Zwu9dpsfojJBS0g3HQMvelrmAT2Lx6iDNRh"

	"2PDoG6kazepcRAVgxSFvBWNLym97KidrXnYZ851Oj3M4CUEslQtHuwqbhTIfJ0"

	"jClKN4n71FTX9peHDbtfUQ82gSO5rqYwA36V0PZdzWJhLvRyaomEkGBiIMuxsc"

	"n3u0aSHWAeXTtvl2YfKr7BhzwF15qUQ4GIM8PbNJsRigdjZyOD6x9EmVcLpkCo"

	"zra0yAvDm4jB2x3pNKqR1f8C6VEwSktgs5LluFdMoZbhPI7GUcHWnX9TYieJOQ"

	"4uM6o2XkTwqFC5ZlYe8nzmivH9OdWLDxIRfJQar0NPpcSUy3gEAV7KthsjB1bG"

	"VNAzwyiGP52SImYsBnpQvtFcJCu8krod3X614TEZK0qfaexLHbR7UjM9lDOghW"

	"AqMXFevwOZygIYRNiD70khdc6JLUB5tCxb9EPr4Glzo1Q8f2s3uKTaVWnSHmpj"

	"Y1ZdOPeTg2jnbR6LH0qXupBflFwMsGErxoiaC3t4mkv7DQ5hUSJzI9VAN8yWcK"

	"DvHxq0oS4w5lb9paudR3YktPCFGOKzBgr1TMcf8VhWLAsy2iZjU6enJX7mNIEQ"

	"dHVUMsK1wB98Pbf0jJyDtaClvcR6xG7kITpm2EZoLXz3igSWneQNqrYOAFu5h4"

	"DzFbu0de6LAJ5vMBNRYQT7awjfCrKqiyZH8omIE9UPlShn4OWpcG2st1XxgV3k"

	"3BGrh7onLzRjQeAf1Tl9ZODEWxCYkbJUuP4qNIyaF5Vw6dH2pX0vtg8micMKSs"

	"NBxXuRI1w4kt8Jq6pOzml9hACFYbo2nevZaL7GiQfSsyDHPTcKr3UjWMdgV5E0"

	"4xXDbLem7Soa3Mf5r0NuyVWZpAIc2PUhC9Et68Rws1YFTkKJlvHzigjdBGQOqn"

	"uPHI8hpqQjGtwM5kabLR1OZdz6XFTcfY0vgm39CUDesrKN7JVE4S2noiWAlyBx"

	"OwgVpx290UmcsRyl8LHj3b4PqChGY6vKnST5iNfJADB7tdauXzQIrZokeWFM1E";



struct encrypt_table

{

	enum { TABLE_COUNT = 50, TABLE_SIZE = 26 * 2 + 10 };

	encrypt_table()

	{

		for( int i = 0; i < TABLE_COUNT; ++i )

			memcpy( &tables[i].data, &character_encrypt_table[i * TABLE_SIZE], TABLE_SIZE );

	}

	struct tb {

		char data[TABLE_SIZE];

		const char encrypt_get( char a ) const

		{

			const char* p = std::find( &character_original_table[0], &character_original_table[TABLE_SIZE], a );

			if( p == &character_original_table[TABLE_SIZE] )

				return 0;

			else

				return data[p - &character_original_table[0]];

		}

		const char decrypt_get( char a ) const

		{

			const char* p = std::find( &data[0], &data[TABLE_SIZE], a );

			if( p == &data[TABLE_SIZE] )

				return 0;

			else

				return character_original_table[p - &data[0]];

		}

	};

	tb tables[TABLE_COUNT];

} s_et;



unsigned int generate_table_key()
{
	return rand() % encrypt_table::TABLE_COUNT;
}



void encrypt_string( const std::string& in, std::string& out, unsigned int table )
{

	if( table >= encrypt_table::TABLE_COUNT )
	{
		return;
	}

	size_t size = in.length();
	out.clear();
	const char* p = in.c_str();
	const encrypt_table::tb& t = s_et.tables[table];
	for( size_t i = 0; i < size; ++i )
	{
		char c = t.encrypt_get( p[i] );

		if( !c )
		{

			out.clear();
			break;
		}
		out += c;
	}

}



void decrypt_string( const std::string& in, std::string& out, unsigned int table )
{

	if( table >= encrypt_table::TABLE_COUNT )
	{
		return;
	}

	size_t size = in.length();
	out.clear();
	const char* p = in.c_str();
	const encrypt_table::tb& t = s_et.tables[table];
	for( size_t i = 0; i < size; ++i )
	{
		char c = t.decrypt_get( p[i] );
		if( !c )
		{
			out.clear();
			break;
		}
		out += c;
	}
}





#ifdef _WIN32

static double s_freq = 0.0;

static int s_inited = 0;

void init_tick_count()

{

	LARGE_INTEGER lv;

	QueryPerformanceFrequency( &lv );

	s_freq = lv.QuadPart;

}



unsigned int get_tick_count()

{

	if( s_inited == 0 )

	{

		init_tick_count();

		s_inited = 1;

	}



	LARGE_INTEGER lv;

	QueryPerformanceCounter( &lv );

	double timeElapsedTotal = lv.QuadPart / s_freq * 1000.0;

	return (unsigned int)timeElapsedTotal;

}

#else

#include <sys/time.h>

unsigned int get_tick_count()

{

	struct timeval tp;

	gettimeofday(&tp, NULL);

	return (tp.tv_sec * 1000) + (tp.tv_usec / 1000);

}



void strreverse(char* begin, char* end)

{

	char aux;

	while(end>begin)

		aux=*end, *end--=*begin, *begin++=aux;

}



const char* itoa( int value, char* str, int base )

{

	static char num[] = "0123456789abcdefghijklmnopqrstuvwxyz";

	char* wstr=str;

	int sign;

	div_t res;

	// Validate base

	if (base<2 || base>35){ *wstr='\0'; return str; }



	// Take care of sign

	if ((sign=value) < 0) value = -value;

	// Conversion. Number is reversed.

	do {

		res = div(value,base);

		*wstr++ = num[res.rem];

	}while(value=res.quot);

	if(sign<0) *wstr++='-';

	*wstr='\0';

	// Reverse string

	strreverse(str,wstr-1);

	return str;

}

#endif





function_spend_time_monitor::function_spend_time_monitor( const char* fn_name ) : m_name( fn_name )

{

	m_begin = get_tick_count();

}



function_spend_time_monitor::~function_spend_time_monitor()
{

	m_end = get_tick_count();



	printf( "function:[%s] spend time:[%u]ms\n", m_name.c_str(), m_end - m_begin );

}



void convert_unix_time( unsigned int t, int* outyear, int* outmonth, int* outday, int* outhour, int* outminute, int* outsecond )
{

	time_t _t = t;

	tm* p = localtime( &_t );

	*outyear = p->tm_year + 1900;

	*outmonth = p->tm_mon + 1;

	*outday = p->tm_mday;

	*outhour = p->tm_hour;

	*outminute = p->tm_min;

	*outsecond = p->tm_sec;

}



void build_unix_time_to_string( unsigned int t, std::string& out )
{
	char tmp[64];
	int y,m,d,h,min,s;
	convert_unix_time( t, &y, &m, &d, &h, &min, &s );
	sprintf( tmp, "%d-%d-%d %d:%d:%d", y, m, d, h, min, s );
	out = tmp;
}



unsigned int make_unix_time( int year, int month, int day, int hour, int minute, int second )
{

	struct tm stm;

	memset( &stm, 0, sizeof( stm ) );

	stm.tm_year = year - 1900;

	stm.tm_mon = month - 1;

	stm.tm_mday = day;

	stm.tm_hour = hour;

	stm.tm_min = minute;

	stm.tm_sec = second;

	return (unsigned int)mktime( &stm );

}



bool same_day(unsigned int t1, unsigned int t2)
{
	time_t _t1 = t1;
	tm* p1 = localtime(&_t1);
	int y1 = p1->tm_year;
	int d1 = p1->tm_yday;

	time_t _t2 = t2;
	tm* p2 = localtime(&_t2);
	int y2 = p2->tm_year;
	int d2 = p2->tm_yday;
	return (y1 == y2 && d1 == d2);
}


bool same_month( unsigned int t1, unsigned int t2 )
{
	time_t _t1 = t1;
	tm* p1 = localtime( &_t1 );
	int y1 = p1->tm_year;
	int m1 = p1->tm_mon;

	time_t _t2 = t2;
	tm* p2 = localtime( &_t2 );
	int y2 = p2->tm_year;
	int m2 = p2->tm_mon;

	return (y1 == y2 && m1 == m2);
}



bool same_week( unsigned int t1, unsigned int t2 )

{

	time_t _t1 = t1;

	tm* p1 = localtime( &_t1 );

	int y1 = p1->tm_year;

	int m1 = p1->tm_mon;

	int md1 = p1->tm_mday;

	int wd1 = p1->tm_wday;



	time_t _t2 = t2;

	tm* p2 = localtime( &_t2 );

	int y2 = p2->tm_year;

	int m2 = p2->tm_mon;

	int md2 = p2->tm_mday;

	int wd2 = p2->tm_wday;



	if( y1 == y2 && m1 == m2 )

		if( wd1 + ( md2 - md1 ) < 7 )

			return true;

	return false;

}



bool is_dayofweek( unsigned int t, unsigned char dow )
{

	time_t _t = t;

	tm* p = localtime( &_t );

	return p->tm_wday == dow;

}



bool in_duration( unsigned int t, unsigned char hstart, unsigned char hend )
{

	time_t _t = t;

	tm* p = localtime( &_t );

	return ( p->tm_hour >= hstart && p->tm_hour < hend );

}

int SplitStringA(const std::string& strIn, const std::string& strDelimiter, std::vector<std::string>& ret)
{
	ret.clear();

	int iPos = 0;
	int newPos = -1;
	int delimiterLength = strDelimiter.size();
	int strInLength = strIn.size();

	if (delimiterLength == 0 || strInLength == 0)
		return 0;

	std::vector<int> positions;

	newPos = strIn.find(strDelimiter, 0);

	if (newPos < 0)
	{
		ret.push_back(strIn);
		return 1;
	}

	int numFound = 0;

	while (newPos >= iPos)
	{
		numFound++;
		positions.push_back(newPos);
		iPos = newPos;
		newPos = strIn.find(strDelimiter, iPos + delimiterLength);
	}

	for (size_t i = 0; i <= positions.size(); ++i)
	{
		std::string s("");
		if (i == 0)
		{
			s = strIn.substr(i, positions[i]);
		}
		else
		{
			int offset = positions[i - 1] + delimiterLength;
			if (offset < strInLength)
			{
				if (i == positions.size())
				{
					s = strIn.substr(offset);
				}
				else
				{
					s = strIn.substr(offset, positions[i] - positions[i - 1] - delimiterLength);
				}
			}
		}

		if (s.size() > 0)
		{
			ret.push_back(s);
		}
	}
	return numFound;
}

bool isIntger(std::string str)
{
	std::stringstream sin(str);
	int number;
	if (!(sin >> number))
		return false;
	return true;
}

bool is_valid_string( const std::string& str )
{
	int len = str.length();
	const unsigned char* p = (const unsigned char*)str.c_str();
	const unsigned char* porig = p;
	while( p < porig + len )
	{
		if( *p > 0x80 )
		{
			p += 2;
			continue;
		}
		else if( *p >= 'a' && *p <= 'z' )
		{
			++p;
			continue;
		}
		else if( *p >= 'A' && *p <= 'Z' )
		{
			++p;
			continue;
		}
		else if( *p >= '0' && *p <= '9' )
		{
			++p;
			continue;
		}
		return false;
	}
	return true;
}



char* get_and_create_static_buffer( std::size_t size )

{

	static char* s_ptr = NULL;

	if( !s_ptr )

		s_ptr = (char*)malloc( size );



	return s_ptr;

}


std::string get_time(time_t cur_time)
{
	time_t timep;
	if (cur_time == 0)
	{
		time(&timep); /*获取time_t类型的当前时间*/
	}
	else
	{
		timep = cur_time;
	}


	struct tm* cur = localtime(&timep);
	char sz_time[256];
	sprintf(sz_time, "%d-%d-%d %d:%d:%d", cur->tm_year + 1900, cur->tm_mon + 1, cur->tm_mday + 1, cur->tm_hour, cur->tm_min, cur->tm_sec);
	return std::string(sz_time);

}

data_stream_verifier::data_stream_verifier()
{
	reset();
}

void data_stream_verifier::reset()
{
	send_index = 0;
	recv_index = 0;
}

unsigned int data_stream_verifier::make_checksum( const void* data, unsigned int length )
{
	unsigned int checksum = ssl::sslCrc32( (const char*)data, length );
	//printf( "send_index = %d\n", send_index );
	//return checksum ^ get_magic_number( send_index++ );
	return checksum;
}

bool data_stream_verifier::verify_data_stream( const void* data, unsigned int length, unsigned int checksum )
{
	unsigned int tmp = ssl::sslCrc32((const char*)data, length );
	//printf( "recv_index = %d\n", recv_index );
	//return checksum == ( tmp ^ get_magic_number( recv_index++ ) );
	return checksum == tmp;
}

unsigned int data_stream_verifier::get_magic_number( unsigned int key )
{
	static const char* char_table = "2PDoG6kazepcRAVgxSFvBWNLym97KidrXnYZ851Oj3M4CUEslQtHuwqbhTIfJ0"
		"jClKN4n71FTX9peHDbtfUQ82gSO5rqYwA36V0PZdzWJhLvRyaomEkGBiIMuxsc"
		"n3u0aSHWAeXTtvl2YfKr7BhzwF15qUQ4GIM8PbNJsRigdjZyOD6x9EmVcLpkCo"
		"zra0yAvDm4jB2x3pNKqR1f8C6VEwSktgs5LluFdMoZbhPI7GUcHWnX9TYieJOQ"
		"4uM6o2XkTwqFC5ZlYe8nzmivH9OdWLDxIRfJQar0NPpcSUy3gEAV7KthsjB1bG";

	static const int char_table_size = 5*(26*2+10);
	return *(unsigned int*)(&char_table[key % (char_table_size - 4)]);
}


void urlencode(unsigned char * src, unsigned char * dest)
{
	assert(src);
	assert(dest);
	unsigned char ch;
	int len = 0;

	//while (len < (dest_len - 4) && *src) 
	while (*src)
	{
		ch = (unsigned char)*src;
		if (*src == ' ')
		{
			*dest++ = '+';
		}
		else if (is_alpha_number_char(ch) || strchr("-_.!~*'()", ch))
		{
			*dest++ = *src;
		}
		else
		{
			*dest++ = '%';
			*dest++ = char_to_hex((unsigned char)(ch >> 4));
			*dest++ = char_to_hex((unsigned char)(ch % 16));
		}
		++src;
		++len;
	}
	*dest = 0;
	return;
}
char* urldecode(char* encd, char* decd)
{
	assert(encd);
	assert(decd);
	int i, j;
	char *cd = encd;
	char p[2];
	unsigned int num;
	j = 0;

	for (i = 0; i < strlen(cd); i++)
	{
		memset(p, '\0', 2);
		if (cd[i] != '%')
		{
			decd[j++] = cd[i];
			continue;
		}

		p[0] = cd[++i];
		p[1] = cd[++i];

		p[0] = p[0] - 48 - ((p[0] >= 'A') ? 7 : 0) - ((p[0] >= 'a') ? 32 : 0);
		p[1] = p[1] - 48 - ((p[1] >= 'A') ? 7 : 0) - ((p[1] >= 'a') ? 32 : 0);
		decd[j++] = (char)(p[0] * 16 + p[1]);

	}
	decd[j] = '\0';

	return decd;
}
/*
bool base64_encode(std::string &input, std::string &output)
{
	typedef boost::archive::iterators::base64_from_binary<boost::archive::iterators::transform_width<std::string::const_iterator, 6, 8> >Base64EncodeIterator;
	std::stringstream result;
	copy(Base64EncodeIterator(input.begin()), Base64EncodeIterator(input.end()), std::ostream_iterator<char>(result));
	size_t equal_count = (3 - input.length() % 3) % 3;
	for (size_t i = 0; i < equal_count; i++)
	{
		result.put('=');
	}
	output = result.str();
	return output.empty() == false;
}

bool Base64_decode(const std::string& input, std::string& output)
{
	typedef boost::archive::iterators::transform_width<boost::archive::iterators::binary_from_base64<std::string::const_iterator>, 8, 6> Base64DecodeIterator;
	std::stringstream result;
	try
	{
		copy(Base64DecodeIterator(input.begin()), Base64DecodeIterator(input.end()), std::ostream_iterator<char>(result));
	}
	catch (...)
	{
		return false;
	}
	output = result.str();
	return output.empty() == false;
}

*/

static inline bool is_base64(unsigned char c) {
	return (isalnum(c) || (c == '+') || (c == '/'));
}


const char * base64char = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
char * base64_encode(const unsigned char * bindata, char * base64, int binlength)
{
	int i, j;
	unsigned char current;

	for (i = 0, j = 0; i < binlength; i += 3)
	{
		current = (bindata[i] >> 2);
		current &= (unsigned char)0x3F;
		base64[j++] = base64char[(int)current];

		current = ((unsigned char)(bindata[i] << 4)) & ((unsigned char)0x30);
		if (i + 1 >= binlength)
		{
			base64[j++] = base64char[(int)current];
			base64[j++] = '=';
			base64[j++] = '=';
			break;
		}
		current |= ((unsigned char)(bindata[i + 1] >> 4)) & ((unsigned char)0x0F);
		base64[j++] = base64char[(int)current];

		current = ((unsigned char)(bindata[i + 1] << 2)) & ((unsigned char)0x3C);
		if (i + 2 >= binlength)
		{
			base64[j++] = base64char[(int)current];
			base64[j++] = '=';
			break;
		}
		current |= ((unsigned char)(bindata[i + 2] >> 6)) & ((unsigned char)0x03);
		base64[j++] = base64char[(int)current];

		current = ((unsigned char)bindata[i + 2]) & ((unsigned char)0x3F);
		base64[j++] = base64char[(int)current];
	}
	base64[j] = '\0';
	return base64;
}

int base64_decode(const char * base64, unsigned char * bindata)
{
	int i, j;
	unsigned char k;
	unsigned char temp[4];
	for (i = 0, j = 0; base64[i] != '\0'; i += 4)
	{
		memset(temp, 0xFF, sizeof(temp));
		for (k = 0; k < 64; k++)
		{
			if (base64char[k] == base64[i])
				temp[0] = k;
		}
		for (k = 0; k < 64; k++)
		{
			if (base64char[k] == base64[i + 1])
				temp[1] = k;
		}
		for (k = 0; k < 64; k++)
		{
			if (base64char[k] == base64[i + 2])
				temp[2] = k;
		}
		for (k = 0; k < 64; k++)
		{
			if (base64char[k] == base64[i + 3])
				temp[3] = k;
		}

		bindata[j++] = ((unsigned char)(((unsigned char)(temp[0] << 2)) & 0xFC)) |
			((unsigned char)((unsigned char)(temp[1] >> 4) & 0x03));
		if (base64[i + 2] == '=')
			break;

		bindata[j++] = ((unsigned char)(((unsigned char)(temp[1] << 4)) & 0xF0)) |
			((unsigned char)((unsigned char)(temp[2] >> 2) & 0x0F));
		if (base64[i + 3] == '=')
			break;

		bindata[j++] = ((unsigned char)(((unsigned char)(temp[2] << 6)) & 0xF0)) |
			((unsigned char)(temp[3] & 0x3F));
	}
	return j;
}




//std::string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len) {
//	std::string ret;
//	int i = 0;
//	int j = 0;
//	unsigned char char_array_3[3];
//	unsigned char char_array_4[4];
//
//	while (in_len--) {
//		char_array_3[i++] = *(bytes_to_encode++);
//		if (i == 3) {
//			char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
//			char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
//			char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
//			char_array_4[3] = char_array_3[2] & 0x3f;
//
//			for (i = 0; (i < 4); i++)
//				ret += base64_chars[char_array_4[i]];
//			i = 0;
//		}
//	}
//
//	if (i)
//	{
//		for (j = i; j < 3; j++)
//			char_array_3[j] = '\0';
//
//		char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
//		char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
//		char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
//		char_array_4[3] = char_array_3[2] & 0x3f;
//
//		for (j = 0; (j < i + 1); j++)
//			ret += base64_chars[char_array_4[j]];
//
//		while ((i++ < 3))
//			ret += '=';
//
//	}
//
//	return ret;
//
//}
//
//std::string base64_decode(std::string const& encoded_string) {
//	int in_len = encoded_string.size();
//	int i = 0;
//	int j = 0;
//	int in_ = 0;
//	unsigned char char_array_4[4], char_array_3[3];
//	std::string ret;
//
//	while (in_len-- && (encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
//		char_array_4[i++] = encoded_string[in_]; in_++;
//		if (i == 4) {
//			for (i = 0; i < 4; i++)
//				char_array_4[i] = base64_chars.find(char_array_4[i]);
//
//			char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
//			char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
//			char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];
//
//			for (i = 0; (i < 3); i++)
//				ret += char_array_3[i];
//			i = 0;
//		}
//	}
//
//	if (i) {
//		for (j = i; j < 4; j++)
//			char_array_4[j] = 0;
//		for (j = 0; j < 4; j++)
//			char_array_4[j] = base64_chars.find(char_array_4[j]);
//		char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
//		char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
//		char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];
//		for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
//	}
//
//	return ret;
//}
//base64编码 
int Base64Encode(unsigned char *out, const unsigned char *in, int inlen)
{
	const char base64digits[] =
		"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	int outlen = 0;
	for (; inlen >= 3; inlen -= 3)
	{
		*out++ = base64digits[in[0] >> 2];
		*out++ = base64digits[((in[0] << 4) & 0x30) | (in[1] >> 4)];
		*out++ = base64digits[((in[1] << 2) & 0x3c) | (in[2] >> 6)];
		*out++ = base64digits[in[2] & 0x3f];
		in += 3;
		outlen += 4;
	}
	if (inlen > 0)
	{
		unsigned char fragment;

		*out++ = base64digits[in[0] >> 2];
		fragment = (in[0] << 4) & 0x30;
		if (inlen > 1)
			fragment |= in[1] >> 4;
		*out++ = base64digits[fragment];
		*out++ = (inlen < 2) ? '=' : base64digits[(in[1] << 2) & 0x3c];
		*out++ = '=';
		outlen += 4;
	}
	*out = '\0';

	return outlen;
}

unsigned char LMoveBit(int base, int MoveNum)
{
	unsigned char result = base;
	if (MoveNum == 0)return 1;
	if (MoveNum == 1)return MoveNum;
	result = base << (MoveNum - 1);
	return result;
}
int Base64Decode(const char *base64code, long base64length, unsigned char* outbin)
{
	char base64_alphabet[] =
	{ 'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P',
		'Q','R','S','T','U','V','W','X','Y','Z','a','b','c','d','e','f',
		'g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v',
		'w','x','y','z','0','1','2','3','4','5','6','7','8','9','+','/','=' };
	char buf[4];
	int i, j;
	int k;
	int l = 0;
	unsigned char temp1[4], temp2;
	unsigned char *Buffer = outbin;
	int outlen = 0;
	memset(Buffer, 0, base64length * 3 / 4);
	unsigned long base64a = (base64length / 4) - 1;
	unsigned long base64b = 0;
	int m_padnum = 0;
	for (; base64b<base64a + 1; base64b++)
	{
		for (i = 0; i<4; i++)
		{
			buf[i] = *(base64code + (base64b * 4) + i);
			for (j = 0; j<65; j++)
			{
				if (buf[i] == base64_alphabet[j])
				{
					temp1[i] = j;
					break;
				}
			}
		}
		i--;
		for (k = 1; k<4; k++)
		{
			if (temp1[i - (k - 1)] == 64) { m_padnum++; continue; }
			temp1[i - (k - 1)] = temp1[i - (k - 1)] / LMoveBit(2, (k - 1) * 2);
			temp2 = temp1[i - k];
			temp2 = temp2&(LMoveBit(2, k * 2) - 1);
			temp2 *= LMoveBit(2, 8 - (2 * k));//move 4 
			temp1[i - (k - 1)] = temp1[i - (k - 1)] + temp2;
			Buffer[base64b * 3 + (3 - k)] = temp1[i - (k - 1)];
			outlen++;
		}
	}
	return outlen;
}