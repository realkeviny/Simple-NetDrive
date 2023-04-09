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
#define DELETE_DIRECTORY_SUCCESS "Successfully Deleted Directory!"
#define DELETE_DIRECTORY_FAILURE "Failed to delete directory: not a directory"
#define RENAMING_SUCCESS "Success"
#define RENAMING_FAILURE "Failed"
#define ENTERING_SUCCEESS
#define ENTERING_FAILURE "Failed to enter directory: Not a directory"
#define UPLOAD_SUCCESS "Uploaded!"
#define UPLOAD_FAILURE "Failed to upload!"
#define DELETE_FILE_SUCCESS "Successfully Deleted File!"
#define DELETE_FILE_FAILURE "Failed to delete file: not a file"

enum MSG_TYPE
{
	MIN = 0,
	REGISTER_REQUEST,	//注册请求
	REGISTER_RESPOND,	//回复注册请求
	LOGIN_REQUEST,	//登录请求
	LOGIN_RESPOND,	//回复登录请求
	ONLINE_REQUEST, //在线用户请求
	ONLINE_RESPOND, //在线用户回复
	SEARCH_REQUEST, //搜索用户请求
	SEARCH_RESPOND, //搜索用户回复
	ADD_REQUEST,//加好友请求
	ADD_RESPOND,//加好友回应
	ADD_AGREEMENT,//同意添加
	ADD_REFUSE,//拒绝添加
	FRIENDLIST_REFRESH_REQUEST,//好友列表刷新请求
	FRIENDLIST_REFRESH_RESPOND,//好友列表刷新回复
	FRIEND_DELETE_REQUEST,//删除好友请求
	FRIEND_DELETE_RESPOND,//删除好友回复
	CHAT_REQUEST,//通用私聊请求
	CHAT_RESPOND,//通用私聊回复
	GROUP_CHAT_REQUEST,//群聊请求
	GROUP_CHAT_RESPOND,//群聊回复
	CREATE_DIRECTORY_REQUEST,//目录创建请求
	CREATE_DIRECTORY_RESPOND,//目录创建回应
	REFRESH_REQUEST,//刷新请求
	REFRESH_RESPOND,//刷新回应
	DELETE_DIRECTORY_REQUEST,//目录删除请求
	DELETE_DIRECTORY_RESPOND,//目录删除回应
	RENAME_REQUEST,//重命名请求
	RENAME_RESPOND,//重命名回复
	ENTER_DIRECTORY_REQUEST,//进入文件夹请求
	ENTER_DIRECTORY_RESPOND,//进入文件夹回应
	UPLOAD_REQUEST,//上传请求
	UPLOAD_RESPOND,//上传回复
	DOWNLOAD_REQUEST,//下载请求
	DOWNLOAD_RESPOND,//下载回复
	DELETE_FILE_REQUEST,//文件删除请求
	DELETE_FILE_RESPOND,//文件删除回应
	MAX = 0x00ffffff
};

struct FileInfo
{
	char fileName[64];//文件名
	int fileType;//文件类型
};

struct PDU
{
	unsigned PDULen;//协议数据单元大小
	unsigned MsgType;//消息类型
	char Data[128];
	unsigned MsgLen;//实际消息长度
	int Msg[];//消息
};

PDU* makePDU(unsigned MsgLen);

#endif