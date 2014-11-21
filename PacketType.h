
#ifndef __PACKET_TYPE_H_
#define __PACKET_TYPE_H_

#define MAX_NAME_LENGTH 10

//��Ŷ Ÿ�� ����
typedef char PacketType;

//Ŭ�� -> ����
#define PKT_CHANGE_NAME 1 //�̸� ���� ��Ŷ
#define PKT_CHAT		2 //���� ���� -> ���� / Ŭ�� ����
#define	PKT_DISCONNECT	3 //���� ���� ��û
#define PKT_CONNECT		4 //���� ��û

//���� -> Ŭ��
#define PKT_UPDATE_LIST 5 //������ ��� ����
#define PKT_PERMISSION	6 //���� ���� ����.

//permission code
#define PERMISSION_WAIT				0 //������ ���� ������ ��ٸ��� ����.
#define PERMISSION_SERVER_FULL		1 //������ ������ ���� �Ұ�.
#define PERMISSION_EQUAL_NAME		2 //������ �̹� ���� ���̵� ����.
#define PERMISSION_TOO_LONG_NAME	3 //�̸��� �ʹ� ���.
#define PERMISSION_OK				4 //���� ���� ����.

#endif