#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#include <iostream>
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <time.h>
#include <assert.h>
#include "AES/AesCrypto.h"

#define AES_KEY_BASE "hey28dbsjci239d7"

#pragma warning(disable : 4996)
#pragma comment(lib, "ws2_32.lib")
using namespace std;
using namespace my_cryptoAes;

void my_delay(int dur);

char* get_ip(char* a);
int get_port(char* a);

int init();
void deinit();
int sock_err(const char* func, int s);
void s_close(int s);

char* aes_update_key(int);
int aes_get_key_mask(char**);
char* decrypt_large_text(char* text, int length, unsigned char* key);

int main(int argc, char* argv[])
{
	char default_ipport[] = "127.0.0.1:9000";
	if (argc < 2)
		cout << "[!] IP & port not found :: client.exe IP:PORT key_mask\n";
	

	init();
	int s = socket(AF_INET, SOCK_STREAM, 0);
	if (s < 0)
		return sock_err("socket", s);

	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = (argc == 2) ? htons(get_port(argv[1])) : htons(get_port(default_ipport));
	addr.sin_addr.s_addr = (argc == 2) ? inet_addr(get_ip(argv[1])) : inet_addr(get_ip(default_ipport));

	cout << "waiting";
	int check_conection;
	for (int i = 0; i < 10; i++)
	{
		cout << '.';
		check_conection = connect(s, (struct sockaddr*) & addr, sizeof(addr));
		if (check_conection != 0)
			my_delay(100);
		else break;
	}
	cout << '\n';

	if (check_conection != 0)
		return sock_err("connect", s);

	char get[] = { 'g', 'e', 't' };
	
	int rcv = send(s, get, 3, 0);
	if (rcv < 0)
		return sock_err("send", s);

	char buf[1000];
	rcv = recv(s, buf, sizeof(buf), 0);
	if (rcv < 0)
		return sock_err("recv", s);
	if (rcv < 1000)
		buf[rcv] = '\0';

	if (!strcmp(buf, "unknown request"))
	{
		cout << "unknown request\n";
		deinit();
		return 0;
	}

	char* key;
	if (argc == 3)
		key = aes_update_key(aes_get_key_mask(argv));
	else
	{
		key = (char*)malloc(16); assert(key);
		strcpy(key, AES_KEY_BASE);
	}
	
	cout << "Recieve " << rcv << " bytes from server\n";
	cout << "##### Message Begin ##### \n\n";

	char* decrypted = decrypt_large_text(buf, rcv, (unsigned char*) key);
	cout << decrypted << endl;

	cout << "##### Message End #####\n\n";

	//free(key);
	deinit();
	return 0;
}

char* decrypt_large_text(char* text, int length, unsigned char* key)
{
	char* decrypted_text = (char*)malloc(length); assert(decrypted_text);
	char block[16];

	AesCrypto aes;
	int i = 0;
	while (i < length)
	{
		memset(block, 0, 16);
		memcpy(block, text + i, 16);

		unsigned char* dec_block = aes.decrypt((unsigned char*)block, key);
		memcpy(decrypted_text + i, (char*)dec_block, 16);

		i += 16;
	}
	return decrypted_text;
}

int aes_get_key_mask(char** argv)
{
	int res = 0;
	for (int i = 0; i < strlen(argv[2]); i++)
		res = res * 10 + (argv[2][i] - '0');
	return res;
}

char* aes_update_key(int mask)
{
	char mask_bytes[] = { ((mask >> 24) & 0xff), ((mask >> 16) & 0xff), ((mask >> 8) & 0xff), (mask & 0xff) };
	char* res = (char*)malloc(16); assert(res);
	for (int i = 0, j = 0; i < 16; i++, j = (j + 1) % 4)
		res[i] = AES_KEY_BASE[i] ^ mask_bytes[j];
	return res;
}

void my_delay(int dur)
{
	time_t t1 = clock();
	while (clock() - t1 < dur);
}

int init()
{
#ifdef _WIN32
	WSADATA wsa_data;
	return (0 == WSAStartup(MAKEWORD(2, 2), &wsa_data));
#else
	return 1;
#endif
}

void deinit()
{
#ifdef _WIN32
	WSACleanup();
#else
#endif
}

int sock_err(const char* func, int s)
{
	int err;
#ifdef _WIN32
	err = WSAGetLastError();
#else
	err = errno;
#endif

	printf("%s: error: %d\r\n", func, err);
#ifdef _WIN32
	system("pause");
#endif // _WIN32
	return -1;
}

void s_close(int s)
{
#ifdef _WIN32
	closesocket(s);
#else
	close(s);
#endif
}

char* get_ip(char* a)
{
	char* res = (char*)malloc(sizeof(char) * 15); assert(res);
	char* trace = res;
	while ((*a) != ':' && (*a))
	{
		(*trace) = (*a);
		trace++;
		a++;
	}
	(*trace) = '\0';
	return res;
}

int get_port(char* a)
{
	while ((*a) != ':' && a) a++;
	int res = 0;
	a++;
	while (*a)
	{
		res = res * 10 + ((*a) - '0');
		a++;
	}
	return res;
}

