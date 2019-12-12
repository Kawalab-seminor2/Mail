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
#define SERVER_IP "192.168.29.105"//5or6"127.0.0.1"
#define SMTP 1900
#define POP 1901

char from[MAX];

int smtp();
int pop();
int history();
int ex_fgets(char buff[],int size);
int ex_fprintf(FILE *fp,char format[],char buff[]);
int get_info(char ret[],char file[],int line);
int cut(char ret[],char data[],const char *token,int point);
int test(FILE *fp,char file[]);

int main(void){
	// 変数
	typedef struct list{
		int id;
		char *mode;
	} List;
	const List menu[]={{0,"終了"},{1,"送信"},{2,"受信"},{3,"履歴"}};
	int i,v;

	system("clear");
	printf("メールアドレスを入力してください\n > ");
	ex_fgets(from,MAX-1);
	while(1){
		printf("\nメニュー\n");
		for(i=0;i<4;i++){
			printf("%d : %s\n",menu[i].id,menu[i].mode);
		}
		printf(" > ");
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
			case 3:
				history();
				break;
			default:
				fprintf(stderr,"argument\n");
		}
	}
	return 0;
}

int smtp(){
	// 変数
	int sd;
	char buff[MAX],msg[MAX];
	char file[]="body.txt";
	FILE *fp;
	time_t t;
	struct sockaddr_in addr;

	// メールの作成
	if((fp = fopen(file,"w")) == NULL){
		fprintf(stderr,"ファイルのオープンに失敗しました\n");
		fclose(fp);
		return -1;
	}
	ex_fprintf(fp,"From;%s\n",from);

	printf("To\n > ");
	ex_fgets(buff,MAX-1);
	ex_fprintf(fp,"To;%s\n",buff);
	t=time(NULL);
	strftime(buff, sizeof(buff), "%Y/%m/%d %H:%M:%S", localtime(&t));
	ex_fprintf(fp,"Date;%s\n",buff);
	printf("Subject\n > ");
	fgets(buff, MAX-1, stdin);
	buff[strlen(buff) - 1] = '\0';
	ex_fprintf(fp,"Subject;%s\n",buff);
	memset(buff,0,sizeof(buff));
	printf("Body\n");
	while(strcmp(buff,".") != 0){
		printf(" > ");
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
	get_info(buff,file,2);
	cut(buff,buff,";@",2);
	send(sd, buff, sizeof(buff),0);
	// 本文
	if((fp = fopen(file,"r")) == NULL){
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
	remove(file);
	return 0;
}
int pop(){
	int sd,quantity,i;
	char buff[MAX],msg[MAX];
	FILE *fp;
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
	cut(buff,from,"@",1);
	send(sd, buff, sizeof(buff),0);
	// 新着メールの有無を取得
	for(i=0;;i++){
		recv(sd, msg, sizeof(msg), 0);
		if(strcmp(msg,"0") == 0){
			if(i==0) printf("新着メールがあります\n");
			test(fp,buff);
			if((fp = fopen(buff,"w")) == NULL){
				fprintf(stderr,"ファイルのオープンに失敗しました\n");
				return -1;
			}
			printf("%d通目\n",i+1);
			printf("-------------------------\n");
			while(strcmp(msg,".") != 0) {
				// 受信処理
				recv(sd, msg, sizeof(msg), 0);
				if(strcmp(msg,".") != 0){
					printf("%s\n",msg);
					strcpy(buff,"0");
				} else {
					printf("-------------------------\n");
					strcpy(buff,"1");
				}
				ex_fprintf(fp,"%s\n",msg);
				send(sd, buff, sizeof(buff),0);
			}
			fclose(fp);
		}else{
			if(i==0) printf("新着メールがありません\n");
			break;
		}
	}
	close(sd);
	printf("Disconnect from server\n\n");
	return 0;
}
int history(){
	int i,j,k,v;
	char buff[MAX],file[MAX],user[MAX];
	FILE *fp;

	cut(user,from,"@",1);
	for(i=0;;i++){
		sprintf(file, "get/%s/mail%d.txt",user,i+1);
		if((fp = fopen(file,"r")) == NULL){
			v=i;
			if(v==0){
				printf("受信済みメールがありません\n\n");
				return 0;
			}else{
				printf("\n%d 件受信済み\n",v);
				break;
			}
		}
	}
	while(1){
		printf("何件目を表示しますか?(0で終了) > ");
		scanf("%d%*c",&j);
		if(j>0&&j<=v) i=v-j+1;
		else if(j==0) return 0;
		else{
			printf("Err\n");
			return -1;
		}
		sprintf(file, "get/%s/mail%d.txt",user,i);
		if((fp = fopen(file,"r")) != NULL){
			printf("\n%d件目\n",v-i+1);
			printf("-------------------------\n");
			while (fgets(buff, MAX, fp)!=NULL) {
				if(strcmp(buff,".\n") != 0) printf("%s",buff);
				else printf("-------------------------\n\n");
			}
		}
	}
	return 0;
}
//空白を許さないfgets
int ex_fgets(char buff[],int size){
	do {
		if (fgets(buff, size, stdin) == NULL){
			buff='\0';
			return -1;
		}
		buff[strlen(buff) - 1] = '\0';
	} while (strcmp(buff, "\0") == 0);
	return 0;
}
//エラー処理付きfprintf
int ex_fprintf(FILE *fp,char format[],char buff[]){
	if(fprintf(fp,format,buff) < 0){
		fprintf(stderr,"ファイルの書き込みに失敗しました\n");
		return -1;
	}
	return 0;
}
//ファイルfileのline行目をretに格納する関数
int get_info(char ret[],char file[],int line){
	int i;
	char buff[MAX][MAX];
	FILE *fp;

	if((fp = fopen(file,"r")) == NULL){
		fprintf(stderr,"ファイルのオープンに失敗しました\n");
		fclose(fp);
		return -1;
	}
	for(i=0;fgets(buff[i], sizeof(buff[i]), fp) != NULL;i++);
	fclose(fp);
	cut(buff[line-1],buff[line-1],"\n",1);
	strcpy(ret,buff[line-1]);
	return 0;
}
//文字列dataを区切り文字tokenで区切った時の、point個目の要素をretに格納する関数
int cut(char ret[],char data[],const char *token,int point){
	int i=0;
	char buff[MAX],*tp,*argv[MAX];

	strcpy(buff,data);
	tp=strtok(buff,token);
	argv[i]=tp;
	for(i=1;i<MAX && tp!=NULL;i++) {
		tp=strtok(NULL,token);
		if(tp!=NULL) argv[i]=tp;
	}
	strcpy(ret,argv[point-1]);
	return 0;
}
int test(FILE *fp,char file[]){
	int i;
	char buff[MAX];

	cut(buff,from,"@",1);
	for(i=0;;i++){
		sprintf(file, "get/%s/mail%d.txt",buff,i+1);
		if((fp = fopen(file,"r")) == NULL) break;
	}
	return 0;
}
