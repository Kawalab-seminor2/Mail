#include <stdio.h>
#include <sys/stat.h>
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
#define ACC 1902

char from[MAX];
int debug=FALSE; //TRUE or FALSE

int account();
int smtp();
int pop();
int history();
int forward();
int fgets_or(char buff[],int size);
int fprintf_or(FILE *fp,char format[],char buff[]);
int get_info(char ret[],char file[],int line);
int get_info_for(char ret[],char file[],int line);
int cut(char ret[],char data[],const char *token,int point);
int wait_ent(void);


int main(void){
	// 変数
	typedef struct list{
		int id;
		char *mode;
	} List;

	int i,val,flag=-1;
	char buff[MAX];

	const List menu[]={{1,"送信"},{2,"受信"},{3,"受信箱"},{0,"終了"}};

	system("clear");
	printf("メールアドレスを入力してください\n > ");
	fgets_or(from,MAX-1);
	
	//アカウントの認証
	while(1){
	printf("%d",flag);
	flag=account();
    if(flag==0)break;
    }
	
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

int account(){//アカウントの認証
    //変数
    int val,i,sd,val1,ln,ln1;
    typedef struct list{
        int id;
        char *mode;
    } List;
    char file[]="account.txt";
    char buff[MAX],msg[MAX];
    char cli_name[MAX];
    char dir_nm[MAX];
    struct sockaddr_in addr;
    const List menu[]={{1,"ログイン"},{2,"登録"},{3,"アカウント管理"},{0,"終了"}};
    const List menu1[]={{1,"アカウント一覧"},{2,"アカウント削除"},{0,"終了"}};
    FILE *fp;
    
    
        system("clear");
        printf("Hello %s\n\nメニュー\n",from);
        
        //認証に関わるソケット通信の準備
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
        addr.sin_port = htons(ACC);
        addr.sin_addr.s_addr = inet_addr(SERVER_IP);

        // サーバ接続///////////////
        if(connect(sd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) < 0){
            perror("connect");
            wait_ent();
            return -1;
        }
        
        send(sd, from, sizeof(from),0);//アドレスの送信//ここまでは共通
        
        //認証開始
        for(i=0;i<(int)(sizeof(menu)/sizeof(menu[0]));i++) printf("%d : %s ",menu[i].id,menu[i].mode);
        printf("\n > ");
        fgets(buff, MAX, stdin);
        val=atoi(buff);
    
        switch(val){
            case 1://ログイン
                send(sd, buff, sizeof(buff),0);
                printf("パスワードを入力してください\n>");
                fgets_or(buff,MAX-1);
                //printf("%s\n",buff);
                send(sd, buff, sizeof(buff),0);
                recv(sd, buff, sizeof(msg), 0);
                //printf("%s",buff);
                if(strcmp(buff,"1")==0){
                    printf("認証に成功しました\n");
                    sleep(2);
                    return 0;
                }else if(strcmp(buff,"2")==0){
                    printf("パスワードが違います\n");
                    sleep(2);
                    return -1;
                }else{
                    printf("アカウントを登録してください\n");
                    sleep(2);
                    exit(1);
                }
                break;
                
            case 2://登録
                send(sd, buff, sizeof(buff),0);
                recv(sd, buff, sizeof(msg), 0);
                
                if(strcmp(buff,"1")==0){
                    printf("登録するパスワードを入力してください\n>");
                    fgets_or(buff,MAX-1);
                    send(sd, buff, sizeof(buff),0);
                    recv(sd, buff, sizeof(msg), 0);
                    sprintf(cli_name,"client%s",from);
                    if(mkdir(cli_name,0777)==0){ //_mkdir("name")に変更
                        printf("ディレクトリ%s作成に成功\n",cli_name);
                        return 0;
                    }else{
                        printf("ディレクトリ1作成できませんでした\n");
                        return -1;
                        
                    }
                    if(strcmp(buff,"1")==0)printf("アカウント作成に成功しました\n");
                    else printf("エラーが起こりました\n");
                    return -1;
                }else if(strcmp(buff,"2")==0){
                    printf("アカウントが存在するためアドレスを変更してください\n");
                    sleep(2);
                    return -1;
                }
                break;
                
            case 3://アカウント管理
                send(sd, buff, sizeof(buff),0);
                printf("管理者パスワードを入力してください\n>");
                fgets_or(buff,MAX-1);
                printf("%s\n",buff);
                send(sd, buff, sizeof(buff),0);
                recv(sd, buff, sizeof(msg), 0);
                if(strcmp(buff,"1") == 0){//管理者の動作
                    system("clear");
                    printf("ようこそ管理者\n\nメニュー\n");
                    
                    for(i=0;i<(int)(sizeof(menu1)/sizeof(menu1[0]));i++) printf("%d : %s ",menu1[i].id,menu1[i].mode);
                    printf("\n > ");
                    fgets(buff, MAX, stdin);
                    val1=atoi(buff);
                    switch(val1){
                        case 1://一覧表示
                            send(sd, buff, sizeof(buff),0);
                            recv(sd, buff, sizeof(msg), 0);
                            while(strcmp(buff,".")!=0){
                                printf("%s\n",buff);
                                recv(sd, buff, sizeof(msg), 0);
                            }
                            wait_ent();
                            return -1;
                            
                        case 2://アカウント削除
                            send(sd, buff, sizeof(buff),0);
                            recv(sd, buff, sizeof(msg), 0);
                            ln=1;
                            while(strcmp(buff,".")!=0){
                                printf("%d: %s\n",ln,buff);
                                recv(sd, buff, sizeof(msg), 0);
                                ln++;
                            }
                            printf("\n削除するアカウントの番号を入力してください\n");
                            fgets_or(buff,MAX-1);
                            ln1=atoi(buff);
                            
                            if(ln1<ln&&ln1>0&&ln1!=1){
                            send(sd,buff,sizeof(buff),0);
                            recv(sd,dir_nm, sizeof(msg), 0);//dir_nmに削除するディレクトリ名を代入
                            if(rmdir(dir_nm)==0){ //_mkdir("name")に変更
                                printf("%sのディレクトリ削除に成功\n",dir_nm);
                            }else{
                                printf("%sのディレクトリを削除できませんでした\n",dir_nm);
                            }
                            }else {
                                printf("正しく番号を入力してください\n");
                                sleep(2);
                                return -1;
                            }
                            
                            
                        case 0://終了
                            printf("終了します\n");
                            exit(1);
                    }
                }else{
                    printf("%s",buff);
                    printf("アドレスまたはパスワードが間違っています\n");
                    sleep(2);
                    return -1;
                }
                
            case 0://終了
                printf("\nGoodbye %s\n",from);
                exit(0);
            default:
                fprintf(stderr,"\n無効な数値が指定されました\n");
                exit(1);
        }
}

int smtp(){
	// 変数
	int sd;
	char buff[MAX],msg[MAX],file[MAX];
	FILE *fp;
	time_t t;
	struct sockaddr_in addr;

	system("clear");
	sprintf(file,"mail0%s.txt",from);
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
	if(debug==TRUE) printf("\nDisconnect from server\n");
	//Cc
	// 送受信バッファの初期化
	memset(buff,0,sizeof(buff));
	memset(msg,0,sizeof(buff));
	// ユーザー情報
	get_info(buff,file,3);
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
				sprintf(file, "client%s/mail%d.txt",user,j+1);
				if((fp = fopen(file,"r")) == NULL) break; //すでにファイルが存在するか？
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
	const List menu[]={{"表示","cat n"},{"削除","rm n"},{"転送","for n"},{"終了","0"}};
	FILE *fp;

	cut(user,from,"@",1);
	while(1){
		system("clear");
		// 受信フォルダ内のメール数をカウント
		for(i=0;;i++){
			sprintf(file, "client%s/mail%d.txt",user,i+1);
			if((fp = fopen(file,"r")) == NULL) break;
		}
		amount=i;
		if(amount!=0){
			// 受信リスト表示
			printf("受信フォルダ\n");
			for(i=0;i<amount;i++){
				sprintf(file, "client%s/mail%d.txt",user,amount-i);
				if((fp = fopen(file,"r")) == NULL){
					fprintf(stderr,"ファイルのオープンに失敗しました\n");
					wait_ent();
					return -1;
				}
				get_info(buff,file,1);
				cut(buff,buff,";",2);
				printf("%2d件目   送信元 : %-15s\n",i+1,buff);
				get_info(buff,file,5);
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
					sprintf(file, "client%s/mail%d.txt",user,val);
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
					sprintf(file, "client%s/mail%d.txt",user,val);
					remove(file);
					printf("メールを削除しました\n");
					for(i=val;i<amount;i++){
						sprintf(old, "client%s/mail%d.txt",user,i+1);
						sprintf(new, "client%s/mail%d.txt",user,i);
						if(rename(old, new)!=0) fprintf(stderr,"リネームに失敗しました\n");
					}
				}
				else fprintf(stderr,"\n無効な数値が指定されました\n");
			}
			// メール転送
			else if(strcmp(mode,"for") == 0){
				if(val>0 && val<=amount){
					sprintf(file, "client%s/mail%d.txt",user,val);
					forward(file);
				
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

//メール転送
int forward(char readfile[]){
int sd,i=7;
	char buff[MAX],msg[MAX],file[MAX];
	FILE *fp;
	time_t t;
	struct sockaddr_in addr;
	
	

	system("clear");
	sprintf(file,"mail0%s.txt",from);
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
	get_info_for(buff,readfile,1);
	buff[strlen(buff) - 1] = '\0';
	fprintf_or(fp,"作成者%s\n",buff);
	get_info_for(buff,readfile,5);
	buff[strlen(buff) - 1] = '\0';
	fprintf_or(fp,"%s\n",buff);
	memset(buff,0,sizeof(buff));
		get_info_for(buff,readfile,6);
		buff[strlen(buff) - 1] = '\0';
		fprintf_or(fp,"%s\n",buff);
		//printf("number:6 %s\n",buff);
		while(strcmp(buff,".") != 0){
		get_info_for(buff,readfile,i);
		buff[strlen(buff) - 1] = '\0';
		fprintf_or(fp,"%s\n",buff);
		//printf("mumber:%d %s\n ",i,buff);
		i++;
	}
		
	/*fprintf_or(fp,"%s\n",main);*/
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
	if(debug==TRUE) printf("\nDisconnect from server\n");
	//Cc
	// 送受信バッファの初期化
	memset(buff,0,sizeof(buff));
	memset(msg,0,sizeof(buff));
	// ユーザー情報
	get_info(buff,file,3);
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
	return 0;
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
	//printf("%s\n",ret); debug
	return 0;
}

int get_info_for(char ret[],char file[],int line){
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
	strcpy(ret,buff[line-1]);
	//rintf("%s\n",ret); //debug
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
	//printf("%s\n",ret); debug
	return 0;
}
// Enterが入力されるまで待機する関数
int wait_ent(void){
	char buff[MAX];

	printf("\nEnterを押して継続");
	fgets(buff, sizeof(buff), stdin);
	return 0;
}
