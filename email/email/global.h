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
//extern bitset<64> key;                // 64λ��Կ
extern bitset<48> subKey[16];         // ���16������Կ

// ��ʼ�û���
extern int IP[];

// ��β�û���
extern int IP_1[];

/*------------------������������Կ���ñ�-----------------*/

// ��Կ�û�����64λ��Կ���56λ
extern int PC_1[];

// ѹ���û�����56λ��Կѹ����48λ����Կ
extern int PC_2[];

// ÿ�����Ƶ�λ��
extern int shiftBits[];

/*------------------���������뺯�� f ���ñ�-----------------*/

// ��չ�û����� 32λ ��չ�� 48λ
extern int E[];

// S�У�ÿ��S����4x16���û���6λ -> 4λ
extern int S_BOX[8][4][16];

// P�û���32λ -> 32λ
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
