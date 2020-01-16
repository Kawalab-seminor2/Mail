#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>


#define FALSE 0
#define TRUE 1
#define MAX 64
#define SERVER_IP "192.168.29.105"
#define SMTP 1900
#define POP 1901

char from[MAX];
int debug=FALSE; //TRUE or FALSE

int smtp();
int pop();
int history();
int fgets_or(char buff[],int size);
int fprintf_or(FILE *fp,char format[],char buff[]);
int get_info(char ret[],char file[],int line);
int cut(char ret[],char data[],const char *token,int point);
int wait_ent(void);

int main(void){
	// 変数
	typedef struct list{
		int id;
		char *mode;
	} List;

	int i,val;
	char buff[MAX];
	const List menu[]={{1,"送信"},{2,"受信"},{3,"受信箱"},{0,"終了"}};

	system("clear");
	printf("メールアドレスを入力してください\n > ");
	fgets_or(from,MAX-1);
	while(1){
		system("clear");
		printf("Hello %s\n\nメニュー\n",from);
		for(i=0;i<(int)(sizeof(menu)/sizeof(menu[0]));i++) printf("%d : %s	",menu[i].id,menu[i].mode);
		printf("\n > ");
		fgets(buff, MAX, stdin);
		val=atoi(buff);
		switch(val){
			case 1:
				smtp();
				break;
			case 2:
				pop();
				break;
			case 3:
				history();
				break;
			case 0:
				printf("\nGoodbye %s\n",from);
				return 0;
			default:
				fprintf(stderr,"\n無効な数値が指定されました\n");
				wait_ent();
		}
	}
	return 0;
}

int smtp(){
	// 変数
	int sd;
	char buff[MAX],msg[MAX],file[]="mail0.txt";
	FILE *fp;
	time_t t;
	struct sockaddr_in addr;

	system("clear");
	// メールの作成
	if((fp = fopen(file,"w")) == NULL){
		fprintf(stderr,"ファイルのオープンに失敗しました\n");
		wait_ent();
		return -1;
	}
	fprintf_or(fp,"From;%s\n",from);
	printf("宛先\n > ");
	fgets_or(buff,MAX-1);
	fprintf_or(fp,"To;%s\n",buff);
	printf("Cc\n > ");
	fgets_or(buff,MAX-1);
	fprintf_or(fp,"Cc;%s\n",buff);
	t=time(NULL);
	strftime(buff, sizeof(buff), "%Y/%m/%d %H:%M:%S", localtime(&t));
	fprintf_or(fp,"Date;%s\n",buff);
	printf("件名\n > ");
	fgets(buff, MAX-1, stdin);
	buff[strlen(buff) - 1] = '\0';
	fprintf_or(fp,"Subject;%s\n",buff);
	memset(buff,0,sizeof(buff));
	printf("本文('.'で終了)\n");
	while(strcmp(buff,".") != 0){
		printf(" > ");
		fgets(buff, MAX-1, stdin);
		buff[strlen(buff) - 1] = '\0';
		fprintf_or(fp,"%s\n",buff);
	}
	fclose(fp);
	// 送受信バッファの初期化
	memset(buff,0,sizeof(buff));
	memset(msg,0,sizeof(buff));
	// IPv4 TCP のソケットを作成する
	if((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		wait_ent();
		return -1;
	}
	// 送信先アドレスとポート番号を設定する
	addr.sin_family = AF_INET;
	addr.sin_port = htons(SMTP);
	addr.sin_addr.s_addr = inet_addr(SERVER_IP);
	// サーバ接続
	if(connect(sd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) < 0){
		perror("connect");
		wait_ent();
		return -1;
	}
	// 送信処理
	if(debug==TRUE) printf("\nConnect to server\n");
	// ユーザー情報
	get_info(buff,file,2);
	cut(buff,buff,";@",2);
	send(sd, buff, sizeof(buff),0);
	// 本文
	if((fp = fopen(file,"r")) == NULL){
		fprintf(stderr,"ファイルのオープンに失敗しました\n");
		close(sd);
		wait_ent();
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
	if(debug==TRUE) printf("\nDisconnect from server\n");
	remove(file);
	printf("\n送信完了\n");
	wait_ent();
	return 0;
}
int pop(){
	int sd,i,j;
	char buff[MAX],msg[MAX],file[MAX],user[MAX];
	FILE *fp;
	struct sockaddr_in addr;

	system("clear");
	// 送受信バッファの初期化
	memset(buff,0,sizeof(buff));
	memset(msg,0,sizeof(buff));
	// IPv4 TCP のソケットを作成する
	if((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		wait_ent();
		return -1;
	}
	// 送信先アドレスとポート番号を設定する
	addr.sin_family = AF_INET;
	addr.sin_port = htons(POP);
	addr.sin_addr.s_addr = inet_addr(SERVER_IP);
	// サーバ接続
	if(connect(sd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) < 0){
		perror("connect");
		wait_ent();
		return -1;
	}
	// 受信処理
	if(debug==TRUE) printf("\nConnect to server\n");
	// ユーザー情報
	cut(user,from,"@",1);
	send(sd, user, sizeof(user),0);
	// 新着メールの有無を取得
	for(i=0;;i++){
		recv(sd, msg, sizeof(msg), 0);
		if(strcmp(msg,"0") == 0){
			if(i==0) printf("新着メールがあります\n");
			for(j=0;;j++){
				sprintf(file, "%s/mail%d.txt",user,j+1);
				if((fp = fopen(file,"r")) == NULL) break;
			}
			if((fp = fopen(file,"w")) == NULL){
				fprintf(stderr,"ファイルのオープンに失敗しました\n");
				close(sd);
				wait_ent();
				return -1;
			}
			printf("\n%d通目\n",i+1);
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
				fprintf_or(fp,"%s\n",msg);
				send(sd, buff, sizeof(buff),0);
			}
			fclose(fp);
		}else{
			if(i==0) printf("新着メールがありません\n");
			else printf("\nすべてのメールを受信しました\n");
			wait_ent();
			break;
		}
	}
	close(sd);
	if(debug==TRUE) printf("\nDisconnect from server\n");
	return 0;
}
int history(){
	typedef struct list{
		char *mode;
		char *format;
	} List;

	int amount,val,i;
	char buff[MAX],file[MAX],user[MAX],mode[MAX],old[MAX],new[MAX];;
	const List menu[]={{"表示","cat n"},{"削除","rm n"},{"終了","0"}};
	FILE *fp;

	cut(user,from,"@",1);
	while(1){
		system("clear");
		// 受信フォルダ内のメール数をカウント
		for(i=0;;i++){
			sprintf(file, "%s/mail%d.txt",user,i+1);
			if((fp = fopen(file,"r")) == NULL) break;
		}
		amount=i;
		if(amount!=0){
			// 受信リスト表示
			printf("受信フォルダ\n");
			for(i=0;i<amount;i++){
				sprintf(file, "%s/mail%d.txt",user,amount-i);
				if((fp = fopen(file,"r")) == NULL){
					fprintf(stderr,"ファイルのオープンに失敗しました\n");
					wait_ent();
					return -1;
				}
				get_info(buff,file,1);
				cut(buff,buff,";",2);
				printf("%2d件目   送信元 : %-20s",i+1,buff);
				get_info(buff,file,4);
				cut(buff,buff,";",2);
				printf("	件名   : %s\n",buff);
				fclose(fp);
			}
			// メニュー表示
			printf("\n操作\n");
			for(i=0;i<(int)(sizeof(menu)/sizeof(menu[0]));i++) printf("%s : %s	",menu[i].mode,menu[i].format);
			printf("\n > ");
			fgets_or(buff, MAX);
			cut(mode,buff," \n",1);
			cut(buff,buff," \n",2);
			val=amount-atoi(buff)+1;
			// メール表示
			if(strcmp(mode,"cat") == 0){
				if(val>0 && val<=amount){
					sprintf(file, "%s/mail%d.txt",user,val);
					if((fp = fopen(file,"r")) == NULL){
						fprintf(stderr,"ファイルのオープンに失敗しました\n");
						wait_ent();
						return -1;
					}
					printf("\n-------------------------\n");
					while (fgets(buff, MAX, fp)!=NULL) {
						if(strcmp(buff,".\n") != 0) printf("%s",buff);
						else printf("-------------------------\n");
					}
					fclose(fp);
				}
				else fprintf(stderr,"\n無効な数値が指定されました\n");
			}
			// メール削除
			else if(strcmp(mode,"rm") == 0){
				if(val>0 && val<=amount){
					sprintf(file, "%s/mail%d.txt",user,val);
					remove(file);
					for(i=val;i<amount;i++){
						sprintf(old, "%s/mail%d.txt",user,i+1);
						sprintf(new, "%s/mail%d.txt",user,i);
						if(rename(old, new)!=0) fprintf(stderr,"リネームに失敗しました\n");
					}
				}
				else fprintf(stderr,"\n無効な数値が指定されました\n");
			}
			else if(strcmp(mode,"0") == 0) return 0;
			else fprintf(stderr,"\n無効なコマンドが入力されました\n");
		}
		else{
			printf("受信済みメールがありません\n");
			wait_ent();
			return 0;
		}
		wait_ent();
	}
}
//空白を許さないfgets
int fgets_or(char buff[],int size){
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
int fprintf_or(FILE *fp,char format[],char buff[]){
	if(fprintf(fp,format,buff) < 0){
		fprintf(stderr,"ファイルの書き込みに失敗しました\n");
		fclose(fp);
		wait_ent();
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
		wait_ent();
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
	int len=1,i=0;
	char *tp,buff[MAX],*argv[MAX];

	strcpy(buff,data);
	tp=strtok(buff,token);
	argv[i]=tp;
	for(i=1;i<MAX && tp!=NULL;i++) {
		tp=strtok(NULL,token);
		if(tp!=NULL){
			argv[i]=tp;
			len++;
		}
	}
	if(point-1<len) strcpy(ret,argv[point-1]);
	else strcpy(ret,"");
	return 0;
}
// Enterが入力されるまで待機する関数
int wait_ent(void){
	char buff[MAX];

	printf("\nEnterを押して継続");
	fgets(buff, sizeof(buff), stdin);
	return 0;
}
