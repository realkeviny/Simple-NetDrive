#include "TcpSocket.h"
#include <QDebug>
#include "TcpServer.h"
#include <QFileInfoList>

TcpSocket::TcpSocket()
{
	connect(this, SIGNAL(readyRead()), this, SLOT(receiveMessage()));
	connect(this, SIGNAL(disconnected()), this, SLOT(clientOffline()));

	m_uploadstatus = false;
}

void TcpSocket::receiveMessage()
{
	if (!m_uploadstatus)
	{
		//qDebug() << this->bytesAvailable();
		uint PDULen = 0;
		this->read((char*)&PDULen, sizeof(uint));
		uint MsgLen = PDULen - sizeof(PDU);
		PDU* pdu = makePDU(MsgLen);
		this->read((char*)pdu + sizeof(uint), PDULen - sizeof(uint));
		switch (pdu->MsgType)
		{
		case REGISTER_REQUEST:
		{
			char nameMsg[64] = { '\0' };
			char pwdMsg[64] = { '\0' };
			strncpy(nameMsg, pdu->Data, 64);
			strncpy(pwdMsg, pdu->Data + 64, 64);
			bool ret = DatabaseOperation::getInstance().handleRegister(nameMsg, pwdMsg);
			PDU* respondPDU = makePDU(0);
			respondPDU->MsgType = REGISTER_RESPOND;
			if (ret)
			{
				strcpy(respondPDU->Data, REGISTER_OK);
				QDir dir;
				qDebug() << dir.mkdir(QString("D:/UserFiles/%1").arg(nameMsg));
			}
			else
			{
				strcpy(respondPDU->Data, REGISTER_FAILURE);
			}
			write((char*)respondPDU, respondPDU->PDULen);
			free(respondPDU);
			respondPDU = nullptr;
			break;
		}
		case LOGIN_REQUEST:
		{
			char nameMsg[64] = { '\0' };
			char pwdMsg[64] = { '\0' };
			strncpy(nameMsg, pdu->Data, 64);
			strncpy(pwdMsg, pdu->Data + 64, 64);
			bool ret = DatabaseOperation::getInstance().handleLogin(nameMsg, pwdMsg);
			PDU* respondPDU = makePDU(0);
			respondPDU->MsgType = LOGIN_RESPOND;
			if (ret)
			{
				strcpy(respondPDU->Data, LOGIN_OK);
				strName = nameMsg;
			}
			else
			{
				strcpy(respondPDU->Data, LOGIN_FAILURE);
			}
			write((char*)respondPDU, respondPDU->PDULen);
			free(respondPDU);
			respondPDU = nullptr;
			break;
		}
		case ONLINE_REQUEST:
		{
			QStringList ret = DatabaseOperation::getInstance().handleAllOnlineUsers();

			uint MsgLen = ret.size() * 64;
			PDU* responsePDU = makePDU(MsgLen);
			responsePDU->MsgType = ONLINE_RESPOND;
			for (int i = 0; i < ret.size(); i++)
			{
				memcpy((char*)(responsePDU->Msg) + i * 64, ret[i].toStdString().c_str(), ret.at(i).size());
			}
			write((char*)responsePDU, responsePDU->PDULen);
			free(responsePDU);
			responsePDU = nullptr;
			break;
		}
		case SEARCH_REQUEST:
		{
			int ret = DatabaseOperation::getInstance().handleSearch(pdu->Data);
			PDU* responsePDU = makePDU(0);
			responsePDU->MsgType = SEARCH_RESPOND;
			if (ret == -1)
			{
				strcpy(responsePDU->Data, SEARCH_USER_INEXIST);
			}
			else if (ret == 1)
			{
				strcpy(responsePDU->Data, SEARCH_USER_EXIST_ONLINE);
			}
			else if (!ret)
			{
				strcpy(responsePDU->Data, SEARCH_USER_EXIST_OFFLINE);
			}
			write((char*)responsePDU, responsePDU->PDULen);
			free(responsePDU);
			responsePDU = nullptr;
			break;
		}
		case ADD_REQUEST:
		{
			char friendName[64] = { '\0' };
			char myName[64] = { '\0' };
			strncpy(friendName, pdu->Data, 64);
			strncpy(myName, pdu->Data + 64, 64);
			int ret = DatabaseOperation::getInstance().handleAddFriends(friendName, myName);
			PDU* responsePDU = nullptr;
			if (-1 == ret)
			{
				responsePDU = makePDU(0);
				responsePDU->MsgType = ADD_RESPOND;
				strcpy(responsePDU->Data, UNKNOWN_ERROR);
				write((char*)responsePDU, responsePDU->PDULen);
				free(responsePDU);
				responsePDU = nullptr;
			}
			else if (0 == ret)
			{
				responsePDU = makePDU(0);
				responsePDU->MsgType = ADD_RESPOND;
				strcpy(responsePDU->Data, FRIEND_ALREADY_EXISTS);
				write((char*)responsePDU, responsePDU->PDULen);
				free(responsePDU);
				responsePDU = nullptr;
			}
			else if (1 == ret)
			{
				TcpServer::getInstance().forward(friendName, pdu);
			}
			else if (2 == ret)
			{
				responsePDU = makePDU(0);
				responsePDU->MsgType = ADD_RESPOND;
				strcpy(responsePDU->Data, USER_OFFLINE);
				write((char*)responsePDU, responsePDU->PDULen);
				free(responsePDU);
				responsePDU = nullptr;
			}
			else if (3 == ret)
			{
				responsePDU = makePDU(0);
				responsePDU->MsgType = ADD_RESPOND;
				strcpy(responsePDU->Data, USER_NOT_EXIST);
				write((char*)responsePDU, responsePDU->PDULen);
				free(responsePDU);
				responsePDU = nullptr;
			}
			break;
		}
		case ADD_AGREEMENT:
		{
			char Name[64] = { '\0' };
			char friendName[64] = { '\0' };
			strncpy(friendName, pdu->Data, 64);
			strncpy(Name, pdu->Data + 64, 64);
			DatabaseOperation::getInstance().handleAgreeAddFriend(friendName, Name);

			TcpServer::getInstance().forward(Name, pdu);
			break;
		}
		case ADD_REFUSE:
		{
			char Name[64] = { '\0' };
			strncpy(Name, pdu->Data + 64, 64);
			TcpServer::getInstance().forward(Name, pdu);
			break;
		}

		case FRIENDLIST_REFRESH_REQUEST:
		{
			char Name[64] = { '\0' };
			strncpy(Name, pdu->Data, 64);
			QStringList ret = DatabaseOperation::getInstance().handleRefresh(Name);
			uint messageLength = ret.size() * 64;
			PDU* responsePDU = makePDU(messageLength);
			responsePDU->MsgType = FRIENDLIST_REFRESH_RESPOND;
			for (int i = 0; i < ret.size(); i++)
			{
				memcpy((char*)(responsePDU->Msg) + i * 64, ret.at(i).toStdString().c_str(), ret.at(i).size());
			}
			write((char*)responsePDU, responsePDU->PDULen);
			free(responsePDU);
			responsePDU = nullptr;
			break;
		}
		case FRIEND_DELETE_REQUEST:
		{
			char myName[64] = { '\0' };
			char friendName[64] = { '\0' };
			strncpy(myName, pdu->Data, 64);
			strncpy(friendName, pdu->Data + 64, 64);
			DatabaseOperation::getInstance().handleDeleteFriend(myName, friendName);

			PDU* responsePDU = makePDU(0);
			responsePDU->MsgType = FRIEND_DELETE_RESPOND;
			strcpy(responsePDU->Data, FRIEND_DELETED);
			write((char*)responsePDU, responsePDU->PDULen);
			free(responsePDU);
			responsePDU = nullptr;

			TcpServer::getInstance().forward(friendName, pdu);
			break;
		}
		case CHAT_REQUEST:
		{
			char cUserName[64] = { '\0' };
			memcpy(cUserName, pdu->Data + 64, 64);
			qDebug() << cUserName;
			TcpServer::getInstance().forward(cUserName, pdu);
			break;
		}
		case GROUP_CHAT_REQUEST:
		{
			char Name[64] = { '\0' };
			strncpy(Name, pdu->Data, 64);
			QStringList onlineFriends = DatabaseOperation::getInstance().handleRefresh(Name);
			QString buf;
			for (int i = 0; i < onlineFriends.size(); i++)
			{
				buf = onlineFriends.at(i);
				TcpServer::getInstance().forward(buf.toStdString().c_str(), pdu);
			}
			break;
		}
		case CREATE_DIRECTORY_REQUEST:
		{
			QDir dir;
			QString strCurrentPath = QString("%1").arg((char*)(pdu->Msg));
			qDebug() << strCurrentPath;
			bool ret = dir.exists(strCurrentPath);
			PDU* resPDU = nullptr;
			if (ret)//当前目录存在
			{
				//判断下级目录有无同名文件夹
				char newDir[64] = { '\0' };
				memcpy(newDir, pdu->Data + 64, 64);
				QString strNewPath = strCurrentPath + "/" + newDir;
				qDebug() << strNewPath;
				ret = dir.exists(strNewPath);
				qDebug() << "-->" << ret;
				if (ret)//子目录名同名
				{
					resPDU = makePDU(0);
					resPDU->MsgType = CREATE_DIRECTORY_RESPOND;
					strcpy(resPDU->Data, FILENAME_ALREADY_EXIST);
				}
				else//子目录名不存在
				{
					dir.mkdir(strNewPath);
					resPDU = makePDU(0);
					resPDU->MsgType = CREATE_DIRECTORY_RESPOND;
					strcpy(resPDU->Data, CREATE_DIRECTORY_SUCCESS);
				}
			}
			else//当前目录不存在
			{
				resPDU = makePDU(0);
				resPDU->MsgType = CREATE_DIRECTORY_RESPOND;
				strcpy(resPDU->Data, DIRECTORY_NOT_EXIST);
			}
			write((char*)resPDU, resPDU->PDULen);
			free(resPDU);
			resPDU = nullptr;
			break;
		}
		case REFRESH_REQUEST:
		{
			char* currentPath = new char[pdu->MsgLen];
			memcpy(currentPath, pdu->Msg, pdu->MsgLen);
			QDir dir(currentPath);
			QFileInfoList fileInfoList = dir.entryInfoList();
			int fileCount = fileInfoList.size();
			PDU* responsePDU = makePDU(sizeof(FileInfo) * fileCount);
			responsePDU->MsgType = REFRESH_RESPOND;
			FileInfo* pointerFileInfo = nullptr;
			QString StrfileName;
			for (int i = 0; i < fileCount; i++)
			{
				pointerFileInfo = reinterpret_cast<FileInfo*>(responsePDU->Msg) + i;
				StrfileName = fileInfoList[i].fileName();

				memcpy(pointerFileInfo->fileName, StrfileName.toStdString().c_str(), StrfileName.size());
				if (fileInfoList[i].isDir())
				{
					pointerFileInfo->fileType = 0;//Folder
				}
				else if (fileInfoList[i].isFile())
				{
					pointerFileInfo->fileType = 1;//File
				}
			}
			write((char*)responsePDU, responsePDU->PDULen);
			free(responsePDU);
			responsePDU = nullptr;
			break;
		}
		case DELETE_DIRECTORY_REQUEST:
		{
			char dirName[64] = { '\0' };
			strcpy(dirName, pdu->Data);
			char* path = new char[pdu->MsgLen];
			memcpy(path, pdu->Msg, pdu->MsgLen);
			QString fullPath = QString("%1/%2").arg(path).arg(dirName);

			QFileInfo fileInfo(fullPath);
			bool ret = false;
			if (fileInfo.isDir())
			{
				QDir dir;
				dir.setPath(fullPath);
				ret = dir.removeRecursively();
			}
			else if (fileInfo.isFile())//常规文件
			{
				ret = false;
			}

			PDU* responsePDU = nullptr;
			if (ret)
			{
				responsePDU = makePDU(strlen(DELETE_DIRECTORY_SUCCESS) + 1);
				responsePDU->MsgType = DELETE_DIRECTORY_RESPOND;
				memcpy(responsePDU->Data, DELETE_DIRECTORY_SUCCESS, strlen(DELETE_DIRECTORY_SUCCESS));
			}
			else
			{
				responsePDU = makePDU(strlen(DELETE_DIRECTORY_FAILURE) + 1);
				responsePDU->MsgType = DELETE_DIRECTORY_RESPOND;
				memcpy(responsePDU->Data, DELETE_DIRECTORY_FAILURE, strlen(DELETE_DIRECTORY_FAILURE));
			}

			write((char*)responsePDU, responsePDU->PDULen);
			free(responsePDU);
			responsePDU = nullptr;
			break;
		}
		case RENAME_REQUEST:
		{
			char oldName[64] = { '\0' };
			char newName[64] = { '\0' };
			strncpy(oldName, pdu->Data, 64);
			strncpy(newName, pdu->Data + 64, 64);

			char* path = new char[pdu->MsgLen];
			memcpy(path, pdu->Msg, pdu->MsgLen);

			QString oldPath = QString("%1/%2").arg(path).arg(oldName);
			QString newPath = QString("%1/%2").arg(path).arg(newName);

			qDebug() << oldPath;
			qDebug() << newPath;

			QDir dir;
			bool ret = dir.rename(oldPath, newPath);
			PDU* responsePDU = makePDU(0);
			responsePDU->MsgType = RENAME_RESPOND;
			if (ret)
			{
				strcpy(responsePDU->Data, RENAMING_SUCCESS);
			}
			else
			{
				strcpy(responsePDU->Data, RENAMING_FAILURE);
			}
			write((char*)responsePDU, responsePDU->PDULen);
			free(responsePDU);
			responsePDU = nullptr;
			break;
		}
		case ENTER_DIRECTORY_REQUEST:
		{
			char enteringName[64] = { '\0' };
			strncpy(enteringName, pdu->Data, 64);

			char* path = new char[pdu->MsgLen];
			memcpy(path, pdu->Msg, pdu->MsgLen);

			QString fullPath = QString("%1/%2").arg(path).arg(enteringName);

			qDebug() << fullPath;
			QFileInfo fileInfo(fullPath);
			PDU* responsePDU = nullptr;
			if (fileInfo.isDir())
			{
				QDir dir(fullPath);
				QFileInfoList fileInfoList = dir.entryInfoList();
				int fileCount = fileInfoList.size();
				PDU* responsePDU = makePDU(sizeof(FileInfo) * fileCount);
				responsePDU->MsgType = REFRESH_RESPOND;
				FileInfo* pointerFileInfo = nullptr;
				QString StrfileName;
				for (int i = 0; i < fileCount; i++)
				{
					pointerFileInfo = reinterpret_cast<FileInfo*>(responsePDU->Msg) + i;
					StrfileName = fileInfoList[i].fileName();

					memcpy(pointerFileInfo->fileName, StrfileName.toStdString().c_str(), StrfileName.size());
					if (fileInfoList[i].isDir())
					{
						pointerFileInfo->fileType = 0;//Folder
					}
					else if (fileInfoList[i].isFile())
					{
						pointerFileInfo->fileType = 1;//File
					}
				}
				write((char*)responsePDU, responsePDU->PDULen);
				free(responsePDU);
				responsePDU = nullptr;
			}
			else if (fileInfo.isFile())
			{
				responsePDU = makePDU(0);
				responsePDU->MsgType = ENTER_DIRECTORY_RESPOND;
				strcpy(responsePDU->Data, ENTERING_FAILURE);

				write((char*)responsePDU, responsePDU->PDULen);
				free(responsePDU);
				responsePDU = nullptr;
			}
			break;
		}

		case UPLOAD_REQUEST:
		{
			char fileName[64] = { '\0' };
			qint64 fileSize = 0;
			sscanf(pdu->Data, "%s %lld", fileName, &fileSize);
			char* path = new char[pdu->MsgLen];
			memcpy(path, pdu->Msg, pdu->MsgLen);
			QString fullPath = QString("%1/%2").arg(path).arg(fileName);
			qDebug() << fullPath;
			delete[] path;
			path = nullptr;

			m_file.setFileName(fullPath);
			//只写方式打开文件，不存在则创建
			if (m_file.open(QIODevice::WriteOnly))
			{
				m_uploadstatus = true;
				m_totalsize = fileSize;
				m_receivedsize = 0;
			}

			break;
		}
		}

		free(pdu);
		pdu = nullptr;
	}
	else
	{
		PDU* respdu = nullptr;
		respdu = makePDU(0);
		respdu->MsgType = UPLOAD_RESPOND;

		QByteArray buf = readAll();
		m_file.write(buf);
		m_receivedsize += buf.size();
		if (m_totalsize == m_receivedsize)
		{
			m_file.close();
			m_uploadstatus = false;

			strcpy(respdu->Data, UPLOAD_SUCCESS);
			write(reinterpret_cast<char*>(respdu), respdu->PDULen);
			free(respdu);
			respdu = nullptr;
		}
		else if (m_totalsize < m_receivedsize)
		{
			m_file.close();
			m_uploadstatus = false;

			strcpy(respdu->Data, UPLOAD_FAILURE);
			write(reinterpret_cast<char*>(respdu), respdu->PDULen);
			free(respdu);
			respdu = nullptr;
		}
	}
}

void TcpSocket::clientOffline()
{
	DatabaseOperation::getInstance().handleOffline(strName.toStdString().c_str());
	emit offline(this);
}

QString TcpSocket::getName() const
{
	return strName;
}