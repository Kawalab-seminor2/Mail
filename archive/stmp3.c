// SMTPの基本プロコトルをC言語で実装
// 整えてプログラムしてないのでかなり見にくい
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
  
#define ERR -1
#define MAX 64
#define SERVER_IP "127.0.0.1" //local loopback
#define SMTP 1900
#define POP 110
  
void usage(void);
char *command_name;
int socket(int domain, int type, int protocol);
  
int main(int argc, char* argv[]){
  int sd;                 //ソケットディスクリプタ
  FILE *fp;
  struct sockaddr_in server;//サーバプロセスのソケットアドレス情報
  struct hostent *hp; //ホスト情報
  char buf[MAX];
  char *mes;
   
  if(argc != 6)
    usage(); 
   
  /*ソケットの作成*/
  if((sd = socket(AF_INET, SOCK_STREAM, 0)) == ERR){
    perror("client: socket");
    exit(1);
  }

  /*サーバの情報を格納*/
  memset((void*)&server, 0, sizeof(server));
  server.sin_family =AF_INET;
  server.sin_port = htons(SMTP);
//   printf("%d", server.sin_family);
   
  /*IPアドレスの設定*/
  hp = gethostbyname(argv[4]);
  memcpy(&(server.sin_addr), hp->h_addr_list[0], hp->h_length);
   
  /*サーバに接続*/
  if(connect(sd, (struct sockaddr *)&server, sizeof(server))==ERR){
    perror("client: connect");
    exit(1);
  }

  read(sd, buf, sizeof(buf));
  printf("%s", buf);
  
/* HELOの送信 */
  strcpy(buf,"HELO ");
  strcat(buf,argv[5]);
  strcat(buf,"\n");
  printf("書き込む内容//%s",buf);
  write(sd, buf, strlen(buf));  
  read(sd, buf, sizeof(buf));
  printf("読み込んだ内容//%s", buf);
   
  /* MAIL FROMの送信 */
  strcpy(buf, "MAIL FROM: ");
  strcat(buf,argv[1]); 
  strcat(buf, "\r\n"); 
  printf("書き込む内容//%s",buf);
  write(sd, buf, strlen(buf)); 
  read(sd, buf, sizeof(buf));
  printf("読み込んだ内容//%s", buf);
  
/* RCPT TOの送信 */
  strcpy(buf, "RCPT TO: ");
  strcat(buf, argv[2]);
  strcat(buf, "\r\n");
  printf("書き込む内容//%s",buf);
  write(sd, buf, strlen(buf)); 
  read(sd, buf, sizeof(buf));
  printf("読み込んだ内容//%s", buf);
   
  strcpy(buf,"DATA\r\n");
  printf("書き込む内容//%s",buf);
  write(sd, buf, strlen(buf)); 
  read(sd, buf, sizeof(buf));
  printf("読み込んだ内容//%s", buf);
  strcpy(buf,"From:");
  strcat(buf,argv[1]);
  strcat(buf,"\r\n");
  strcat(buf,"To:");
  strcat(buf,argv[2]);
  strcat(buf,"\n");
  strcat(buf,argv[3]);
  strcat(buf,"\r\n");
  printf("書き込む内容//%s",buf);
  write(sd, buf, strlen(buf)); 
  strcpy(buf,".\r\n");
  write(sd, buf, strlen(buf));
  printf("書き込む内容//%s",buf);
  read(sd, buf, sizeof(buf));
  printf("読み込んだ内容//%s", buf);
  
  write(sd, "QUIT", sizeof(buf));  
  read(sd, write, sizeof(buf));
   
  return 0;
} 

void usage(void)
{
  fprintf(stderr, "Usage: %s 差出 宛先 テキスト サーバー名 ドメイン\n",command_name);
  exit(1);
}


