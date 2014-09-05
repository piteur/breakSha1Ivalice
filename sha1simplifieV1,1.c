#include <stdio.h> 
#include <string.h> 
#include <windows.h>
#ifndef _GLOBAL_H_
#define _GLOBAL_H_ 1

/* POINTER defines a generic pointer type */
typedef unsigned char *POINTER;

/* UINT4 defines a four byte word */
typedef unsigned long int UINT4;

/* BYTE defines a unsigned character */
typedef unsigned char BYTE;

#ifndef TRUE
  #define FALSE	0
  #define TRUE	( !FALSE )
#endif /* TRUE */

#endif /* end _GLOBAL_H_ */

/* sha.h */

#ifndef _SHA_H_
#define _SHA_H_ 1

/* #include "global.h" */

/* The structure for storing SHS info */

typedef struct 
{
	UINT4 digest[ 5 ];            /* Message digest */
	UINT4 countLo, countHi;       /* 64-bit bit count */
	UINT4 data[ 16 ];             /* SHS data buffer */
	int Endianness;
} SHA_CTX;

/* Message digest functions */

void SHAInit(SHA_CTX *);
void SHAUpdate(SHA_CTX *, BYTE *buffer, int count);
void SHAFinal(BYTE *output, SHA_CTX *);

#endif /* end _SHA_H_ */

/* endian.h */

#ifndef _ENDIAN_H_
#define _ENDIAN_H_ 1

void endianTest(int *endianness);

#endif /* end _ENDIAN_H_ */

/* sha.c */

#include <stdio.h>
#include <string.h>

static void SHAtoByte(BYTE *output, UINT4 *input, unsigned int len);

/* The SHS block size and message digest sizes, in bytes */

#define SHS_DATASIZE    64
#define SHS_DIGESTSIZE  20


/* The SHS f()-functions.  The f1 and f3 functions can be optimized to
   save one boolean operation each - thanks to Rich Schroeppel,
   rcs@cs.arizona.edu for discovering this */

/*#define f1(x,y,z) ( ( x & y ) | ( ~x & z ) )          // Rounds  0-19 */
#define f1(x,y,z)   ( z ^ ( x & ( y ^ z ) ) )           /* Rounds  0-19 */
#define f2(x,y,z)   ( x ^ y ^ z )                       /* Rounds 20-39 */
/*#define f3(x,y,z) ( ( x & y ) | ( x & z ) | ( y & z ) )   // Rounds 40-59 */
#define f3(x,y,z)   ( ( x & y ) | ( z & ( x | y ) ) )   /* Rounds 40-59 */
#define f4(x,y,z)   ( x ^ y ^ z )                       /* Rounds 60-79 */

/* The SHS Mysterious Constants */

#define K1  0x5A827999L                                 /* Rounds  0-19 */
#define K2  0x6ED9EBA1L                                 /* Rounds 20-39 */
#define K3  0x8F1BBCDCL                                 /* Rounds 40-59 */
#define K4  0xCA62C1D6L                                 /* Rounds 60-79 */

/* SHS initial values */

#define h0init  0x67452301L
#define h1init  0xEFCDAB89L
#define h2init  0x98BADCFEL
#define h3init  0x10325476L
#define h4init  0xC3D2E1F0L

/* Note that it may be necessary to add parentheses to these macros if they
   are to be called with expressions as arguments */
/* 32-bit rotate left - kludged with shifts */

#define ROTL(n,X)  ( ( ( X ) << n ) | ( ( X ) >> ( 32 - n ) ) )

/* The initial expanding function.  The hash function is defined over an
   80-UINT2 expanded input array W, where the first 16 are copies of the input
   data, and the remaining 64 are defined by

        W[ i ] = W[ i - 16 ] ^ W[ i - 14 ] ^ W[ i - 8 ] ^ W[ i - 3 ]

   This implementation generates these values on the fly in a circular
   buffer - thanks to Colin Plumb, colin@nyx10.cs.du.edu for this
   optimization.

   The updated SHS changes the expanding function by adding a rotate of 1
   bit.  Thanks to Jim Gillogly, jim@rand.org, and an anonymous contributor
   for this information */

#define expand(W,i) ( W[ i & 15 ] = ROTL( 1, ( W[ i & 15 ] ^ W[ (i - 14) & 15 ] ^ \
                                                 W[ (i - 8) & 15 ] ^ W[ (i - 3) & 15 ] ) ) )


/* The prototype SHS sub-round.  The fundamental sub-round is:

        a' = e + ROTL( 5, a ) + f( b, c, d ) + k + data;
        b' = a;
        c' = ROTL( 30, b );
        d' = c;
        e' = d;

   but this is implemented by unrolling the loop 5 times and renaming the
   variables ( e, a, b, c, d ) = ( a', b', c', d', e' ) each iteration.
   This code is then replicated 20 times for each of the 4 functions, using
   the next 20 values from the W[] array each time */

#define subRound(a, b, c, d, e, f, k, data) \
    ( e += ROTL( 5, a ) + f( b, c, d ) + k + data, b = ROTL( 30, b ) )

/* Initialize the SHS values */

void SHAInit(SHA_CTX *shsInfo)
{
    endianTest(&shsInfo->Endianness);
    /* Set the h-vars to their initial values */
    shsInfo->digest[ 0 ] = h0init;
    shsInfo->digest[ 1 ] = h1init;
    shsInfo->digest[ 2 ] = h2init;
    shsInfo->digest[ 3 ] = h3init;
    shsInfo->digest[ 4 ] = h4init;

    /* Initialise bit count */
    shsInfo->countLo = shsInfo->countHi = 0;
}


/* Perform the SHS transformation.  Note that this code, like MD5, seems to
   break some optimizing compilers due to the complexity of the expressions
   and the size of the basic block.  It may be necessary to split it into
   sections, e.g. based on the four subrounds

   Note that this corrupts the shsInfo->data area */

static void SHSTransform( digest, data )
     UINT4 *digest, *data ;
    {
    UINT4 A, B, C, D, E;     /* Local vars */
    UINT4 eData[ 16 ];       /* Expanded data */

    /* Set up first buffer and local data buffer */
    A = digest[ 0 ];
    B = digest[ 1 ];
    C = digest[ 2 ];
    D = digest[ 3 ];
    E = digest[ 4 ];
    memcpy( (POINTER)eData, (POINTER)data, SHS_DATASIZE );

    /* Heavy mangling, in 4 sub-rounds of 20 interations each. */
    subRound( A, B, C, D, E, f1, K1, eData[  0 ] );
    subRound( E, A, B, C, D, f1, K1, eData[  1 ] );
    subRound( D, E, A, B, C, f1, K1, eData[  2 ] );
    subRound( C, D, E, A, B, f1, K1, eData[  3 ] );
    subRound( B, C, D, E, A, f1, K1, eData[  4 ] );
    subRound( A, B, C, D, E, f1, K1, eData[  5 ] );
    subRound( E, A, B, C, D, f1, K1, eData[  6 ] );
    subRound( D, E, A, B, C, f1, K1, eData[  7 ] );
    subRound( C, D, E, A, B, f1, K1, eData[  8 ] );
    subRound( B, C, D, E, A, f1, K1, eData[  9 ] );
    subRound( A, B, C, D, E, f1, K1, eData[ 10 ] );
    subRound( E, A, B, C, D, f1, K1, eData[ 11 ] );
    subRound( D, E, A, B, C, f1, K1, eData[ 12 ] );
    subRound( C, D, E, A, B, f1, K1, eData[ 13 ] );
    subRound( B, C, D, E, A, f1, K1, eData[ 14 ] );
    subRound( A, B, C, D, E, f1, K1, eData[ 15 ] );
    subRound( E, A, B, C, D, f1, K1, expand( eData, 16 ) );
    subRound( D, E, A, B, C, f1, K1, expand( eData, 17 ) );
    subRound( C, D, E, A, B, f1, K1, expand( eData, 18 ) );
    subRound( B, C, D, E, A, f1, K1, expand( eData, 19 ) );

    subRound( A, B, C, D, E, f2, K2, expand( eData, 20 ) );
    subRound( E, A, B, C, D, f2, K2, expand( eData, 21 ) );
    subRound( D, E, A, B, C, f2, K2, expand( eData, 22 ) );
    subRound( C, D, E, A, B, f2, K2, expand( eData, 23 ) );
    subRound( B, C, D, E, A, f2, K2, expand( eData, 24 ) );
    subRound( A, B, C, D, E, f2, K2, expand( eData, 25 ) );
    subRound( E, A, B, C, D, f2, K2, expand( eData, 26 ) );
    subRound( D, E, A, B, C, f2, K2, expand( eData, 27 ) );
    subRound( C, D, E, A, B, f2, K2, expand( eData, 28 ) );
    subRound( B, C, D, E, A, f2, K2, expand( eData, 29 ) );
    subRound( A, B, C, D, E, f2, K2, expand( eData, 30 ) );
    subRound( E, A, B, C, D, f2, K2, expand( eData, 31 ) );
    subRound( D, E, A, B, C, f2, K2, expand( eData, 32 ) );
    subRound( C, D, E, A, B, f2, K2, expand( eData, 33 ) );
    subRound( B, C, D, E, A, f2, K2, expand( eData, 34 ) );
    subRound( A, B, C, D, E, f2, K2, expand( eData, 35 ) );
    subRound( E, A, B, C, D, f2, K2, expand( eData, 36 ) );
    subRound( D, E, A, B, C, f2, K2, expand( eData, 37 ) );
    subRound( C, D, E, A, B, f2, K2, expand( eData, 38 ) );
    subRound( B, C, D, E, A, f2, K2, expand( eData, 39 ) );

    subRound( A, B, C, D, E, f3, K3, expand( eData, 40 ) );
    subRound( E, A, B, C, D, f3, K3, expand( eData, 41 ) );
    subRound( D, E, A, B, C, f3, K3, expand( eData, 42 ) );
    subRound( C, D, E, A, B, f3, K3, expand( eData, 43 ) );
    subRound( B, C, D, E, A, f3, K3, expand( eData, 44 ) );
    subRound( A, B, C, D, E, f3, K3, expand( eData, 45 ) );
    subRound( E, A, B, C, D, f3, K3, expand( eData, 46 ) );
    subRound( D, E, A, B, C, f3, K3, expand( eData, 47 ) );
    subRound( C, D, E, A, B, f3, K3, expand( eData, 48 ) );
    subRound( B, C, D, E, A, f3, K3, expand( eData, 49 ) );
    subRound( A, B, C, D, E, f3, K3, expand( eData, 50 ) );
    subRound( E, A, B, C, D, f3, K3, expand( eData, 51 ) );
    subRound( D, E, A, B, C, f3, K3, expand( eData, 52 ) );
    subRound( C, D, E, A, B, f3, K3, expand( eData, 53 ) );
    subRound( B, C, D, E, A, f3, K3, expand( eData, 54 ) );
    subRound( A, B, C, D, E, f3, K3, expand( eData, 55 ) );
    subRound( E, A, B, C, D, f3, K3, expand( eData, 56 ) );
    subRound( D, E, A, B, C, f3, K3, expand( eData, 57 ) );
    subRound( C, D, E, A, B, f3, K3, expand( eData, 58 ) );
    subRound( B, C, D, E, A, f3, K3, expand( eData, 59 ) );

    subRound( A, B, C, D, E, f4, K4, expand( eData, 60 ) );
    subRound( E, A, B, C, D, f4, K4, expand( eData, 61 ) );
    subRound( D, E, A, B, C, f4, K4, expand( eData, 62 ) );
    subRound( C, D, E, A, B, f4, K4, expand( eData, 63 ) );
    subRound( B, C, D, E, A, f4, K4, expand( eData, 64 ) );
    subRound( A, B, C, D, E, f4, K4, expand( eData, 65 ) );
    subRound( E, A, B, C, D, f4, K4, expand( eData, 66 ) );
    subRound( D, E, A, B, C, f4, K4, expand( eData, 67 ) );
    subRound( C, D, E, A, B, f4, K4, expand( eData, 68 ) );
    subRound( B, C, D, E, A, f4, K4, expand( eData, 69 ) );
    subRound( A, B, C, D, E, f4, K4, expand( eData, 70 ) );
    subRound( E, A, B, C, D, f4, K4, expand( eData, 71 ) );
    subRound( D, E, A, B, C, f4, K4, expand( eData, 72 ) );
    subRound( C, D, E, A, B, f4, K4, expand( eData, 73 ) );
    subRound( B, C, D, E, A, f4, K4, expand( eData, 74 ) );
    subRound( A, B, C, D, E, f4, K4, expand( eData, 75 ) );
    subRound( E, A, B, C, D, f4, K4, expand( eData, 76 ) );
    subRound( D, E, A, B, C, f4, K4, expand( eData, 77 ) );
    subRound( C, D, E, A, B, f4, K4, expand( eData, 78 ) );
    subRound( B, C, D, E, A, f4, K4, expand( eData, 79 ) );

    /* Build message digest */
    digest[ 0 ] += A;
    digest[ 1 ] += B;
    digest[ 2 ] += C;
    digest[ 3 ] += D;
    digest[ 4 ] += E;
    }

/* When run on a little-endian CPU we need to perform byte reversal on an
   array of long words. */

static void longReverse(UINT4 *buffer, int byteCount, int Endianness )
{
    UINT4 value;

    if (Endianness==TRUE) return;
    byteCount /= sizeof( UINT4 );
    while( byteCount-- )
        {
        value = *buffer;
        value = ( ( value & 0xFF00FF00L ) >> 8  ) | \
                ( ( value & 0x00FF00FFL ) << 8 );
        *buffer++ = ( value << 16 ) | ( value >> 16 );
        }
}

/* Update SHS for a block of data */

void SHAUpdate(SHA_CTX *shsInfo, BYTE *buffer, int count)
{
    UINT4 tmp;
    int dataCount;

    /* Update bitcount */
    tmp = shsInfo->countLo;
    if ( ( shsInfo->countLo = tmp + ( ( UINT4 ) count << 3 ) ) < tmp )
        shsInfo->countHi++;             /* Carry from low to high */
    shsInfo->countHi += count >> 29;

    /* Get count of bytes already in data */
    dataCount = ( int ) ( tmp >> 3 ) & 0x3F;

    /* Handle any leading odd-sized chunks */
    if( dataCount )
        {
        BYTE *p = ( BYTE * ) shsInfo->data + dataCount;

        dataCount = SHS_DATASIZE - dataCount;
        if( count < dataCount )
            {
            memcpy( p, buffer, count );
            return;
            }
        memcpy( p, buffer, dataCount );
        longReverse( shsInfo->data, SHS_DATASIZE, shsInfo->Endianness);
        SHSTransform( shsInfo->digest, shsInfo->data );
        buffer += dataCount;
        count -= dataCount;
        }

    /* Process data in SHS_DATASIZE chunks */
    while( count >= SHS_DATASIZE )
        {
        memcpy( (POINTER)shsInfo->data, (POINTER)buffer, SHS_DATASIZE );
        longReverse( shsInfo->data, SHS_DATASIZE, shsInfo->Endianness );
        SHSTransform( shsInfo->digest, shsInfo->data );
        buffer += SHS_DATASIZE;
        count -= SHS_DATASIZE;
        }

    /* Handle any remaining bytes of data. */
    memcpy( (POINTER)shsInfo->data, (POINTER)buffer, count );
    }

/* Final wrapup - pad to SHS_DATASIZE-byte boundary with the bit pattern
   1 0* (64-bit count of bits processed, MSB-first) */

void SHAFinal(BYTE *output, SHA_CTX *shsInfo)
{
    int count;
    BYTE *dataPtr;

    /* Compute number of bytes mod 64 */
    count = ( int ) shsInfo->countLo;
    count = ( count >> 3 ) & 0x3F;

    /* Set the first char of padding to 0x80.  This is safe since there is
       always at least one byte free */
    dataPtr = ( BYTE * ) shsInfo->data + count;
    *dataPtr++ = 0x80;

    /* Bytes of padding needed to make 64 bytes */
    count = SHS_DATASIZE - 1 - count;

    /* Pad out to 56 mod 64 */
    if( count < 8 )
        {
        /* Two lots of padding:  Pad the first block to 64 bytes */
        memset( dataPtr, 0, count );
        longReverse( shsInfo->data, SHS_DATASIZE, shsInfo->Endianness );
        SHSTransform( shsInfo->digest, shsInfo->data );

        /* Now fill the next block with 56 bytes */
        memset( (POINTER)shsInfo->data, 0, SHS_DATASIZE - 8 );
        }
    else
        /* Pad block to 56 bytes */
        memset( dataPtr, 0, count - 8 );

    /* Append length in bits and transform */
    shsInfo->data[ 14 ] = shsInfo->countHi;
    shsInfo->data[ 15 ] = shsInfo->countLo;

    longReverse( shsInfo->data, SHS_DATASIZE - 8, shsInfo->Endianness );
    SHSTransform( shsInfo->digest, shsInfo->data );

	/* Output to an array of bytes */
	SHAtoByte(output, shsInfo->digest, SHS_DIGESTSIZE);

	/* Zeroise sensitive stuff */
	memset((POINTER)shsInfo, 0, sizeof(shsInfo));
}

static void SHAtoByte(BYTE *output, UINT4 *input, unsigned int len)
{	/* Output SHA digest in byte array */
	unsigned int i, j;

	for(i = 0, j = 0; j < len; i++, j += 4) 
	{
        output[j+3] = (BYTE)( input[i]        & 0xff);
        output[j+2] = (BYTE)((input[i] >> 8 ) & 0xff);
        output[j+1] = (BYTE)((input[i] >> 16) & 0xff);
        output[j  ] = (BYTE)((input[i] >> 24) & 0xff);
	}
}


unsigned char digest[20];


/* Output should look like:-
 a9993e36 4706816a ba3e2571 7850c26c 9cd0d89d
 A9993E36 4706816A BA3E2571 7850C26C 9CD0D89D <= correct
 84983e44 1c3bd26e baae4aa1 f95129e5 e54670f1
 84983E44 1C3BD26E BAAE4AA1 F95129E5 E54670F1 <= correct
 34aa973c d4c4daa4 f61eeb2b dbad2731 6534016f
 34AA973C D4C4DAA4 F61EEB2B DBAD2731 6534016F <= correct
*/
/* endian.c */

void endianTest(int *endian_ness)
{
	if((*(unsigned short *) ("#S") >> 8) == '#')
	{
		/* printf("Big endian = no change\n"); */
		*endian_ness = !(0);
	}
	else
	{
		/* printf("Little endian = swap\n"); */
		*endian_ness = 0;
	}
}


char transforme(int chiffre)
{
    switch(chiffre)
    {
    case 0 : return('0');break;
    case 1 : return('1');break;
    case 2 : return('2');break;
    case 3 : return('3');break;
    case 4 : return('4');break;
    case 5 : return('5');break;
    case 6 : return('6');break;
    case 7 : return('7');break;
    case 8 : return('8');break;
    case 9 : return('9');break;
    case 10 : return('a');break;
    case 11 : return('b');break;
    case 12 : return('c');break;
    case 13 : return('d');break;
    case 14 : return('e');break;
    case 15 : return('f');break;
    case 16 : return('g');break;
    case 17 : return('h');break;
    case 18 : return('i');break;
    case 19 : return('j');break;
    case 20 : return('k');break;
    case 21 : return('l');break;
    case 22 : return('m');break;
    case 23 : return('n');break;
    case 24 : return('o');break;
    case 25 : return('p');break;
    case 26 : return('q');break;
    case 27 : return('r');break;
    case 28 : return('s');break;
    case 29 : return('t');break;
    case 30 : return('u');break;
    case 31 : return('v');break;
    case 32 : return('w');break;
    case 33 : return('x');break;
    case 34 : return('y');break;
    case 35 : return('z');break;
    case 36 : return('A');break;
    case 37 : return('B');break;
    case 38 : return('C');break;
    case 39 : return('D');break;
    case 40 : return('E');break;
    case 41 : return('F');break;
    case 42 : return('G');break;
    case 43 : return('H');break;
    case 44 : return('I');break;
    case 45 : return('J');break;
    case 46 : return('K');break;
    case 47 : return('L');break;
    case 48 : return('M');break;
    case 49 : return('N');break;
    case 50 : return('O');break;
    case 51 : return('P');break;
    case 52 : return('Q');break;
    case 53 : return('R');break;
    case 54 : return('S');break;
    case 55 : return('T');break;
    case 56 : return('U');break;
    case 57 : return('V');break;
    case 58 : return('W');break;
    case 59 : return('X');break;
    case 60 : return('Y');break;
    case 61 : return('Z');break;
    }
}

int transformeinv(char carac)
{
    switch(carac)
    {
    case '0' : return(0);break;
    case '1' : return(1);break;
    case '2' : return(2);break;
    case '3' : return(3);break;
    case '4' : return(4);break;
    case '5' : return(5);break;
    case '6' : return(6);break;
    case '7' : return(7);break;
    case '8' : return(8);break;
    case '9' : return(9);break;
    case 'a' : return(10);break;
    case 'b' : return(11);break;
    case 'c' : return(12);break;
    case 'd' : return(13);break;
    case 'e' : return(14);break;
    case 'f' : return(15);break;
    case 'g' : return(16);break;
    case 'h' : return(17);break;
    case 'i' : return(18);break;
    case 'j' : return(19);break;
    case 'k' : return(20);break;
    case 'l' : return(21);break;
    case 'm' : return(22);break;
    case 'n' : return(23);break;
    case 'o' : return(24);break;
    case 'p' : return(25);break;
    case 'q' : return(26);break;
    case 'r' : return(27);break;
    case 's' : return(28);break;
    case 't' : return(29);break;
    case 'u' : return(30);break;
    case 'v' : return(31);break;
    case 'w' : return(32);break;
    case 'x' : return(33);break;
    case 'y' : return(34);break;
    case 'z' : return(35);break;
    case 'A' : return(36);break;
    case 'B' : return(37);break;
    case 'C' : return(38);break;
    case 'D' : return(39);break;
    case 'E' : return(40);break;
    case 'F' : return(41);break;
    case 'G' : return(42);break;
    case 'H' : return(43);break;
    case 'I' : return(44);break;
    case 'J' : return(45);break;
    case 'K' : return(46);break;
    case 'L' : return(47);break;
    case 'M' : return(48);break;
    case 'N' : return(49);break;
    case 'O' : return(50);break;
    case 'P' : return(51);break;
    case 'Q' : return(52);break;
    case 'R' : return(53);break;
    case 'S' : return(54);break;
    case 'T' : return(55);break;
    case 'U' : return(56);break;
    case 'V' : return(57);break;
    case 'W' : return(58);break;
    case 'X' : return(59);break;
    case 'Y' : return(60);break;
    case 'Z' : return(61);break;
    }
}

/////////////////////////////////////////////////////////////////
//Fonction pour les couleurs en C (trouvé sur le net)
////////////////////////////////////////////////////////////////////////
//0 : Noir
//1 : Bleu foncé
//2 : Vert foncé
//3 : Turquoise
//4 : Rouge foncé
//5 : Violet
//6 : Vert caca d'oie
//7 : Gris clair
//8 : Gris foncé
//9 : Bleu fluo
//10 : Vert fluo
//11 : Turquoise
//12 : Rouge fluo
//13 : Violet 2
//14 : Jaune
//15 : Blanc 
void Color(int couleurDuTexte,int couleurDeFond)
{
        HANDLE H=GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(H,couleurDeFond*16+couleurDuTexte);
}


void AfficherAvancement()
{
int i,j,k,l,m=0;char mot[3];int etat;FILE* fichier;
for(i=0;i<62;i++)
     {
     printf("\n%c : ",transforme(i));
     l=0;
     for(j=0;j<62;j++)
     {
     k=j+62*i;
     sprintf (mot, "%d", k);   
     fichier = fopen(mot, "r");
     fscanf(fichier, "%d", &etat);
     fclose(fichier);
     switch(etat)
         {
         case 0:Color(7,4);printf("%c",transforme(j));break;
         case 1:Color(7,14);printf("%c",transforme(j));break;
         case 2:Color(7,2);printf("%c",transforme(j));l=l+1;break;
         case 3:Color(7,5);printf("%c",transforme(j));l=l+1;break;
         }
     }
     Color(7,0);
     printf(" %d/62",l);
     m=m+l;
     }
     printf(" %d/3844",m);
}

void tester(unsigned char* propose,unsigned char* cherche)
{FILE* fichier = NULL;
int i=0,ok=0;
	SHA_CTX sha;
    SHAInit(&sha);
	SHAUpdate(&sha, propose, 6);
	SHAFinal(digest, &sha);
    char pRet[4];
    do
        {
        sprintf (pRet, "%02x", digest [i/2]);
        if(strcmp("Dj89Io",propose)==0){printf("%c-%c et %c-%c",pRet[0],cherche[i],pRet[1],cherche[i+1]);}
        if(pRet[0]!=cherche[i] || pRet[1]!=cherche[i+1])
        {return (void)0;}
        i=i+2;    
        }while(i<20);
        
        char mot[3];
        sprintf (mot, "%d", transformeinv(propose[0])*62+transformeinv(propose[1]));
        fichier = fopen(mot, "w+");fputs("3 TROUVE LE RESULTAT\n", fichier);
        printf("\nTROUVE !!!!!"); for(ok=0;ok<6;ok++){printf("%c",propose[ok]);fputc(propose[ok], fichier);}
        fclose(fichier);          

}


main()
{
	int i,stop=0,taillemot;char memoire[40];
	//Le code en sha1 que l'on cherche à casser
    unsigned char propose[40];
    FILE* fichier = NULL;
    fichier = fopen("code.txt", "r");
    unsigned char cherche[41];  
    fgets(cherche, 41, fichier);
    fclose(fichier);
    char proposee[6];

    printf("\n RECHERCHE DE \n %s\n",cherche);
        int numero,etat;
        char mot[3];
        int i1,i2,i3,i4,i5;
        numero=-1;
    printf("Mode rand ? (0=non 1=oui)\n");
    int ran;int lim;
    scanf("%d",&ran);
    printf("affichage de l'avancement ?(10=oui 11=non)\n");
    int fait;
    scanf("%d",&fait);

    
    while(stop==0)
    {
        if(ran==1)
            {lim=0;
            do{lim=lim+1;
            numero=rand()%3844;
            sprintf (mot, "%d", numero);   
            fichier = fopen(mot, "r");
            fscanf(fichier, "%d", &etat);
            fclose(fichier);
            }while(etat!=0 && lim<3800);
            }
        if(ran==0 || lim ==3800)
        {numero=-1;ran=0;
            do{
            numero=numero+1;
            sprintf (mot, "%d", numero);   
            fichier = fopen(mot, "r");
            fscanf(fichier, "%d", &etat);
            fclose(fichier);
            }while(etat != 0 && numero<3844);
            }
        if(numero>=3844)
        {stop=1;}
        else
        {       
            printf("TEST DU CARAC %c%c(fichier numero %d)",transforme(numero/62),transforme(numero%62),numero);     
            fichier = fopen(mot, "w+");
            fputc('1', fichier);
            fclose(fichier);
            
            if(fait==10)
                {
                fait=0;
                AfficherAvancement();
                printf("\n");
                }
            fait=fait+1;
            
            propose[0]=transforme(numero/62);
            propose[1]=transforme(numero%62);
            for(i2=0;i2<62;i2++)
				{
				propose[2]=transforme(i2);
				for(i3=0;i3<62;i3++)
					{
					propose[3]=transforme(i3);
					for(i4=0;i4<62;i4++)
						{
						propose[4]=transforme(i4);
						for(i5=0;i5<62;i5++)
							{
							propose[5]=transforme(i5);
							tester(propose,cherche);            
							}
						}
					}
				}
				

            fichier = fopen(mot, "r");
            fscanf(fichier, "%d", &etat);
            fclose(fichier);
            if(etat==1)
            {
            fichier = fopen(mot, "w+");
            fputc('2', fichier);
            fclose(fichier);    
            }
        	printf("\n TERMINE\n");
        }
    }
    printf("\n Fin du balayage\n");scanf("%d",&i);
}



