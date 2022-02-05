#include <stdio.h>
#include <memory.h>
#include <string.h>
#include <fstream>
#include <iostream>

using namespace std;

typedef bool    (*PSubKey)[16][48];
enum    {ENCRYPT,DECRYPT};             //选择：加密；解密
static bool SubKey[2][16][48];         // 16圈子密钥
static bool Is3DES;                    // 3次DES标志
static char Tmp[256], deskey[16];      //暂存字符串，密钥串
 
 
static void do_DES(char out[8], char in[8], const PSubKey subkey, bool Type);//标准DES加/解密
static void SetKey(const char* Key, int len);// 设置密钥
static void SetSubKey(PSubKey subkey, const char Key[8]);// 设置子密钥
static void F_func(bool in[32], const bool Ki[48]);// f 函数
static void S_func(bool out[32], const bool in[48]);// S 盒代替
static void transform(bool *out, bool *in, const char *table, int len);// 变换
static void do_xor(bool *inA, const bool *inB, int len);// 异或
static void RotateL(bool *in, int len, int loop);// 循环左移
static void Byte2Bit(bool *out, const char *in, int bits);// 字节组转换成位组
static void Bit2Byte(char *out, const bool *in, int bits);// 位组转换成字节组
 
 
// Type（选择）—ENCRYPT:加密,DECRYPT:解密
// 输出缓冲区(out)的长度 >= ((datalen+7)/8)*8,即比datalen大的且是8的倍数的最小正整数
// in 可以= out,此时加/解密后将覆盖输入缓冲区(in)的内容
// 当keylen>8时系统自动使用3次DES加/解密,否则使用标准DES加/解密.超过16字节后只取前16字节
 
 
//加密解密函数：
bool DES(char *out,char *in,long datalen,const char *Key,int keylen,bool Type = ENCRYPT);
 
 
 
 
int main()
{
	char plain_text[100];                         // 设置明文

    ofstream outfile;
    outfile.open("res.txt");
	char key[100] ;                               // 密钥设置
	printf("Please enter clear text:\n");
    gets(plain_text);
    outfile << "clear text:" << endl;
    outfile << plain_text;
    outfile << endl << endl;
 
	printf("\n Please enter the key:\n");
    gets(key);

    outfile << "Key" << endl;
    outfile << key << endl << endl;
 
	char encrypt_text[255];                       // 密文
	char decrypt_text[255];                       // 解密文
 
	memset(encrypt_text,0,sizeof(encrypt_text));//memset(a,b,c)函数,在a的地址开始到c的长度的字节都初始化为b
	memset(decrypt_text,0,sizeof(decrypt_text));
 
 
 
    // 进行DES加密：
	DES(encrypt_text, plain_text, sizeof(plain_text), key, sizeof(key), ENCRYPT);
	printf("\nThe ciphertext after DES encryption:\n");
	printf("%s\n\n",encrypt_text);

    outfile << "Ciphertext:" << endl;
    outfile << encrypt_text << endl << endl;


 
	// 进行DES解密：
	DES(decrypt_text, encrypt_text, sizeof(plain_text), key, sizeof(key), DECRYPT);
	printf("\nDecrypted output:\n");
	printf("%s",decrypt_text);

    outfile << "After decryption: " << endl;
    outfile << decrypt_text << endl; 
 
    outfile.close();
 
    printf("\n\n");
	return 0;
}
 
 
//下面是DES算法中用到的各种表：
 
// 初始置换IP表
const static char IP_table[64] =
{
	58, 50, 42, 34, 26, 18, 10, 2, 
    60, 52, 44, 36, 28, 20, 12, 4,
	62, 54, 46, 38, 30, 22, 14, 6, 
    64, 56, 48, 40, 32, 24, 16, 8,
	57, 49, 41, 33, 25, 17,  9, 1, 
    59, 51, 43, 35, 27, 19, 11, 3,
    61, 53, 45, 37, 29, 21, 13, 5, 
    63, 55, 47, 39, 31, 23, 15, 7
};
 
 
// 逆初始置换IP1表
const static char Reverse_table[64] =
{
	40, 8, 48, 16, 56, 24, 64, 32, 
    39, 7, 47, 15, 55, 23, 63, 31,
	38, 6, 46, 14, 54, 22, 62, 30, 
    37, 5, 45, 13, 53, 21, 61, 29,
    36, 4, 44, 12, 52, 20, 60, 28, 
    35, 3, 43, 11, 51, 19, 59, 27,
	34, 2, 42, 10, 50, 18, 58, 26, 
    33, 1, 41,  9, 49, 17, 57, 25
};
 
 
// 扩展置换E表
static const char E_table[48] =
{
	32,  1,  2,  3,  4,  5,  
    4,  5,  6,  7,  8,  9,
	8,  9, 10, 11, 12, 13, 
    12, 13, 14, 15, 16, 17,
	16, 17, 18, 19, 20, 21, 
    20, 21, 22, 23, 24, 25,
	24, 25, 26, 27, 28, 29, 
    28, 29, 30, 31, 32,  1
};
 
 
// P盒置换表
const static char P_table[32] =
{
	16, 7, 20, 21, 29, 12, 28, 17, 1,  15, 23, 26, 5,  18, 31, 10,
	2,  8, 24, 14, 32, 27, 3,  9,  19, 13, 30, 6,  22, 11, 4,  25
};
 
 
// 密钥置换表
const static char PC1_table[56] =
{
	57, 49, 41, 33, 25, 17,  9, 
     1, 58, 50, 42, 34, 26, 18,
	10,  2, 59, 51, 43, 35, 27, 
    19, 11,  3, 60, 52, 44, 36,
	63, 55, 47, 39, 31, 23, 15,  
     7, 62, 54, 46, 38, 30, 22,
	14,  6, 61, 53, 45, 37, 29, 
    21, 13,  5, 28, 20, 12,  4
};
 
 
// 压缩置换表
const static char PC2_table[48] =
{
	14, 17, 11, 24,  1,  5, 
     3, 28, 15,  6, 21, 10,
	23, 19, 12,  4, 26,  8, 
    16,  7, 27, 20, 13,  2,
	41, 52, 31, 37, 47, 55, 
    30, 40, 51, 45, 33, 48,
	44, 49, 39, 56, 34, 53, 
    46, 42, 50, 36, 29, 32
};
 
 
// 每轮移动的位数
const static char Move_table[16] =
{
	1,1,2,2,2,2,2,2,1,2,2,2,2,2,2,1
};
 
 
// S盒设计
const static char S_Box[8][4][16] =
{
	// S盒1
	14,	 4,	13,	 1,  2, 15, 11,  8,  3, 10,  6, 12,  5,  9,  0,  7,
	 0, 15,  7,  4, 14,  2, 13,  1, 10,  6, 12, 11,  9,  5,  3,  8,
	 4,  1, 14,  8, 13,  6,  2, 11, 15, 12,  9,  7,  3, 10,  5,  0,
    15, 12,  8,  2,  4,  9,  1,  7,  5, 11,  3, 14, 10,  0,  6, 13,
	// S盒2
    15,  1,  8, 14,  6, 11,  3,  4,  9,  7,  2, 13, 12,  0,  5, 10,
	 3, 13,  4,  7, 15,  2,  8, 14, 12,  0,  1, 10,  6,  9, 11,  5,
	 0, 14,  7, 11, 10,  4, 13,  1,  5,  8, 12,  6,  9,  3,  2, 15,
    13,  8, 10,  1,  3, 15,  4,  2, 11,  6,  7, 12,  0,  5, 14,  9,
	// S盒3
    10,  0,  9, 14,  6,  3, 15,  5,  1, 13, 12,  7, 11,  4,  2,  8,
	13,  7,  0,  9,  3,  4,  6, 10,  2,  8,  5, 14, 12, 11, 15,  1,
	13,  6,  4,  9,  8, 15,  3,  0, 11,  1,  2, 12,  5, 10, 14,  7,
     1, 10, 13,  0,  6,  9,  8,  7,  4, 15, 14,  3, 11,  5,  2, 12,
	// S盒4
     7, 13, 14,  3,  0,  6,  9, 10,  1,  2,  8,  5, 11, 12,  4, 15,
	13,  8, 11,  5,  6, 15,  0,  3,  4,  7,  2, 12,  1, 10, 14,  9,
	10,  6,  9,  0, 12, 11,  7, 13, 15,  1,  3, 14,  5,  2,  8,  4,
     3, 15,  0,  6, 10,  1, 13,  8,  9,  4,  5, 11, 12,  7,  2, 14,
	// S盒5
     2, 12,  4,  1,  7, 10, 11,  6,  8,  5,  3, 15, 13,  0, 14,  9,
	14, 11,  2, 12,  4,  7, 13,  1,  5,  0, 15, 10,  3,  9,  8,  6,
	 4,  2,  1, 11, 10, 13,  7,  8, 15,  9, 12,  5,  6,  3,  0, 14,
    11,  8, 12,  7,  1, 14,  2, 13,  6, 15,  0,  9, 10,  4,  5,  3,
	// S盒6
    12,  1, 10, 15,  9,  2,  6,  8,  0, 13,  3,  4, 14,  7,  5, 11,
	10, 15,  4,  2,  7, 12,  9,  5,  6,  1, 13, 14,  0, 11,  3,  8,
	 9, 14, 15,  5,  2,  8, 12,  3,  7,  0,  4, 10,  1, 13, 11,  6,
     4,  3,  2, 12,  9,  5, 15, 10, 11, 14,  1,  7,  6,  0,  8, 13,
	// S盒7
     4, 11,  2, 14, 15,  0,  8, 13,  3, 12,  9,  7,  5, 10,  6,  1,
	13,  0, 11,  7,  4,  9,  1, 10, 14,  3,  5, 12,  2, 15,  8,  6,
	 1,  4, 11, 13, 12,  3,  7, 14, 10, 15,  6,  8,  0,  5,  9,  2,
     6, 11, 13,  8,  1,  4, 10,  7,  9,  5,  0, 15, 14,  2,  3, 12,
	// S盒8
    13,  2,  8,  4,  6, 15, 11,  1, 10,  9,  3, 14,  5,  0, 12,  7,
	 1, 15, 13,  8, 10,  3,  7,  4, 12,  5,  6, 11,  0, 14,  9,  2,
	 7, 11,  4,  1,  9, 12, 14,  2,  0,  6, 10, 13, 15,  3,  5,  8,
     2,  1, 14,  7,  4, 10,  8, 13, 15, 12,  9,  0,  3,  5,  6, 11
};
 
 
 
//下面是DES算法中调用的函数：
 
// 字节转换函数
void Byte2Bit(bool *out, const char *in, int bits)
{
    for(int i=0; i<bits; ++i)
        out[i] = (in[i>>3]>>(i&7)) & 1;//in[i/8] 这个作用是取出1个字节：i=0~7的时候就取in[0]， i=8~15的时候就取in[1]，……
//in[i/8] >> (i%8)，是把取出来的1个字节右移0~7位，也就是依次取出那个字节的每一个bit
//整个函数的作用是：把in里面的每个字节依次转换为8个bit，最后的结果存到out里面
}
 
 
// 比特转换函数
void Bit2Byte(char *out, const bool *in, int bits)
{
    memset(out, 0, bits>>3);//把每个字节都初始化为0
    for(int i=0; i<bits; ++i)
        out[i>>3] |= in[i]<<(i&7);//i>>3位运算，按位右移三位等于i除以8，i&7按位与运算等于i求余8；
}
 
 
// 变换函数
void transform(bool *out, bool *in, const char *table, int len)
{
    for(int i=0; i<len; ++i)
        Tmp[i] = in[ table[i]-1 ];
    memcpy(out, Tmp, len);
}
 
 
// 异或函数的实现
void do_xor(bool *inA, const bool *inB, int len)
{
    for(int i=0; i<len; ++i)
        inA[i] ^= inB[i];//异或运算，相同为0，不同为1
}
 
 
// 轮转函数
void RotateL(bool *in, int len, int loop)
{
    memcpy(Tmp, in, loop);//Tmp接受左移除的loop个字节
    memcpy(in, in+loop, len-loop);//in更新即剩下的字节向前移动loop个字节
    memcpy(in+len-loop, Tmp, loop);//左移除的字节添加到in的len-loop的位置
}
 
 
// S函数的实现
void S_func(bool out[32], const bool in[48])  //将8组，每组6 bits的串，转化为8组，每组4 bits
{
    for(char i=0,j,k; i<8; ++i,in+=6,out+=4)
	{
        j = (in[0]<<1) + in[5];//取第一位和第六位组成的二进制数为S盒的纵坐标
        k = (in[1]<<3) + (in[2]<<2) + (in[3]<<1) + in[4];//取第二、三、四、五位组成的二进制数为S盒的横坐标
		Byte2Bit(out, &S_Box[i][j][k], 4);
    }
}
 
 
// F函数的实现
void F_func(bool in[32], const bool Ki[48])
{
    static bool MR[48];
    transform(MR, in, E_table, 48);           //先进行 E 扩展
    do_xor(MR, Ki, 48);                          //再异或
    S_func(in, MR);                           //各组字符串分别经过各自的 S 盒
    transform(in, in, P_table, 32);           //最后 P 变换
}
 
 
 
// 设置子密钥
void SetSubKey(PSubKey subkey, const char Key[8])
{
    static bool K[64], *KL=&K[0], *KR=&K[28]; //将64位密钥串去掉8位奇偶位后，分成两份
    Byte2Bit(K, Key, 64);                    //转换格式
    transform(K, K, PC1_table, 56);
 
    for(int i=0; i<16; ++i)                   // 由56位密钥产生48位子密钥
	{
        RotateL(KL, 28, Move_table[i]);       //两份子密钥分别进行左移转换
        RotateL(KR, 28, Move_table[i]);
        transform((*subkey)[i], K, PC2_table, 48);
    }
}
 
 
// 设置密钥
void SetKey(const char* Key, int len)
{
	memset(deskey, 0, 16);
	memcpy(deskey, Key, len>16?16:len);//memcpy(a,b,c)函数，将从b地址开始到c长度的字节的内容复制到a
	SetSubKey(&SubKey[0], &deskey[0]);// 设置子密钥
	Is3DES = len>8 ? (SetSubKey(&SubKey[1], &deskey[8]), true) : false;
}
 
 
// DES加解密函数
void do_DES(char out[8], char in[8], const PSubKey subkey, bool Type)
{
    static bool M[64], tmp[32], *Li=&M[0], *Ri=&M[32];  //64 bits明文 经过IP置换后，分成左右两份
    Byte2Bit(M, in, 64);
    transform(M, M, IP_table, 64);
    if( Type == ENCRYPT )                               //加密
	{
        for(int i=0; i<16; ++i)                         //加密时：子密钥 K0~K15
		{
            memcpy(tmp, Ri, 32);
            F_func(Ri, (*subkey)[i]);                  // 调用F函数
            do_xor(Ri, Li, 32);                            //Li与Ri异或
            memcpy(Li, tmp, 32);
        }
    }
	else                 //解密
	{
        for(int i=15; i>=0; --i)                        // 解密时：Ki的顺序与加密相反
		{
            memcpy(tmp, Li, 32);
            F_func(Li, (*subkey)[i]);
            do_xor(Li, Ri, 32);
            memcpy(Ri, tmp, 32);
        }
	}
    transform(M, M, Reverse_table, 64);                     //最后经过逆初始置换IP-1，得到密文/明文
    Bit2Byte(out, M, 64);
}
 
 
// DES加解密函数（可以对长明文分段加密）
bool DES(char *out, char *in, long datalen, const char *Key, int keylen, bool Type)
{
    if( !( out && in && Key && (datalen=(datalen+7)&0xfffffff8) ) )
		return false;
	SetKey(Key, keylen);
	if( !Is3DES )
	{                                                   // 1次DES
		for(long i=0,j=datalen>>3; i<j; ++i,out+=8,in+=8)
			do_DES(out, in, &SubKey[0], Type);
	}
	else
	{                // 3次DES 加密:加(key0)-解(key1)-加(key0) 解密::解(key0)-加(key1)-解(key0)
		for(long i=0,j=datalen>>3; i<j; ++i,out+=8,in+=8) {
			do_DES(out, in,  &SubKey[0], Type);
			do_DES(out, out, &SubKey[1], !Type);
			do_DES(out, out, &SubKey[0], Type);
		}
	}
	return true;
}