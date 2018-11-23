
#include "pch.h"
#include <iostream>
#include "ctime"
#include "winsock2.h"
#include "string.h"
#include "string"
SOCKET clients[64];
int numClients = 0;
void removeSocket(SOCKET *clients, int *pNumClients, SOCKET removedClient)
{
	int i;
	for (i = 0; i < *pNumClients; i++)
	{
		if (clients[i] == removedClient)
			break;
	}
	if (i < *pNumClients - 1)
	{
		clients[i] = clients[*pNumClients - 1];
	}
	*pNumClients = *pNumClients - 1;
}
void Anh(SOCKET client, char tem[64])
{
	char buf[256], filebuf[1024];
	int res;
	sprintf(buf, "E:\\Test\\picture\%s", tem);
	FILE *f = fopen(buf, "rb");
	fseek(f, 0, SEEK_END);
	int size = ftell(f);// kích thước file
	fseek(f, 0, SEEK_SET);
	sprintf(buf, "HTTP/1.1 200 OK\nContent-Type: image/jpeg\nContent-Length: %d\n\n", size);
	send(client, buf, strlen(buf), 0);
	while (true)
	{
		res = fread(filebuf, 1, sizeof(filebuf), f);
		if (res > 0)
			send(client, filebuf, res, 0);
		else
			break;
	}
	fclose(f);
}
void Nhac(SOCKET client, char tem[64])
{
	char buf[256], filebuf[1024];
	int res;
	sprintf(buf, "E:\\Test\\music\%s", tem);

	FILE *f = fopen(buf, "rb");
	fseek(f, 0, SEEK_END);
	int size = ftell(f);
	fseek(f, 0, SEEK_SET);
	sprintf(buf, "HTTP/1.1 200 OK\nContent-Type: audio/mpeg\nContent-Length: %d\n\n", size);
	send(client, buf, strlen(buf), 0);
	while (true)
	{
		res = fread(filebuf, 1, sizeof(filebuf), f);
		if (res > 0)
			send(client, filebuf, res, 0);
		else
			break;
	}
	fclose(f);
}
bool Check(char *user, char *pass) {
	char buf[1024], filebuf[1024];
	int found = 0;
	sprintf(buf, "%s %s", user, pass);
	FILE *f = fopen("E:\\Test\\data.txt", "r");
	while (fgets(filebuf, sizeof(filebuf), f) != NULL) {
		filebuf[strlen(filebuf) - 1] = 0;
		if (strcmp(buf, filebuf) == 0)
		{
			found = 1;
			break;
		}
	}
	return found;
}
void ghiFile(char IP[20])
{
	char buf[1024];
	time_t hientai = time(0);
	char *tg = ctime(&hientai);
	sprintf(buf, "%s Time:%s\n", IP, tg);
	FILE *f = fopen("E:\\Test\\NHATKY.txt", "a");
	fputs(buf, f);
	fclose(f);
}
void NhatKy(SOCKET client)
{
	char buf[1024];
	char msg[50] = "HTTP/1.1 200 OK\nContent-Type: text/plain\n\n";
	send(client, msg, sizeof(msg), 0);
	FILE *f = fopen("E:\\Test\\NHATKY.txt", "rb");
	while (true) {
		int res = fread(buf, 1, sizeof(buf), f);
		if (res == 0) break;
		send(client, buf, res, 0);
	}
	fclose(f);
}
void html(SOCKET client, char *tem)
{
	char buf[256], filebuf[1024];
	int res;
	sprintf(buf, "E:\\Test\\%s.html", tem);

	FILE *f = fopen(buf, "rb");
	fseek(f, 0, SEEK_END);
	int size = ftell(f);
	fseek(f, 0, SEEK_SET);
	sprintf(buf, "HTTP/1.1 200 OK\nContent-Type:  text/html\nContent-Length: %d\n\n", size);
	send(client, buf, strlen(buf), 0);
	while (true)
	{
		res = fread(filebuf, 1, sizeof(filebuf), f);
		if (res > 0)
			send(client, filebuf, res, 0);
		else
			break;
	}
	fclose(f);
}
void replaceSpace(char path[64], char path1[64])
{
	int i = 0;
	int j = 0;
	while (i < strlen(path))
	{
		if ((path[i] == '%') && (path[i + 1] == '2') && (path[i + 2] == '0'))
		{
			path[i] = ' ';
			path1[j] = path[i];
			i = i + 3;
			j++;
			continue;
		}
		else {
			path1[j] = path[i];
			j++;
		}
		i++;
	}
	path1[j] = 0;
	for (int i = 0; i < strlen(path1); i++)
		if (path1[i] == '/') path1[i] = '\\';
}
void ListFile(SOCKET client, char name[8]) {
	char final[1500];
	char buf[1024];
	char path[30];
	buf[0] = 0;
	sprintf(path, "E:\\Test\\%s\\*.*", name);
	WIN32_FIND_DATAA DATA;
	HANDLE h = FindFirstFileA(path, &DATA);
	do {
		char buf1[256];
		if (strcmp(name, "music") == 0)
			sprintf(buf1, "<p><a href=\"%s\">%s</a></p><br>", DATA.cFileName, DATA.cFileName);
		else if (strcmp(name, "picture") == 0)
			sprintf(buf1, "<p><a href=\"%s\">%s</a></p></br>", DATA.cFileName, DATA.cFileName);

	} while (FindNextFileA(h, &DATA));
	sprintf(final, "HTTP/1.1 200 OK\nContent-Type: text/html; charset=utf-8\n\n<html>%s</html>", buf);
	send(client, final, strlen(final), 0);
}
int main()
{
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);
	SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(9000);

	bind(listener, (SOCKADDR *)&addr, sizeof(addr));
	listen(listener, 5);
	printf("Doi ket noi tu Client!...\n");
	fd_set fdread;
	int res;

	char buf[1024];
	char cmd[16], path[64], path1[64];
	while (true)
	{
		FD_ZERO(&fdread);
		FD_SET(listener, &fdread);
		for (int i = 0; i < numClients; i++)
			FD_SET(clients[i], &fdread);

		res = select(0, &fdread, NULL, NULL, NULL);
		if (res == SOCKET_ERROR)
			break;
		if (res > 0)
		{
			if (FD_ISSET(listener, &fdread))
			{
				SOCKADDR_IN addr1;
				int  len = sizeof(addr1);
				char IP[20];
				SOCKET client = accept(listener, (SOCKADDR *)&addr1, &len);
				printf("Co ket noi moi:%d\n", client);
				sprintf(IP, "IP: %s", inet_ntoa(addr1.sin_addr));
				clients[numClients] = client;
				numClients++;
				ghiFile(IP);

			}
			for (int i = 0; i < numClients; i++)
				if (FD_ISSET(clients[i], &fdread))
				{
					// Xu ly du lieu nhan duoc tu client
					res = recv(clients[i], buf, sizeof(buf), 0);
					if (res == SOCKET_ERROR || res == 0)
					{
						continue;
					}

					buf[res] = 0;
					printf("%s", buf);
					res = sscanf(buf, "%s %s", cmd, path);
					replaceSpace(path, path1);
					if (res == 2)
					{
						if (strcmp(cmd, "GET") == 0)
						{
							if (strcmp(path, "/") == 0)
							{
								char msg[5] = "home";
								html(clients[i], msg);
							}
							else if ((strcmp(path, "/music?") == 0) || (strcmp(path, "/picture?") == 0))
							{
								if (strstr(path, "music") != NULL)
								{
									char msg[6] = "music";
									ListFile(clients[i], msg);
								}
									
								else if (strstr(path, "picture") != NULL)
								{
									char msg[8] = "picture";
									ListFile(clients[i], msg);
								}

							}
							else if (strstr(path1, ".jpg") != NULL)
							{

								Anh(clients[i], path1);
							}
							else if (strstr(path1, ".mp3") != NULL)
							{
								Nhac(clients[i], path1);
							}
						}
						else {
							if (strcmp(path, "/main") == 0) {
								char *post, *user, *pass;
								post = strstr(buf, "user=");
								strtok(post, "=");						// Tách chuỗi có dạng:...user=20166810&pass=thuan01
								user = strtok(NULL, "&");
								strtok(NULL, "=");
								pass = strtok(NULL, "=");
								if (Check(user, pass) == 1)
								{
									char msg[6] = "admin";
									html(clients[i], msg);
								}
								else
								{
									char msg[12] = "loginfailed";
									html(clients[i], msg);
								}
							}
							else if (strcmp(path, "/nhatky") == 0) {
								NhatKy(clients[i]);
							}
						}
						closesocket(clients[i]);
						removeSocket(clients, &numClients, clients[i]);
						i--;

					}
				}
		}
	}

	return 0;
     
}
