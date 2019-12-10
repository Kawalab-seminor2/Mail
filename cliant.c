#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>

#define MAX 64
#define SERVER_IP "127.0.0.1" //local loopback
#define SMTP 1900
#define POP 1901

char from[MAX];

int smtp();
int pop();
int ex_fgets(char buff[],int size);
int ex_fprintf(FILE *fp,char format[],char buff[]);
int test(char txt[],FILE *fp);

int main(void){
	// 変数
	typedef struct list{
		int id;
		char *mode;
	} List;
	const List menu[]={{0,"Exit"},{1,"Send"},{2,"Pop"}};
	int i,v;

	system("clear");
	printf("メールアドレスを入力してください\n > ");
	ex_fgets(from,MAX-1);
	while(1){
		printf("メニュー\n");
		for(i=0;i<3;i++){
			printf("%d : %s\n",menu[i].id,menu[i].mode);
		}
		printf(" > ");
		fflush(stdout);
		scanf("%d%*c",&v);
		switch(v){
			case 0:
				printf("Terminate the process\n");
				return 0;
			case 1:
				smtp();
				break;
			case 2:
				pop();
				break;
			default:
				fprintf(stderr,"argument\n");
				return -1;
		}
	}
	return 0;
}

int smtp(){
	// 変数
	int sd;
	char buff[MAX],msg[MAX];
	char txt[]="mail.txt";
	FILE *fp;
	time_t t;
	struct sockaddr_in addr;

	// メールの作成
	if((fp = fopen(txt,"w")) == NULL){
		fprintf(stderr,"ファイルのオープンに失敗しました\n");
		fclose(fp);
		return -1;
	}
	ex_fprintf(fp,"From   ;%s\n",from);
	printf("To\n > ");
	ex_fgets(buff,MAX-1);
	ex_fprintf(fp,"To     ;%s\n",buff);
	t=time(NULL);
	strftime(buff, sizeof(buff), "%Y/%m/%d %H:%M:%S", localtime(&t));
	ex_fprintf(fp,"Date   ;%s\n",buff);
	printf("Subject\n > ");
	fflush(stdout);
	fgets(buff, MAX-1, stdin);
	buff[strlen(buff) - 1] = '\0';
	ex_fprintf(fp,"Subject;%s\n",buff);
	memset(buff,0,sizeof(buff));
	printf("Body\n");
	while(strcmp(buff,".") != 0){
		printf(" > ");
		fflush(stdout);
		fgets(buff, MAX-1, stdin);
		buff[strlen(buff) - 1] = '\0';
		ex_fprintf(fp,"%s\n",buff);
	}
	fclose(fp);
	// 送受信バッファの初期化
	memset(buff,0,sizeof(buff));
	memset(msg,0,sizeof(buff));
	// IPv4 TCP のソケットを作成する
	if((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		return -1;
	}
	// 送信先アドレスとポート番号を設定する
	addr.sin_family = AF_INET;
	addr.sin_port = htons(SMTP);
	addr.sin_addr.s_addr = inet_addr(SERVER_IP);
	// サーバ接続
	if(connect(sd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) < 0){
		perror("connect");
		return -1;
	}
	// 送信処理
	printf("\nConnect to server\n");
	// ユーザー情報
	send(sd, from, sizeof(from),0);
	recv(sd, msg, sizeof(msg),0);
	// 本文
	if((fp = fopen(txt,"r")) == NULL){
		fprintf(stderr,"ファイルのオープンに失敗しました\n");
		fclose(fp);
		return -1;
	}
	while (strcmp(msg,"1") != 0) {
		fgets(buff, MAX-1, fp);
		buff[strlen(buff) - 1] = '\0';
		send(sd, buff, sizeof(buff),0);
		recv(sd, msg, sizeof(msg),0);
	}
	fclose(fp);
	close(sd);
	printf("Disconnect from server\n\n");
	remove(txt);
	return 0;
}
int pop(){
	int sd;
	char buff[MAX],msg[MAX];
	struct sockaddr_in addr;

	// 送受信バッファの初期化
	memset(buff,0,sizeof(buff));
	memset(msg,0,sizeof(buff));
	// IPv4 TCP のソケットを作成する
	if((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		return -1;
	}
	// 送信先アドレスとポート番号を設定する
	addr.sin_family = AF_INET;
	addr.sin_port = htons(POP);
	addr.sin_addr.s_addr = inet_addr(SERVER_IP);
	// サーバ接続
	if(connect(sd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) < 0){
		perror("connect");
		return -1;
	}
	// 受信処理
	printf("\nConnect to server\n");
	// ユーザー情報
	send(sd, from, sizeof(from),0);
	// 受信処理
	printf("-------------------------\n");
	while(strcmp(msg,".") != 0) {
		recv(sd, msg, sizeof(msg), 0);
		if(strcmp(msg,".") == 0){
			printf("-------------------------\n");
			strcpy(buff,"1");
		} else {
			printf("%s\n",msg);
			strcpy(buff,"0");
		}
		send(sd, buff, sizeof(buff),0);
	}
	close(sd);
	printf("Disconnect from server\n\n");
	return 0;
}
int ex_fgets(char buff[],int size){
	do {
		fflush(stdout);
		if (fgets(buff, size, stdin) == NULL){
			buff='\0';
			return -1;
		}
		buff[strlen(buff) - 1] = '\0';
	} while (strcmp(buff, "\0") == 0);
	return 0;
}
int ex_fprintf(FILE *fp,char format[],char buff[]){
	if(fprintf(fp,format,buff) < 0){
		fprintf(stderr,"ファイルの書き込みに失敗しました\n");
		fclose(fp);
		return -1;
	}
	return 0;
}
int test(char txt[],FILE *fp){
	char buff[MAX];

	if((fp = fopen(txt,"r")) == NULL){
		fprintf(stderr,"ファイルのオープンに失敗しました\n");
		fclose(fp);
		return -1;
	}
	printf("-------------------------\n");
	while(fgets(buff, sizeof(buff), fp) != NULL) {
		if(strcmp(buff,".\n") != 0) printf("%s", buff);
	}
	printf("-------------------------\n");
	fclose(fp);
	return 0;
}
