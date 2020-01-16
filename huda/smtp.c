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

int main(){
	int sd, acc_sd, i;
	unsigned int len;
	int sent;
	struct sockaddr_in server, client;
	char buff[MAX], msg[MAX], to[MAX], txt[]="rec.txt", file_dir[MAX];
	FILE *fp;

	system("clear");
	// IPv4 TCP のソケットを作成
    	if((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		perror("socket: ");
        	return -1;
    	}
	// 待ち受けるIPとポート番号を設定
	server.sin_family = AF_INET;		// IPv4 インターネットプロトコル
	server.sin_port = htons(SMTP);		// ポート番号
	server.sin_addr.s_addr = INADDR_ANY;	// IPアドレス
						// INADDR_ANY : 全てのローカルインターフェイスにバインドされうる
	// バインドする
	len = sizeof(struct sockaddr_in);
	if(bind(sd, (struct sockaddr *)&server, sizeof(server)) < 0){
		perror("bind");
		return -1;
	}

	// パケット受信待ち状態とする
	// 待ちうけキューを10としている
	if((listen(sd, 10)) < 0){
		perror("listen");
		return -1;
	}

	while(1){
	// 受信バッファの初期化
		memset(buff, 0, sizeof(buff));
		memset(msg, 0, sizeof(msg));
		memset(to, 0, sizeof(to));
	// クライアントからコネクト要求が来るまで停止する
	// 以降、サーバ側は acc_sd を使ってパケットの送受信を行う
		printf("Waiting for connection\n");
		if((acc_sd = accept(sd, (struct sockaddr *)&client, &len)) < 0){
			perror("accept");
			break;
		}
	// パケット受信。パケットが到着するまでブロック
		recv(acc_sd, to, sizeof(msg), 0);
		printf("Connection request to \"%s\" \n\n",to);
		for(i=0;;i++){
			sprintf(file_dir,"%s/mail%d.txt", to, i+1);
			if ((fp = fopen(file_dir, "r")) == NULL) break;
		}
		fp = fopen(file_dir, "w");
		while(strcmp(msg,".") != 0){
			recv(acc_sd, msg, sizeof(msg),0);
			if(strcmp(msg,".") == 0){
				strcpy(buff, "1");
			}else{
				strcpy(buff, "0");
			}
			fprintf(fp, "%s\n", msg);
			send(acc_sd, buff, sizeof(buff), 0);
		}
		fclose(fp);
		// パケット送受信用ソケットのクローズ
		close(acc_sd);
	}
	// 接続要求待ち受け用ソケットをクローズ
	close(sd);
	return 0;
}  