/*
  ATLAN C++ Library: MD5.hpp
  
  ATLAN C++ Library is a library for personal studying.
  No licence is annouced.
  You can use or change any part of it.
  When you using it, inform the author is hoped but not required.
  
  Author: Copyright (C) 1991-2, RSA Data Security, Inc.
  Modified by: atlantise522@gmail.com
*/
/////////////////////////////////////////////////////////////////////////
// Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All
// rights reserved.
//
// License to copy and use this software is granted provided that it
// is identified as the "RSA Data Security, Inc. MD5 Message-Digest
// Algorithm" in all material mentioning or referencing this software
// or this function.
// License is also granted to make and use derivative works provided
// that such works are identified as "derived from the RSA Data
// Security, Inc. MD5 Message-Digest Algorithm" in all material
// mentioning or referencing the derived work.
// RSA Data Security, Inc. makes no representations concerning either
// the merchantability of this software or the suitability of this
// software for any particular purpose. It is provided "as is"
// without express or implied warranty of any kind.
// These notices must be retained in any copies of any part of this
// documentation and/or software.
/////////////////////////////////////////////////////////////////////////
#ifndef __ACL__MD5_HPP__
#define __ACL__MD5_HPP__
#include <assert.h>
#include <memory.h>
#include <stdio.h>
#include <string.h>

namespace acl{

class MD5{
public:
	MD5() { Init(); }
	
	// MD5String: Performs the MD5 algorithm on a char* string
	char* MD5String(const char* szString){
		int nLen = strlen(szString);

		this->Update((unsigned char*)szString, (unsigned int)nLen);
		this->Finalize();

		return PrintMD5(this->Digest());
	}
	
	// MD5File: Performs the MD5 algorithm on a file (binar or text)
	char* MD5File(const char* szFilename){
		FILE* file;
		int nLen;
		unsigned char chBuffer[1024];

		try{
			memset(chBuffer, 0, 1024);

			if ((file = fopen (szFilename, "rb")) != NULL){
				while (1){
					nLen = fread (chBuffer, 1, 1024, file);
					if(nLen < 1) break;
					this->Update(chBuffer, nLen);
				}
				this->Finalize();
				fclose (file);
				return PrintMD5(this->Digest());
			}
		}catch(...){}

		return NULL; // failed
	}

private:
	void Init(){
		memset(m_Count, 0, 2 * sizeof(unsigned int));

		m_State[0] = 0x67452301;
		m_State[1] = 0xefcdab89;
		m_State[2] = 0x98badcfe;
		m_State[3] = 0x10325476;
		
		m_Padding[0] = 0x80;
		for(int i=1; i<64; i++){
			m_Padding[i] = 0;
		}
		
		S11 = 7;
		S12 = 12;
		S13 = 17;
		S14 = 22;
		S21=  5;
		S22=  9;
		S23 = 14;
		S24 = 20;
		S31=  4;
		S32 = 11;
		S33 = 16;
		S34 = 23;
		S41=  6;
		S42 = 10;
		S43 = 15;
		S44 = 21;
	}
	
	void Update(unsigned char* chInput, unsigned int nInputLen){
		unsigned int i, index, partLen;

		// Compute number of bytes mod 64
		index = (unsigned int)((m_Count[0] >> 3) & 0x3F);

		// Update number of bits
		if ((m_Count[0] += (nInputLen << 3)) < (nInputLen << 3))
			m_Count[1]++;

		m_Count[1] += (nInputLen >> 29);

		partLen = 64 - index;

		// Transform as many times as possible.
		if (nInputLen >= partLen){
			memcpy( &m_Buffer[index], chInput, partLen );
			Transform(m_Buffer);

			for (i = partLen; i + 63 < nInputLen; i += 64)
				Transform(&chInput[i]);

			index = 0;
		}
		else i = 0;

	  // Buffer remaining input
	  memcpy( &m_Buffer[index], &chInput[i], nInputLen-i );
	}
	
	void Finalize(){
		unsigned char bits[8];
		unsigned int index, padLen;

		// Save number of bits
		Encode (bits, m_Count, 8);

		// Pad out to 56 mod 64
		index = (unsigned int)((m_Count[0] >> 3) & 0x3f);
		padLen = (index < 56) ? (56 - index) : (120 - index);
		Update(m_Padding, padLen);

		// Append length (before padding)
		Update (bits, 8);

		// Store state in digest
		Encode (m_Digest, m_State, 16);

		memset(m_Count, 0, 2 * sizeof(unsigned int));
		memset(m_State, 0, 4 * sizeof(unsigned int));
		memset(m_Buffer,0, 64 * sizeof(unsigned char));
	}
	
	unsigned char*	Digest() { return m_Digest; }
	
	// PrintMD5: Converts a completed md5 digest into a char* string.
	char* PrintMD5(unsigned char md5Digest[16]){
		char chBuffer[256];
		char chEach[10];
		int nCount;

		memset(chBuffer,0,256);
		memset(chEach, 0, 10);

		for (nCount = 0; nCount < 16; nCount++){
			sprintf(chEach, "%02x", md5Digest[nCount]);
			strncat(chBuffer, chEach, sizeof(chEach));
		}

		return strdup(chBuffer);
	}
	
	void Transform (unsigned char* block){
	  unsigned int a = m_State[0], b = m_State[1], c = m_State[2], d = m_State[3], x[16];

	  Decode (x, block, 64);

	  // Round 1
	  FF (a, b, c, d, x[ 0], S11, 0xd76aa478);
	  FF (d, a, b, c, x[ 1], S12, 0xe8c7b756);
	  FF (c, d, a, b, x[ 2], S13, 0x242070db);
	  FF (b, c, d, a, x[ 3], S14, 0xc1bdceee);
	  FF (a, b, c, d, x[ 4], S11, 0xf57c0faf);
	  FF (d, a, b, c, x[ 5], S12, 0x4787c62a);
	  FF (c, d, a, b, x[ 6], S13, 0xa8304613);
	  FF (b, c, d, a, x[ 7], S14, 0xfd469501);
	  FF (a, b, c, d, x[ 8], S11, 0x698098d8);
	  FF (d, a, b, c, x[ 9], S12, 0x8b44f7af);
	  FF (c, d, a, b, x[10], S13, 0xffff5bb1);
	  FF (b, c, d, a, x[11], S14, 0x895cd7be);
	  FF (a, b, c, d, x[12], S11, 0x6b901122);
	  FF (d, a, b, c, x[13], S12, 0xfd987193);
	  FF (c, d, a, b, x[14], S13, 0xa679438e);
	  FF (b, c, d, a, x[15], S14, 0x49b40821);

	 // Round 2
	  GG (a, b, c, d, x[ 1], S21, 0xf61e2562);
	  GG (d, a, b, c, x[ 6], S22, 0xc040b340);
	  GG (c, d, a, b, x[11], S23, 0x265e5a51);
	  GG (b, c, d, a, x[ 0], S24, 0xe9b6c7aa);
	  GG (a, b, c, d, x[ 5], S21, 0xd62f105d);
	  GG (d, a, b, c, x[10], S22,  0x2441453);
	  GG (c, d, a, b, x[15], S23, 0xd8a1e681);
	  GG (b, c, d, a, x[ 4], S24, 0xe7d3fbc8);
	  GG (a, b, c, d, x[ 9], S21, 0x21e1cde6);
	  GG (d, a, b, c, x[14], S22, 0xc33707d6);
	  GG (c, d, a, b, x[ 3], S23, 0xf4d50d87);
	  GG (b, c, d, a, x[ 8], S24, 0x455a14ed);
	  GG (a, b, c, d, x[13], S21, 0xa9e3e905);
	  GG (d, a, b, c, x[ 2], S22, 0xfcefa3f8);
	  GG (c, d, a, b, x[ 7], S23, 0x676f02d9);
	  GG (b, c, d, a, x[12], S24, 0x8d2a4c8a);

	  // Round 3
	  HH (a, b, c, d, x[ 5], S31, 0xfffa3942);
	  HH (d, a, b, c, x[ 8], S32, 0x8771f681);
	  HH (c, d, a, b, x[11], S33, 0x6d9d6122);
	  HH (b, c, d, a, x[14], S34, 0xfde5380c);
	  HH (a, b, c, d, x[ 1], S31, 0xa4beea44);
	  HH (d, a, b, c, x[ 4], S32, 0x4bdecfa9);
	  HH (c, d, a, b, x[ 7], S33, 0xf6bb4b60);
	  HH (b, c, d, a, x[10], S34, 0xbebfbc70);
	  HH (a, b, c, d, x[13], S31, 0x289b7ec6);
	  HH (d, a, b, c, x[ 0], S32, 0xeaa127fa);
	  HH (c, d, a, b, x[ 3], S33, 0xd4ef3085);
	  HH (b, c, d, a, x[ 6], S34,  0x4881d05);
	  HH (a, b, c, d, x[ 9], S31, 0xd9d4d039);
	  HH (d, a, b, c, x[12], S32, 0xe6db99e5);
	  HH (c, d, a, b, x[15], S33, 0x1fa27cf8);
	  HH (b, c, d, a, x[ 2], S34, 0xc4ac5665);

	  // Round 4
	  II (a, b, c, d, x[ 0], S41, 0xf4292244);
	  II (d, a, b, c, x[ 7], S42, 0x432aff97);
	  II (c, d, a, b, x[14], S43, 0xab9423a7);
	  II (b, c, d, a, x[ 5], S44, 0xfc93a039);
	  II (a, b, c, d, x[12], S41, 0x655b59c3);
	  II (d, a, b, c, x[ 3], S42, 0x8f0ccc92);
	  II (c, d, a, b, x[10], S43, 0xffeff47d);
	  II (b, c, d, a, x[ 1], S44, 0x85845dd1);
	  II (a, b, c, d, x[ 8], S41, 0x6fa87e4f);
	  II (d, a, b, c, x[15], S42, 0xfe2ce6e0);
	  II (c, d, a, b, x[ 6], S43, 0xa3014314);
	  II (b, c, d, a, x[13], S44, 0x4e0811a1);
	  II (a, b, c, d, x[ 4], S41, 0xf7537e82);
	  II (d, a, b, c, x[11], S42, 0xbd3af235);
	  II (c, d, a, b, x[ 2], S43, 0x2ad7d2bb);
	  II (b, c, d, a, x[ 9], S44, 0xeb86d391);

	  m_State[0] += a;
	  m_State[1] += b;
	  m_State[2] += c;
	  m_State[3] += d;

	  memset(x, 0, sizeof(x));
	}
	
	void Encode(unsigned char* dest, unsigned int* src, unsigned int nLength){
		unsigned int i, j;

		assert(nLength % 4 == 0);

		for (i = 0, j = 0; j < nLength; i++, j += 4)
		{
			dest[j] = (unsigned char)(src[i] & 0xff);
			dest[j+1] = (unsigned char)((src[i] >> 8) & 0xff);
			dest[j+2] = (unsigned char)((src[i] >> 16) & 0xff);
			dest[j+3] = (unsigned char)((src[i] >> 24) & 0xff);
		}
	}
	
	void Decode(unsigned int* dest, unsigned char* src, unsigned int nLength){
		unsigned int i, j;

		assert(nLength % 4 == 0);

		for (i = 0, j = 0; j < nLength; i++, j += 4){
			dest[i] = ((unsigned int)src[j]) | (((unsigned int)src[j+1])<<8) |
					  (((unsigned int)src[j+2])<<16) | (((unsigned int)src[j+3])<<24);
		}
	}


	inline	unsigned int	rotate_left(unsigned int x, unsigned int n)
	                 { return ((x << n) | (x >> (32-n))); }

	inline	unsigned int	F(unsigned int x, unsigned int y, unsigned int z)
	                 { return ((x & y) | (~x & z)); }

	inline  unsigned int	G(unsigned int x, unsigned int y, unsigned int z)
	                 { return ((x & z) | (y & ~z)); }

	inline  unsigned int	H(unsigned int x, unsigned int y, unsigned int z)
	                 { return (x ^ y ^ z); }

	inline  unsigned int	I(unsigned int x, unsigned int y, unsigned int z)
	                 { return (y ^ (x | ~z)); }

	inline	void	FF(unsigned int& a, unsigned int b, unsigned int c, unsigned int d, unsigned int x, unsigned int s, unsigned int ac)
	                 { a += F(b, c, d) + x + ac; a = rotate_left(a, s); a += b; }

	inline	void	GG(unsigned int& a, unsigned int b, unsigned int c, unsigned int d, unsigned int x, unsigned int s, unsigned int ac)
                     { a += G(b, c, d) + x + ac; a = rotate_left(a, s); a += b; }

	inline	void	HH(unsigned int& a, unsigned int b, unsigned int c, unsigned int d, unsigned int x, unsigned int s, unsigned int ac)
                     { a += H(b, c, d) + x + ac; a = rotate_left(a, s); a += b; }

	inline	void	II(unsigned int& a, unsigned int b, unsigned int c, unsigned int d, unsigned int x, unsigned int s, unsigned int ac)
                     { a += I(b, c, d) + x + ac; a = rotate_left(a, s); a += b; }

private:
	unsigned int	m_State[4];
	unsigned int	m_Count[2];
	unsigned char	m_Buffer[64];
	unsigned char	m_Digest[16];
	unsigned char	m_Finalized;
	unsigned char	m_Padding[64];
	unsigned int 	S11;
	unsigned int 	S12;
	unsigned int 	S13;
	unsigned int 	S14;
	unsigned int 	S21;
	unsigned int 	S22;
	unsigned int 	S23;
	unsigned int 	S24;
	unsigned int 	S31;
	unsigned int 	S32;
	unsigned int 	S33;
	unsigned int 	S34;
	unsigned int 	S41;
	unsigned int 	S42;
	unsigned int 	S43;
	unsigned int 	S44;
};

}//namespace acl

#endif //__ACL__MD5_HPP__
