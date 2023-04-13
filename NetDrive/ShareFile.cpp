#include "ShareFile.h"
#include "NetDrive.h"
#include "OperationWidget.h"

ShareFile::ShareFile(QWidget* parent)
	: QWidget(parent)
{
	m_btnSelectAll = new QPushButton("Select All");
	m_btnCancelSelected = new QPushButton("Cancel All");

	m_btnOK = new QPushButton("OK");
	m_btnCancel = new QPushButton("Cancel");

	m_sa = new QScrollArea;
	m_friendWidget = new QWidget;
	m_friendWidgetLayout = new QVBoxLayout(m_friendWidget);
	m_btnGroup = new QButtonGroup(m_friendWidget);
	m_btnGroup->setExclusive(false);

	QHBoxLayout* topLayout = new QHBoxLayout;
	topLayout->addWidget(m_btnSelectAll);
	topLayout->addWidget(m_btnCancelSelected);
	topLayout->addStretch();

	QHBoxLayout* bottomLayout = new QHBoxLayout;
	bottomLayout->addWidget(m_btnOK);
	bottomLayout->addWidget(m_btnCancel);

	QVBoxLayout* mainLayout = new QVBoxLayout;
	mainLayout->addLayout(topLayout);
	mainLayout->addWidget(m_sa);
	mainLayout->addLayout(bottomLayout);
	setLayout(mainLayout);

	connect(m_btnCancelSelected, SIGNAL(clicked()), this, SLOT(onBtnCancelSelectedClicked()));
	connect(m_btnSelectAll, SIGNAL(clicked()), this, SLOT(onBtnSelectAllClicked()));
	connect(m_btnOK, SIGNAL(clicked()), this, SLOT(onBtnOKClicked()));
	connect(m_btnCancel, SIGNAL(clicked()), this, SLOT(onBtnCancelClicked()));
}

ShareFile& ShareFile::getInstance()
{
	// TODO: insert return statement here
	static ShareFile Instance;
	return Instance;
}

void ShareFile::test()
{
	QVBoxLayout* testLayout = new QVBoxLayout(m_friendWidget);
	QCheckBox* CB = nullptr;
	for (int i = 0; i < 15; ++i)
	{
		CB = new QCheckBox("Max Payne");
		testLayout->addWidget(CB);
		m_btnGroup->addButton(CB);
	}
	m_sa->setWidget(m_friendWidget);
}

void ShareFile::updateFriend(QListWidget* fList)
{
	if (fList == nullptr)
		return;
	QAbstractButton* temp = nullptr;
	QList<QAbstractButton*> previousList = m_btnGroup->buttons();
	for (int i = 0; i < previousList.size(); ++i)
	{
		temp = previousList[i];
		m_friendWidgetLayout->removeWidget(temp);
		m_btnGroup->removeButton(temp);
		previousList.removeOne(temp);
		delete temp;
		temp = nullptr;
	}

	QCheckBox* CB = nullptr;
	for (int i = 0; i < fList->count(); ++i)
	{
		CB = new QCheckBox(fList->item(i)->text());
		m_friendWidgetLayout->addWidget(CB);
		m_btnGroup->addButton(CB);
	}
	m_sa->setWidget(m_friendWidget);
}

ShareFile::~ShareFile()
{}

void ShareFile::onBtnCancelSelectedClicked()
{
	QList<QAbstractButton*> checkBoxes = m_btnGroup->buttons();
	for (int i = 0; i < checkBoxes.size(); ++i)
	{
		if (checkBoxes[i]->isChecked())
		{
			checkBoxes[i]->setChecked(false);
		}
	}
}

void ShareFile::onBtnSelectAllClicked()
{
	QList<QAbstractButton*> checkBoxes = m_btnGroup->buttons();
	for (int i = 0; i < checkBoxes.size(); ++i)
	{
		if (!checkBoxes[i]->isChecked())
		{
			checkBoxes[i]->setChecked(true);
		}
	}
}

void ShareFile::onBtnOKClicked()
{
	QString strName = NetDrive::getInstance().getLoginName();
	QString strCurPath = NetDrive::getInstance().getCurrentPath();
	QString strShareFileName = OperationWidget::getInstance().getBook()->getShareFileName();

	QString strPath = strCurPath + "/" + strShareFileName;

	QList<QAbstractButton*> checkBoxes = m_btnGroup->buttons();
	int num = 0;
	for (int i = 0; i < checkBoxes.size(); ++i)
	{
		if (checkBoxes[i]->isChecked())
		{
			num++;
		}
	}

	PDU* pdu = makePDU(64 * num + strPath.size() + 1);
	pdu->MsgType = SHARE_FILE_REQUEST;
	sprintf(pdu->Data, "%s %d", strName.toStdString().c_str(), num);
	int j = 0;
	for (int i = 0; i < checkBoxes.size(); ++i)
	{
		if (checkBoxes[i]->isChecked())
		{
			//memcpy((char*)(pdu->Msg) + j * 64, checkBoxes[i]->text(), 64);
			j++;
		}
	}

	memcpy((char*)(pdu->Msg) + num * 64, strPath.toStdString().c_str(), strPath.size());

	NetDrive::getInstance().getTcpSocket().write((char*)pdu, pdu->PDULen);

	free(pdu);
	pdu = nullptr;
}

void ShareFile::onBtnCancelClicked()
{
}