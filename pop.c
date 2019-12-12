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
#define SERVER_IP "172.0.0.1" //local loopback
#define SMTP 1900
#define POP 1901

int main(){
	int sd,quantity,i;
	int acc_sd;
	struct sockaddr_in addr;
	struct sockaddr_in from_addr;
	socklen_t sin_size = sizeof(struct sockaddr_in);
	char buff[MAX],msg[MAX],from[MAX],file_dir[MAX];
	FILE *fp;

	system("clear");
	// IPv4 TCP のソケットを作成
	if((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		return -1;
	}
	// 待ち受けるIPとポート番号を設定
	addr.sin_family = AF_INET;		// IPv4 インターネットプロトコル
	addr.sin_port = htons(POP);		// ポート番号
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
	while(1){
	// 受信バッファの初期化
		memset(buff, 0, sizeof(buff));
		memset(msg, 0, sizeof(msg));
		memset(from, 0, sizeof(from));
	// クライアントからコネクト要求が来るまで停止する
	// 以降、サーバ側は acc_sd を使ってパケットの送受信を行う
		printf("Waiting for connection\n");
		if((acc_sd = accept(sd, (struct sockaddr *)&from_addr, &sin_size)) < 0) {
			perror("accept");
			break;
		}
	// パケット受信。パケットが到着するまでブロック
		recv(acc_sd, from, sizeof(from), 0);
		printf("Connection request from \"%s\"\n",from);
		for(i=0;;i++){
			sprintf(file_dir, "%s/mail%d.txt",from,i+1);
			if((fp = fopen(file_dir,"r")) != NULL){
				strcpy(buff,"0");
				send(acc_sd, buff, sizeof(buff),0);
				while(strcmp(msg,"1") != 0) {
					fgets(buff, MAX-1, fp);
					buff[strlen(buff) - 1] = '\0';
					send(acc_sd, buff, sizeof(buff),0);
					recv(acc_sd, msg, sizeof(msg), 0);
				}
				remove(file_dir);
			}else{
				if(i==0) printf("There aren't new messages to %s\n",from);
				strcpy(buff,"1");
				send(acc_sd, buff, sizeof(buff),0);
				break;
			}
			memset(msg, 0, sizeof(msg));
			fclose(fp);
		}
	// パケット送受信用ソケットのクローズ
		close(acc_sd);
		printf("Disconnect from \"%s\" \n\n",from);
	}
	// 接続要求待ち受け用ソケットをクローズ
	close(sd);
	return 0;
}
