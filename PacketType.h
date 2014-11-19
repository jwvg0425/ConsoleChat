
#ifndef __PACKET_TYPE_H_
#define __PACKET_TYPE_H_

//패킷 타입 정의
typedef char PacketType;

//클라 -> 서버
#define PKT_CHANGE_NAME 1 //이름 변경 패킷
#define PKT_CHAT		2 //말한 내용 -> 서버 / 클라 공용
#define	PKT_DISCONNECT	3 //연결 종료 요청
#define PKT_CONNECT		4 //연결 요청

#endif