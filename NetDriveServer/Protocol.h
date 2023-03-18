#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <string.h>
#include <stdlib.h>

#define REGISTER_OK "Successfully registered!"
#define REGISTER_FAILURE "User exists!"
#define LOGIN_OK "Logged in"
#define LOGIN_FAILURE "Failed to login!Check Name/Password or your line status!"
#define SEARCH_USER_INEXIST "No result"
#define SEARCH_USER_EXIST_ONLINE "Found online"
#define SEARCH_USER_EXIST_OFFLINE "Found offline"
#define UNKNOWN_ERROR "Unknown Error"
#define FRIEND_ALREADY_EXISTS "Friend already exists!"
#define USER_OFFLINE "User offline!"
#define USER_NOT_EXIST "User Not Found!"
#define FRIEND_DELETED "Deleted!"
#define DIRECTORY_NOT_EXIST "Invalid Directory!"
#define DIRECTORY_ALREADY_EXIST "Directory Already Created!"
#define FILENAME_ALREADY_EXIST "Duplicate FileName!"
#define CREATE_DIRECTORY_SUCCESS "Successfully Create Directory!"

enum MSG_TYPE
{
	MIN = 0,
	REGISTER_REQUEST,	//ע������
	REGISTER_RESPOND,	//�ظ�ע������
	LOGIN_REQUEST,	//��¼����
	LOGIN_RESPOND,	//�ظ���¼����
	ONLINE_REQUEST, //�����û�����
	ONLINE_RESPOND, //�����û��ظ�
	SEARCH_REQUEST, //�����û�����
	SEARCH_RESPOND, //�����û��ظ�
	ADD_REQUEST,//�Ӻ�������
	ADD_RESPOND,//�Ӻ��ѻ�Ӧ
	ADD_AGREEMENT,//ͬ������
	ADD_REFUSE,//�ܾ�����
	FRIENDLIST_REFRESH_REQUEST,//�����б�ˢ������
	FRIENDLIST_REFRESH_RESPOND,//�����б�ˢ�»ظ�
	FRIEND_DELETE_REQUEST,//ɾ����������
	FRIEND_DELETE_RESPOND,//ɾ�����ѻظ�
	CHAT_REQUEST,//ͨ��˽������
	CHAT_RESPOND,//ͨ��˽�Ļظ�
	GROUP_CHAT_REQUEST,//Ⱥ������
	GROUP_CHAT_RESPOND,//Ⱥ�Ļظ�
	CREATE_DIRECTORY_REQUEST,//Ŀ¼��������
	CREATE_DIRECTORY_RESPOND,//Ŀ¼������Ӧ
	MAX = 0x00ffffff
};

struct PDU
{
	unsigned PDULen;//Э�����ݵ�Ԫ��С
	unsigned MsgType;//��Ϣ����
	char Data[128];
	unsigned MsgLen;//ʵ����Ϣ����
	int Msg[];//��Ϣ
};

PDU* makePDU(unsigned MsgLen);

#endif