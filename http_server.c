#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>//close


int main(void){
  int rsock, wsock;
  struct sockaddr_in addr, client;
  int len, ret;

  char buf[2048];
  char inbuf[2048];

  // ソケットの作成
  rsock = socket(AF_INET, SOCK_STREAM, 0);// AF_INET : IPv4, SOCK_STREAM : バイトストリーム
  if(rsock < 0){
    fprintf(stderr, "ソケットを作成できません : socket\n");
    return -1;
  }

  // ソケットの設定
  addr.sin_family = AF_INET;// AF_INET : IPv4
  addr.sin_port = htons(8080);// 8080ポート
  addr.sin_addr.s_addr=INADDR_ANY;// INADDR_ANY : アドレスの指定はしない

  // ソケットの登録
  ret = bind(rsock, (struct sockaddr *)&addr, sizeof(addr));
  if (ret < 0){
    fprintf(stderr, "ソケット登録ができません : bind\n");
    close(rsock);
    return -1;
  }

  // ソケットの準備
  ret = listen(rsock, 5);
  if (ret < 0){
    fprintf(stderr, "ソケットの準備ができません : listen \n");
    close(rsock);
    return -1;
  }

  memset(buf, 0, sizeof(buf));
	snprintf(buf, sizeof(buf),
			"HTTP/1.1 200 OK\r\n"
			"\r\n"
			"HELLO World!\r\n"
			"This is first http server. \r\n"
      );

  while(1){
    len = sizeof(client);
    wsock = accept(rsock, (struct sockaddr *)&client, &len); 
    if (ret < 0){
      fprintf(stderr, "ソケットを受信できません : accept.\n");
      break;
    }

    memset(inbuf, 0, sizeof(inbuf));
    recv(wsock, inbuf,sizeof(inbuf), 0);
    send(wsock, buf, (int)strlen(buf), 0);

    close(wsock);
  }

  close(rsock);

  return 0;
}

