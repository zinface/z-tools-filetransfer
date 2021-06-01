#ifndef FILETRANSFERMANAGER_H
#define FILETRANSFERMANAGER_H

#include "sessionmanager.h"
#include "../fileiteminfo.h"

#include <QObject>
#include <QTcpSocket>

class FileTransferTaskManager;

class FileTransferManager : public QObject
{
    Q_OBJECT
public:
    explicit FileTransferManager(QObject *parent = nullptr);
    ~FileTransferManager();

    enum FullEvent{
        OP_ALL = 1,
        OP_UPLOAD,
        OP_DOWNLOAD,
        S_APPEND,
        S_DELETE,
        S_CLEANR,
    };

    void setManagerTask(QString host, int port, SessionManager::SessionManagerWorkType type = SessionManager::SERVER);
    void setSavePath(QString path) {this->savePath = path;}
    void setMaxTaskToggetherRunningCount(int count = 5);

    bool state();
    bool serverState();
    bool clientState();

    // Server Public Function
    void pushFileAppend(QString filename, qint64 filesize);
    void pushFileDeleted(QString filename);
    void pushFileDeleted(QStringList filenames);
    void pushFileClaer();

    // Reply Action
    void broadCaseAction(QTcpSocket *c, FullEvent e, QString fileName, qint64 fileSize, QString filePath);
    void broadCaseAction(QTcpSocket *c, FullEvent e, QString filename);
    void broadCaseAction(QTcpSocket *c, FullEvent e, QString filename, qint64 filesize);

    // Client Actions
    void fetchFileListAction();              // Base Action
    void fetchFileAction(const QString filename, qint64 filesize, const QString savePath);       // CLI Action
    void fetchFileItemInfoAction(const FileItemInfo &fileinfo);  // Donwload

private:
    SessionManager *adapter;
    SessionManager::SessionManagerWorkType _manager_work;

    FileTransferTaskManager *taskManager;
    QString savePath;

private slots:
    void onNewAction(QTcpSocket *c);

signals:
    /** Manager Signals******/
    void clientCountChanged(int count);
    void newConnectSocket(QTcpSocket *c);
    void onRemoteFetchFileList(QTcpSocket *c);
    void onRemoteFetchFile(QTcpSocket *c, QString filename);

    void connected();
    void readRead();
    void disconnected();

    /** Manager Actions */
    void onRemoteFileAppend(const QString &filename, qint64 filesize);
    void onRemoteFileDelete(const QString &filename);
    void onRemoteFileClear();


    /** Session Signals **/
    void ClientSocketConnected();
    void ClientSocketConnecting();
    void ClientSocketUnConnected();

};

#endif // FILETRANSFERMANAGER_H
