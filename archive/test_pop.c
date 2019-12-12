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
	int sd;
	int acc_sd;
	struct sockaddr_in addr;
	struct sockaddr_in from_addr;
	socklen_t sin_size = sizeof(struct sockaddr_in);
	char buff[MAX],msg[MAX],from[MAX],txt[]="rec.txt";
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
		
		//if((fp = fopen(txt,"r")) == NULL){
			//fprintf(stderr,"ファイルのオープンに失敗しました\n");
			//fclose(fp);
			//break;
		//}
		recv(acc_sd, from, sizeof(from), 0);
		send(acc_sd, from, sizeof(from), 0);//ファイルの個数
	
		while()
		printf("Connection request from \"%s\"\n",from);
		
		
		//while(strcmp(msg,"1") != 0) {
			//fgets(buff, MAX-1, fp);
			//buff[strlen(buff) - 1] = '\0';
			//send(acc_sd, buff, sizeof(buff),0);
			//recv(acc_sd, msg, sizeof(msg), 0);
		//}
		fclose(fp);
	// パケット送受信用ソケットのクローズ
		close(acc_sd);
		printf("Disconnect from \"%s\" \n\n",from);
	}
	// 接続要求待ち受け用ソケットをクローズ
	close(sd);
	return 0;
}

    #define N 256 // 1行の最大文字数(バイト数)
     
    int pop(void) {
    	FILE *fp; // FILE型構造体
    	char fname[16],fname2[]=".txt";
    	char str[N];
		char fi[2]="1";

	while(1){
		strcpy(fname,"UserA/test");
		strcat(fname,fi);
		strcat(fname,fname2);

		fp = fopen(fname, "r"); // ファイルを開く。失敗するとNULLを返す。
		if(fp == NULL) {
    		printf("%s file not open!\n", fname);
    		return 1;
    		}
     
    		while(fgets(str, N, fp) != NULL) {
    			send(acc_sd, str,sizeof(str),0);
    		}
     
    		fclose(fp); // ファイルを閉じる
    		remove(fname);
    		fi[0]+=1;
     	}
    	return 1;
    }

