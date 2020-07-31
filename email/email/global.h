#pragma once
#include "afx.h"
#include <bitset>
using namespace std;

extern CString m_strPOP3_ser;
extern CString	m_strPOP3_port;
extern CString	m_strSMTP_ser;
extern CString	m_strSMTP_port;
extern CString	m_strUser_name;
extern CString	m_strUser_pass;
extern char key[256];
//extern bitset<64> key;                // 64位密钥
extern bitset<48> subKey[16];         // 存放16轮子密钥

// 初始置换表
extern int IP[];

// 结尾置换表
extern int IP_1[];

/*------------------下面是生成密钥所用表-----------------*/

// 密钥置换表，将64位密钥变成56位
extern int PC_1[];

// 压缩置换，将56位密钥压缩成48位子密钥
extern int PC_2[];

// 每轮左移的位数
extern int shiftBits[];

/*------------------下面是密码函数 f 所用表-----------------*/

// 扩展置换表，将 32位 扩展至 48位
extern int E[];

// S盒，每个S盒是4x16的置换表，6位 -> 4位
extern int S_BOX[8][4][16];

// P置换，32位 -> 32位
extern int P[];

extern bitset<32> f(bitset<32> R, bitset<48> k);
extern bitset<28> leftShift(bitset<28> k, int shift);
extern void generateKeys();
extern bitset<64> charToBitset(const char s[8]);
extern string BitsetTostring(bitset<64> bit);
extern bitset<64> encrypt(bitset<64>& plain);
extern bitset<64> decrypt(bitset<64>& cipher);
extern void char_2_Hex(char* Char, char* Hex);
extern void Hex_2_char(char* Hex, char* Char);
extern char* RC4(char* C, char* key);
