#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>//close
#include <fcntl.h>//open
#include <sys/stat.h>


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
  int fd = open("image.jpg", O_RDONLY);
  if(fd<0){
    fprintf(stderr, "画像ファイルを開けません : open\n");
    return -1;
  }
  struct stat stbuf;
  if (fstat(fd, &stbuf)<0){
    fprintf(stderr, "ファイル情報の取得に失敗しました : fstat\n");
    return -1;
  }
  size_t image_size = stbuf.st_size;
  char *image_data = (char *)malloc(sizeof(char)* image_size);
  ssize_t n = read(fd,image_data,image_size);
  if(n !=image_size){
    fprintf(stderr, "画像ファイルの読み込みに失敗しました: read\n");
    return -1;
  }
  close(fd);

  memset(buf, 0, sizeof(buf));
  snprintf(buf, sizeof(buf),
      "HTTP/1.1 200 OK\r\n"
      "Content-Type: image/jpeg\r\n"
      "Content-Length: %zd\r\n"
      "\r\n", image_size);

  while(1){
    len = sizeof(client);
    wsock = accept(rsock, (struct sockaddr *)&client, &len); 
    if (wsock < 0){
      fprintf(stderr, "ソケットを受信できません : accept.\n");
      break;
    }
    memset(inbuf, 0, sizeof(inbuf));
    recv(wsock, inbuf, sizeof(inbuf),0);
    send(wsock, buf, strlen(buf), 0);
    send(wsock, image_data, image_size, 0);
    close(wsock);
  }
  close(rsock);
  return 0;
}

