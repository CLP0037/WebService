#include "mainwindow.h"
#include "client.h"
#include "ui_mainwindow.h"


QtSoapHttpTransport* m_pHttp;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->cli = new Client("localhost",12345);
    connect(cli,SIGNAL(response(QString)), this, SLOT(receiveResponse(QString)));

    // 连接信号槽
    m_pHttp = new QtSoapHttpTransport(this);
    connect(m_pHttp, SIGNAL(responseReady(const QtSoapMessage &)), this, SLOT(onResponse(const QtSoapMessage &)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWindow::on_checkLogin_clicked(){
    cli->checkLogin(ui->login->text());

}

void MainWindow::on_createUser_clicked(){
    cli->createUser(ui->login->text(), ui->pass->text());

}

void MainWindow::receiveResponse(QString response){
    ui->responseText->setText(response);

}



void MainWindow::on_QQ_OnlineCheck_clicked()
{
    //ui->lineEdit_QQNumber
    QtSoapMessage message;

    // 设置方法
    message.setMethod("qqCheckOnline", "http://WebXml.com.cn/");

    // 设置动作
    m_pHttp->setAction("http://WebXml.com.cn/qqCheckOnline");

    // 设置主机
    m_pHttp->setHost("www.webxml.com.cn");

    // 添加方法参数
    QString strQQ = ui->lineEdit_QQNumber->text();
    message.addMethodArgument("qqCode", "", strQQ);
    QString strXML = message.toXmlString();

    // 提交请求
    m_pHttp->submitRequest(message, "/webservices/qqOnlineWebService.asmx");
}

void MainWindow::onResponse(const QtSoapMessage &response)
{
    QString strXML = response.toXmlString();
    QDomDocument doc;
    doc.setContent(strXML);

    // 在线状态
    QDomNodeList nodeList = doc.elementsByTagName("qqCheckOnlineResult");
    if (!nodeList.isEmpty())
    {
        QDomNode node = nodeList.at(0);
        QString strResult = node.toElement().text();
        QString strState("N/A");
        if (QString::compare(strResult, "Y") ==0)
        {
            strState = QString::fromLocal8Bit("在线");
        }
        else if (QString::compare(strResult, "N") == 0)
        {
            strState = QString::fromLocal8Bit("离线");
        }
        else if (QString::compare(strResult, "E") == 0)
        {
            strState = QString::fromLocal8Bit("QQ号码错误");
        }
        else if (QString::compare(strResult, "A") == 0)
        {
            strState = QString::fromLocal8Bit("商业用户验证失败");
        }
        else if (QString::compare(strResult, "V") == 0)
        {
            strState = QString::fromLocal8Bit("免费用户超过数量");
        }

        ui->lineEdit_QQOnlineState->setText(strState);
    }


    //归属地查看
    nodeList = doc.elementsByTagName("getMobileCodeInfoResult");
    if (!nodeList.isEmpty())
    {
        QDomNode node = nodeList.at(0);
        QString strResult = node.toElement().text();
        QString strState("N/A");

        ui->lineEdit_PhoneAddress->setText(strResult);
    }

    //验证 Email 地址是否正确
    nodeList = doc.elementsByTagName("ValidateEmailAddressResult");
    if (!nodeList.isEmpty())
    {
        bool ok;
        QDomNode node = nodeList.at(0);
        unsigned char ucharResult = node.toElement().text().trimmed().toUShort(&ok,10);
        QString strState("N/A");
        switch(ucharResult)
        {
        case 0:
            strState = QString::fromLocal8Bit("请重新验证");
            break;
        case 1:
            strState = QString::fromLocal8Bit("邮件地址合法");
            break;
        case 2:
            strState = QString::fromLocal8Bit("只是域名正确");
            break;
        case 3:
            strState = QString::fromLocal8Bit("一个未知错误");
            break;
        case 4:
            strState = QString::fromLocal8Bit("邮件服务器没有找到");
            break;
        case 5:
            strState = QString::fromLocal8Bit("电子邮件地址错误");
            break;
        case 6:
            strState = QString::fromLocal8Bit("免费用户验证超过数量（50次/24小时）");
            break;
        case 7:
            strState = QString::fromLocal8Bit("商业用户不能通过验证");
            break;
        }

        ui->lineEdit_EmailResult->setText(strState);
    }



}

void MainWindow::on_Phone_AddrCheck_clicked()
{
    QtSoapMessage message;

    // 设置方法
    message.setMethod("getMobileCodeInfo", "http://WebXml.com.cn/");

    // 设置动作
    m_pHttp->setAction("http://WebXml.com.cn/getMobileCodeInfo");

    // 设置主机
    m_pHttp->setHost("www.webxml.com.cn");

    // 添加方法参数
    QString strPhone = ui->lineEdit_PhoneNumber->text();
    message.addMethodArgument("mobileCode", "", strPhone);
    message.addMethodArgument("userID", "", "");
    QString strXML = message.toXmlString();

    // 提交请求
    m_pHttp->submitRequest(message, "/webservices/MobileCodeWS.asmx");
}

void MainWindow::on_Phone_EmailCheck_clicked()
{
    QtSoapMessage message;

    // 设置方法
    message.setMethod("ValidateEmailAddress", "http://WebXml.com.cn/");

    // 设置动作
    m_pHttp->setAction("http://WebXml.com.cn/ValidateEmailAddress");

    // 设置主机
    m_pHttp->setHost("www.webxml.com.cn");

    // 添加方法参数
    QString strPhone = ui->lineEdit_EmailAddress->text();
    message.addMethodArgument("theEmail", "", strPhone);

    QString strXML = message.toXmlString();

    // 提交请求
    m_pHttp->submitRequest(message, "/webservices/ValidateEmailWebService.asmx");
}
