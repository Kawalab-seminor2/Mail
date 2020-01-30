//ac-sv.c
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define ERR -1
#define MAX 64
#define SERVER_IP "192.168.29.105" //local loopback
#define SMTP 1900
#define POP 1901
#define ACC 1902

int main(){
    int sock, cli, k,i,val,val1,ac_flag=0,ln=0;
    unsigned int len;
    int sent;
    struct sockaddr_in server, client;
    char buff[MAX], msg[MAX], to[MAX],file[MAX]="account.txt";
    FILE *fp;
    pid_t child_pid;
    char txt_addr[MAX],txt_pass[MAX];
    struct Account{
        char addr[MAX];
        char pass[MAX];
    }Account;
    char p_id[]="addr: ";
    char p_pass[]=" , pass: ";
    char snd_msg[MAX];
    char arr[MAX][MAX];
    char dirnm[MAX];
    char rmdirnm[MAX];
    
    
    //ソケットの作成
    if((sock = socket(AF_INET, SOCK_STREAM, 0)) == ERR)
    {
        perror("socket: ");
        exit(-1);
    }
    
    
    server.sin_family = AF_INET;
    server.sin_port = htons(ACC);
    server.sin_addr.s_addr = INADDR_ANY;
    
    //bind
    len = sizeof(struct sockaddr_in);
    int yes=1;
    
    if(setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,(const char *)&yes,sizeof(yes))<0){
    perror("ERROR on setsocketopt");
    exit(1);
    }
    
    if(bind(sock, (struct sockaddr *)&server, sizeof(server)) == ERR)
    {
        perror("bind");
        exit(-1);
    }
    
    //listen
    if((listen(sock, 10)) == ERR)
    {
        perror("listen");
        exit(-1);
    }

        memset(buff, 0, sizeof(buff));
        memset(msg, 0, sizeof(msg));
        memset(to, 0, sizeof(to));
        
        while(1){
        printf("waiting for connection\n");
        //accept
        if((cli = accept(sock, (struct sockaddr *)&client, &len)) == ERR)
        {
            perror("accept");
            exit(-1);
        }
        //create child process
		if ((child_pid = fork()) < 0){
			perror("fork");
			exit(-1);
		}
		//child
		else if (child_pid== 0){
		close(sock);	//close child copy
        //receive from client
        recv(cli, to, sizeof(msg), 0);
        for (i=0;i<sizeof(Account.addr); i++) {//account.addrにアドレスを代入
            Account.addr[i]=to[i];
        }
        printf("%s",Account.addr);
        printf("connection request to %s\n",to);
        
        recv(cli, to, sizeof(msg), 0);
        val=atoi(to);
        
        switch(val){
            case 1://ログイン
                printf("request login\n");
                recv(cli, Account.pass, sizeof(msg), 0);
                if ((fp = fopen(file, "r")) == NULL) break;
                while(1){
                if(fscanf(fp,"%s %s",txt_addr,txt_pass)!=EOF){
                    printf("%s %s %s %s\n",txt_addr,Account.addr,txt_pass,Account.pass);
                    
                    if(strcmp(txt_addr,Account.addr)==0){
                        if(strcmp(txt_pass,Account.pass)==0){
                            printf("認証成功\n");
                            send(cli,"1", sizeof(buff),0);
                            break;
                            //exit(0);
                        }else{
                            printf("pass違う\n");
                            send(cli,"2", sizeof(buff),0);
                            break;
                        }
                    }
                }else{
                printf("not found account\n");
                send(sock,"3", sizeof(buff),0);
                exit(1);
                }
                }
                fclose(fp);
                printf("case1 end\n");
                //close(cli);
                //close(sock);
                break;
                
            case 2://登録
                printf("request make account\n");
                printf("%s\n",Account.addr);
                if ((fp = fopen(file, "r")) == NULL) break;
                while(fscanf(fp,"%s %s",txt_addr,txt_pass)!=EOF){
                    if(strcmp(txt_addr,Account.addr)==0){//アカウントが存在する場合
                        ac_flag=1;
                    }
                }
                fclose(fp);
                if(ac_flag!=1){//アカウントが存在しない場合
                send(cli,"1", sizeof(buff),0);
                printf("アカウント作成します\n");
                if ((fp = fopen(file, "a")) == NULL) break;
                recv(cli, Account.pass, sizeof(msg), 0);
                fprintf(fp,"\n%s %s",Account.addr,Account.pass);
                send(cli,"1", sizeof(buff),0);
                /////////ディレクトリの作成動作////////
                    if(mkdir(Account.addr,0777)==0){ //_mkdir("name")に変更
                        printf("ディレクトリ1作成に成功\n");
                    }else{
                        printf("ディレクトリ1作成できませんでした\n");
                    }
                ////////////////////////////////////
                    printf("作成に成功\n");
                fclose(fp);
                }else{//アカウントが存在する場合
                       printf("アカウントが存在\n");
                    send(cli,"2", sizeof(buff),0);
                }
                break;
                
            case 3://アカウント管理
                printf("request maintain\n");
                recv(cli, Account.pass, sizeof(msg), 0);
                if ((fp = fopen(file, "r")) == NULL) break;
                fscanf(fp,"%s %s",txt_addr,txt_pass);
                printf("%s %s %s %s\n",txt_addr,Account.addr,txt_pass,Account.pass);
                if(strcmp(txt_addr,Account.addr)==0){//アカウントが存在する場合
                        if (strcmp(txt_pass,Account.pass)==0) {//管理者に対する動作
                            send(cli,"1", sizeof(buff),0);
                        }else{//管理者パスワードが異なった場合
                            send(cli,"2", sizeof(buff),0);
                            break;
                        }
                }else{
                    printf("アカウントが存在しません\n");
                    send(cli,"2", sizeof(buff),0);
                    break;
                }
                fclose(fp);/////////////
                
                recv(cli, to, sizeof(msg), 0);
                val1=atoi(to);
                    switch(val1){
                        case 1://一覧表示
                            if ((fp = fopen(file, "r")) == NULL) break;
                            while(fscanf(fp,"%s %s",txt_addr,txt_pass)!=EOF){
                                sprintf(snd_msg,"%s%s%s%s",p_id,txt_addr,p_pass,txt_pass);
                                printf("%s\n",snd_msg);
                                send(cli,snd_msg,sizeof(buff),0);
                                }
                            fclose(fp);
                            sprintf(snd_msg,".");
                            send(cli,snd_msg, sizeof(buff),0);
                            break;
                            
                            
                        case 2://アカウント削除
                            if ((fp = fopen(file, "r")) == NULL) break;
                            while(fscanf(fp,"%s %s",txt_addr,txt_pass)!=EOF){
                                sprintf(snd_msg,"%s%s%s%s",p_id,txt_addr,p_pass,txt_pass);
                                send(cli,snd_msg,sizeof(buff),0);
                                printf("%s\n",snd_msg);//
                            }
                            sprintf(snd_msg,".");
                            send(cli,snd_msg, sizeof(buff),0);
                            fclose(fp);
                            recv(cli, to, sizeof(msg), 0);
                            val1=atoi(to)+1;//行数を格納**削除する
                            if ((fp = fopen(file, "r")) == NULL) break;
                            for(i = 0; i < sizeof(arr)/sizeof(arr[0]) && fgets(arr[i], sizeof(arr[i]), fp); i++){
                                ln++; // テキストファイルの行数
                            }
                            fclose(fp);
                            for (i=0; arr[val1-1][i]!='\0'; i++) {
                                if(arr[val1-1][i]==' '){
                                    rmdirnm[i]='\0';
                                }else rmdirnm[i]=arr[val1-1][i];
                            }
                            arr[val1-1][i]='\0';
                            printf("%s\n",rmdirnm);
                            arr[val1-1][0]='\0';
                            if ((fp = fopen(file, "w")) == NULL) break;
                            for(i=0;i<ln;i++){
                                if(arr[i][0]!='\0'){
                                    fputs(arr[i],fp);
                                }
                            }
                            fclose(fp);////////
                            /////////ディレクトリの削除動作////////
                            printf("%s",rmdirnm);///////////
                            
                            if(rmdir(rmdirnm)==0){ //_mkdir("name")に変更
                                printf("　のディレクトリ削除に成功\n");
                            }else{
                                printf("　のディレクトリを削除できませんでした\n");
                            }
                            sprintf(dirnm,"client%s",rmdirnm);
                            send(cli,dirnm,sizeof(buff),0);
                            printf("削除するディレクトリ名 %sを送信\n",dirnm);
                            ////////////////////////////////////
                            break;
                    }
                break;

        }
		close(cli);
		exit(0);
		}
		// parent
		else{
		close(cli);		// close parent copy
		waitpid(-1, NULL, WNOHANG);	// waits for child to exit
	}
    }
    close(sock);
    return 0;
}
