#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>

#define MAX 64
#define SERVER_IP "127.0.0.1" //local loopback
#define SMTP 1900
#define POP 1901

void func(int sockfd) 
{ 
    char buff[MAX]; 
    int n; 
    // infinite loop for chat 
    for (;;) { 
        bzero(buff, MAX); 
  
        // read the message from client and copy it in buffer 
        recv(sockfd, buff, sizeof(buff)); 
        // print buffer which contains the client contents 
        printf("From client: %s\t To client : ", buff); 
        bzero(buff, MAX); 
        n = 0; 
        // copy server message in the buffer 
        while ((buff[n++] = getchar()) != '\n') 
            ; 
  
        // and send that buffer to client 
        write(sockfd, buff, sizeof(buff)); 
  
        // if msg contains "Exit" then server exit and chat ended. 
        if (strncmp("exit", buff, 4) == 0) { 
            printf("Server Exit...\n"); 
            break; 
        } 
    } 
} 

int main(){
	int sd;
	int acc_sd;
	struct sockaddr_in addr;
	struct sockaddr_in from_addr;
	socklen_t sin_size = sizeof(struct sockaddr_in);
	char buff[MAX],msg[MAX],from[MAX],txt[]="rec.txt";;
	FILE *fp;

	system("clear");
	// IPv4 TCP のソケットを作成
	if((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		return -1;
	}

    // 待ち受けるIPとポート番号を設定
	addr.sin_family = AF_INET;		// IPv4 インターネットプロトコル
	addr.sin_port = htons(SMTP);		// ポート番号
	addr.sin_addr.s_addr = INADDR_ANY;	// IPアドレス
						// INADDR_ANY : 全てのローカルインターフェイスにバインドされうる
	// バインドする
	if(bind(sd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		perror("bind");
		return -1;
	}
	// パケット受信待ち状態とする
	// 待ちうけキューを10としている
	if(listen(sd, 10) < 0) {
		perror("listen");
		return -1;
	}