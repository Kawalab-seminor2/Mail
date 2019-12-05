#include <sys/fcntl.h>

#include <sys/types.h>

#include <sys/socket.h>

#include <netinet/in.h>

#include <netdb.h>

#include <stdio.h>

#define MAX_BUFF 64

main()

{

  int sockfd

  int new_sockfd;

  int writer_len;

  struct sockaddr_in reader_addr; 

  struct sockaddr_in writer_addr;



  /* ソケットの生成 */

  if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {

    perror("reader: socket");

    exit(1);

  }



  /* 通信ポート・アドレスの設定 */

  bzero((char *) &reader_addr, sizeof(reader_addr));

  reader_addr.sin_family = PF_INET;

  reader_addr.sin_addr.s_addr = htonl(INADDR_ANY);

  reader_addr.sin_port = htons(8000);



  /* ソケットにアドレスを結びつける */

  if (bind(sockfd, (struct sockaddr *)&reader_addr, sizeof(reader_addr)) < 0) {

    perror("reader: bind");

    exit(1);

  }



  /* コネクト要求をいくつまで待つかを設定 */

  if (listen(sockfd, 5) < 0) {

    perror("reader: listen");

    close(sockfd);

    exit(1);

  }



  /* コネクト要求を待つ */

  if ((new_sockfd = accept(sockfd,(struct sockaddr *)&writer_addr, &writer_len)) < 0) {

    perror("reader: accept");

    exit(1);

  }
  
  
  simpe_server(new_sockfd);
  

  close(sockfd);  /* ソケットを閉鎖 */

}



void simpe_server(int sockfd) {

  char buf[MAX_BUFF];

  int buf_len;
  
  FILE *fp; // FILE型構造体
  char fname[] = "test.txt";
 
 
	fp = fopen(fname, "r"); // ファイルを開く。失敗するとNULLを返す。
	if(fp == NULL) {
		printf("%s file not open!\n", fname);
		return -1;
	}
 
	while(fgets(buff, MAX_BUFF, fp) != NULL) {
	
		write(sockfd,buff,MAX_BUFF);
	}
 
	fclose(fp); // ファイルを閉じる
 
  



  close(new_sockfd);  /* ソケットを閉鎖 */

} 

