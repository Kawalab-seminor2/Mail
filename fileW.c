    #include <stdio.h>
    #include <stdlib.h>
     
    #define N 256 // 1行の最大文字数(バイト数)
     
    int main(void) {
    	FILE *fp; // FILE型構造体
    	char fname[] = "UserA/test.txt";
    	char str[N];
     
		fp = fopen(fname, "r"); // ファイルを開く。失敗するとNULLを返す。
		if(fp == NULL) {
    		printf("%s file not open!\n", fname);
    		return -1;
    	}
     
    	while(fgets(str, N, fp) != NULL) {
    		printf("%s", str);
    	}
     
    	fclose(fp); // ファイルを閉じる
     
    	return 0;
    }
