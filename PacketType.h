
#ifndef __PACKET_TYPE_H_
#define __PACKET_TYPE_H_

//��Ŷ Ÿ�� ����
typedef char PacketType;

//Ŭ�� -> ����
#define PKT_CHANGE_NAME 1 //�̸� ���� ��Ŷ
#define PKT_CHAT		2 //���� ���� -> ���� / Ŭ�� ����
#define	PKT_DISCONNECT	3 //���� ���� ��û
#define PKT_CONNECT		4 //���� ��û

//���� -> Ŭ��
#define PKT_LIST		5 // ������ �ִ� ��� ���
#define PKT_JOIN		6 // ���� ��� ����
#define PKT_OUT			7 // ��� �� �� ����

#endif