#ifndef FILETRANSFERSENDER_H
#define FILETRANSFERSENDER_H

#include <QElapsedTimer>
#include <QFile>
#include <QProgressBar>
#include <QQueue>
#include <QTcpServer>
#include <QTime>
#include <QTimer>
#include <QWidget>

class QLabel;
class QListWidget;
class QPushButton;
class FileSenderView;
class FileSenderModel;
class FileSessionAdapter;
class FileTransferManager;

class FileTransferSender : public QWidget
{
    Q_OBJECT
public:
    explicit FileTransferSender(QWidget *parent = nullptr);
     ~FileTransferSender();

private slots:
    /***** manager -> clients *****/
    void onClientChanged(int count);
    void onClientFetchFileList(QTcpSocket *c);
    void onClientFetchFile(QTcpSocket *c, QString filename);

    /***** fileCtlBtns ******/
    void addFile();
    void delFile();
    void clrFile();
    void sendFile();

    /***** fileListWidget *****/
    void onfilesAppended(QStringList& filepaths);
    void onfilesDeleted(QString filepath);
    void onfilesCleanded();
    void onFileListChanged();

    /***** fileQueue *****/
    void onFilesQueueChange();

    /***** tcpBytesWritten *****/
    void onBytesWritten(const qint64 &bytes);
    void send();
    void reset();

signals:
    void clientChanged();

    void filesAppended(QStringList& filepaths);
    void filesDeleted(QString filepath);
    void filesCleanded();

    void filesChanged();
    void emitFilesQueueChange();

private:
    FileTransferManager *manager;

    QQueue<QString> m_fileQueue;
    QFile m_file;
    QDataStream m_stream;

private:
    FileSenderView *fileListView;
    qint64 m_readBlock;
    qint64 m_currentFileSize;
    qint64 m_totalFileSize;
    qint64 m_currentFileBytesWritten;
    qint64 m_totalFileBytesWritten;

    QPushButton *addFileBtn;
    QPushButton *delFileBtn;
    QPushButton *clrFileBtn;
    QPushButton *sendFileBtn;

    /********* Status Part ************/
    QLabel *currentLab;
    QLabel *totalLab;
    QProgressBar currentProgressBar;
    QProgressBar totalProgressBar;

    QLabel *listenPort;
    QLabel *listenStatus;
    QLabel *clientStatus;
    QLabel *filesQueueStatus;
    QLabel *currentSendSpeed;

private:
    void createFileTransferSender();
    void updateProgressBar(int size);
    bool compareQListWidgetItem(QString b);
    bool compareQStringList(QStringList a, QString b){
        foreach (QString s, a) {
            if (compareQString(s, b)){
                return true;
            }
        }
        return false;
    }
    bool compareQString(QString a, QString b){
        return a.compare(b) == 0;
    }

    // QWidget interface
protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);

public:
    QElapsedTimer timer;
};

#endif // FILETRANSFERSENDER_H
