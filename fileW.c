    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>

    #define N 256 // 1行の最大文字数(バイト数)
     
    int main(void) {
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
    		return -1;
    		}
     
    		while(fgets(str, N, fp) != NULL) {
    			printf("%s", str);
    		}
     
    		fclose(fp); // ファイルを閉じる
    		remove(fname);
    		fi[0]+=1;
     	}
    	return 0;
    }
