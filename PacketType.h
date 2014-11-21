
#ifndef __PACKET_TYPE_H_
#define __PACKET_TYPE_H_

#define MAX_NAME_LENGTH 10

//패킷 타입 정의
typedef char PacketType;

//클라 -> 서버
#define PKT_CHANGE_NAME 1 //이름 변경 패킷
#define PKT_CHAT		2 //말한 내용 -> 서버 / 클라 공용
#define	PKT_DISCONNECT	3 //연결 종료 요청
#define PKT_CONNECT		4 //연결 요청

//서버 -> 클라
#define PKT_UPDATE_LIST 5 //접속자 목록 갱신
#define PKT_PERMISSION	6 //접속 승인 정보.

//permission code
#define PERMISSION_WAIT				0 //서버의 접속 승인을 기다리는 상태.
#define PERMISSION_SERVER_FULL		1 //서버가 꽉차서 접속 불가.
#define PERMISSION_EQUAL_NAME		2 //서버에 이미 동일 아이디 존재.
#define PERMISSION_TOO_LONG_NAME	3 //이름이 너무 길다.
#define PERMISSION_OK				4 //서버 접속 승인.

#endif