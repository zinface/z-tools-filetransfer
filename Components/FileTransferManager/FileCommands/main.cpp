#include "filereceivercommand.h"
#include "filesendercommand.h"

#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDir>
#include <QDir>
#include <QTextStream>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("FileCommands");
    QCoreApplication::setApplicationVersion("v1.0");

    // Common Param
    QCommandLineOption workType(QStringList() << "t" << "type", "指定本程序运行模式(默认: Server)", "Server|Client|Any|Daemon", "Server");
    QCommandLineOption workHost(QStringList() << "s" << "host", "Client模式指定要连接的主机地址(默认: localhost)","localhost");
    QCommandLineOption workPort(QStringList() << "p" << "port", "指定工作端口(默认: 8888)","8888");

    QCommandLineOption workMode(QStringList() << "m" << "mode", "指定文件处理模式(默认: Download、Simple)\n"
                                                                "    Server 模式下可用Simple、Cloud\n"
                                                                "    Client 模式下可用Download、Upload", "Simple|Cloud｜Download|Upload", "Download");
    QCommandLineOption workDir(QStringList() << "d" << "directory", "指定被接收文件存储到该目录(Client默认为当前目录)\n"
                                                                    "    Server Simple模式未指定将不接收文件\n"
                                                                    "    Server Cloud 模式未指定将退出",".");
    QCommandLineOption workFiles(QStringList() << "f" << "files", "指定准备被发送的文件\n"
                                                                  "    Server 模式未指定将退出，如有 -d 开启上传功能将不退出\n"
                                                                  "    Client 模式应配合 --mode Upload 才可进行文件上传","file(s)");


    QCommandLineOption workThreadNums(QStringList() << "n" << "thread-num", "指定工作的线程数量: 1-64(默认为1)\n"
                                                                            "    Server 模式默认为为64", "1-64");
    QCommandLineOption workDownloadSpeed(QStringList() << "download-speed", "Client模式指定下载的速度(默认无限制)", "0");
    QCommandLineOption workIgnoreFileSize(QStringList() << "ignore-size", "Client模式忽略超过指定大小的文件(默认无限制)", "0");
    QCommandLineOption workFilterFileSuffix(QStringList() << "suffix", "Client模式指定文件扩展类型(默认无限制)","png|jpg|txt|...");
    QCommandLineOption workDownloadFinishExit(QStringList() << "done-exit", "Client模式指定下载完成时自动退出(默认一直准备接收)","no|yes");


    // Any
    QCommandLineOption workAny(QStringList() << "any", "Any模式标志以上功能完全可用, finish失效(Any模块默认未开启)\n"
                                                       "    此模式为混合Server、Client模式\n"
                                                       "    可进行功能扩展..."
                                                       ,"no|yes", "no");
    QCommandLineOption workDaemon(QStringList() << "daemon", "以Daemon模式运行，本次执行将会退出(Deamon默认未开启)", "no|yes", "no");
    QCommandLineOption workSupporPlugin(QStringList() << "plugin-support", "以Daemon模式运行时支持动态插件功能(默认未开启)", "no|yes", "no");
    QCommandLineOption workSupporVirtualFeature(QStringList() << "virtual-support", "以Daemon模式运行时支持虚拟逻辑实现(默认未开启)\n"
                                                                                    "    虚拟逻辑实现为客户端默认服务端已有该功能\n"
                                                                                    "    或服务端发现与客户端通信时有相对功能缺失\n"
                                                                                    "    此时将产生虚拟逻辑进行处理动作", "no|yes", "no");

    QCommandLineOption fetchFileList(QStringList() << "list", "查看服务端提供的文件数量");
    QCommandLineOption scanFileServer(QStringList() << "scan", "扫描/发现: 指定端口服务.");
    QCommandLineOption fetchWorkServer(QStringList() << "work", "查看服务端工作目录.");

    QCommandLineOption workDownloadTarget(QStringList() << "target", "Client模式指定要下载的目标","file(s)");
    QCommandLineOption workSearchTarget(QStringList() << "search", "Client模式指定要搜索的目标","file(s)");

    QCommandLineParser parser;
    parser.setApplicationDescription("文件传输CLI命令行版.");
    parser.addHelpOption();
    parser.addVersionOption();

    parser.addOptions(QList<QCommandLineOption>() << workType
                      << workMode
                      << workPort
                      << workDir
                      << workFiles
                      << fetchFileList
                      << scanFileServer
                      << fetchWorkServer
                      << workSearchTarget
                      << workDownloadTarget
                      << workThreadNums
                      << workHost
                      << workDownloadSpeed
                      << workIgnoreFileSize
                      << workFilterFileSuffix
                      << workDownloadFinishExit
                      << workAny
                      << workDaemon
                      << workSupporPlugin
                      << workSupporVirtualFeature);

//    parser.addPositionalArgument("file", "List of file(s) ready to be sent.");
//    parser.addPositionalArgument("dir",  "Received file(s) storage directory");

    parser.process(app);

    bool d = false;
    bool f = false;
    bool m = false;
    bool l = false;
    bool s = false;
    bool w = false;
    bool search = false;
    bool target = false;
    FileSenderCommand fileSender(nullptr);
    FileReceiverCommand fileReceiver(nullptr);

    if (parser.isSet(workPort)) {
        QString port = parser.value(workPort);
        if (port.isEmpty()) goto _noport;
        bool isSuccess;
        int num = port.toInt(&isSuccess);
        if (!isSuccess) goto _noport;
        fileSender.setWorkPort(num);
        fileReceiver.setWorkPort(num);
        QTextStream(stdout) << QString("好家伙,已设置指定工作端口(%1).\n").arg(num);
    } else {
    _noport:
        ;
    }

    if (parser.isSet(fetchFileList) && (l=true)) goto _showHost;
    if (parser.isSet(scanFileServer) && (s=true)) goto _scanHost;
    if (parser.isSet(fetchWorkServer) && (w=true)) goto _workHost;
    if (parser.isSet(workSearchTarget) && (search=true)) goto _workSearchTarget;
    if (parser.isSet(workDownloadTarget) && (target=true)) goto _workDownloadTarget;

    if (parser.isSet(workMode)) {
        QString localValue = parser.value(workMode);
        if (!localValue.isEmpty()) {
            if (localValue == "Cloud" || localValue == "Upload") {
                m = true;
                if (localValue == "Cloud") goto _sender;
                if (localValue == "Upload") goto _client;
            }
            if (localValue == "Simple" || localValue == "Download") goto _defaultworkmode;
            goto _failmode;
        } else {
        _failmode:
            QTextStream(stdout) << QString("好家伙,你乱指定文件处理模式. \n\t -m %1\n").arg(workMode.description());
        }
    } else {
    _defaultworkmode:
        ;
    }

    if (parser.isSet(workType)) {
        QString localValue = parser.value(workType);
        if (!localValue.isEmpty()) {
            if (localValue == "Server") goto _sender;
            if (localValue == "Client") goto _client;
            goto _failwork;
        } else {
        _failwork:
            QTextStream(stdout) << QString("好家伙,你乱指定运行模式. \n\tNOTE: -t %1\n").arg(workType.description());
            return -1;
        }
    } else {
        _sender:
            QTextStream(stdout) << QString("好家伙,你现在处于 Server 模式.\n");
            if (m) {
                fileSender.setWorkMode("Cloud");
                QTextStream(stdout) << QString("好家伙,已指定文件处理模式为: Cloud\n");
            }

            if (parser.isSet(workThreadNums)) {
                QString threadNums = parser.value(workThreadNums);
                if (threadNums.isEmpty()) goto _noserverthreadnums;
                bool isSuccess;
                int num = threadNums.toInt(&isSuccess);
                if (!isSuccess) goto _noserverthreadnums;
                QTextStream(stdout) << QString("好家伙,已设置工作队列数量限制(%1).\n").arg(num);
                if (!fileSender.setWorkThreadNums(num)) {
                    QTextStream(stdout) << QString("好家伙,工作队列数量限制设置失败(将使用最大线程).\n");
                    fileSender.setWorkThreadNums(64);
                }
            } else {
             _noserverthreadnums:
                ;
            }

            if (parser.isSet(workDir)) {
                QString localdir = parser.value(workDir);
                if (localdir.isEmpty()) {
                    QTextStream(stdout) << QString("好家伙,未指定存储目录,将不启用接收文件功能.\n");
                    goto _noserverexistsdir;
                }
                if (!QDir(localdir).exists()) {
                    QTextStream(stdout) << QString("好家伙,指定了不存在的目录(%1),将不启用接收文件功能\n").arg(localdir);
                    goto _noserverexistsdir;
                }
                d = true;
                QTextStream(stdout) << QString("好家伙,已指定存储目录: %1\n").arg(localdir);
                fileSender.setWorkDir(localdir);
            } else {
            _noserverexistsdir:
                fileSender.setWorkDir("");
            }

            if (parser.isSet(workFiles)) {
                QStringList localPositionalArguments;
                localPositionalArguments << parser.value(workFiles);
                localPositionalArguments << parser.positionalArguments();
                if (!localPositionalArguments.size()) goto _nofiles;
                QStringList filepaths;
                qint64 fullsize = 0;
                foreach(QString path, localPositionalArguments) {
                    QFileInfo info(path);
                    if (info.isFile()) {
                        filepaths.append(info.absoluteFilePath());
                        fullsize += info.size();
//                        QTextStream(stdout) << QString(" -- path: %1, size: %2 kb\n").arg(info.fileName()).arg(QString::number((double)info.size()/1024, 'f', 2));
                    }
                }
                if (filepaths.size() == 0) goto _zerofiles;
                f = true;
                QTextStream(stdout) << QString("好家伙, 准备发送 %1 个文件，共计 %2 kb.\n").arg(filepaths.size()).arg(fullsize/1024);
                fileSender.setSenderFiles(filepaths);
            } else {
            _nofiles:
            _zerofiles:
                if (!d && !m)QTextStream(stdout) << QString("好家伙,居然没指定任何可发送文件 \n");
            }

            if (((!parser.isSet(workDir) && !parser.isSet(workFiles)) || (!f && !d)) && !m) {
                QTextStream(stdout) << QString("好家伙,未开启下载及上传功能 \n");
                return -1;
            }

            if (!d && m) {
                QTextStream(stdout) << QString("好家伙,未指定云存储目录 \n");
                return -1;
            }

            fileSender.start();
            return app.exec();



/** ****************************************************************************/
        _client:
            QTextStream(stdout) << QString("好家伙,你现在处于 Client 模式.\n");

            if (m) {
                fileReceiver.setWorkMode("Upload");
                QTextStream(stdout) << QString("好家伙,已指定文件处理模式为: Upload\n");
            }
            _workDownloadTarget:
            if (parser.isSet(workDir)) {
                QString localdir = parser.value(workDir);
                if (localdir.isEmpty()) {
                    if (!m) QTextStream(stdout) << QString("好家伙,未指定存储目录,将以当前目录为存储目录\n");
                    goto _noexistsdir;
                }
                if (!QDir(localdir).exists()) {
                    if (!m) QTextStream(stdout) << QString("好家伙,指定了不存在的目录(%1)\n\t -d %2\n").arg(localdir).arg(workDir.description());
                    return -1;
                }
                d = true;
                if (!m) QTextStream(stdout) << QString("好家伙,已指定存储目录: %1\n").arg(localdir);
                fileReceiver.setWorkDir(localdir);
            } else {
            _noexistsdir:
                fileReceiver.setWorkDir(QDir::currentPath());
                if (!m) QTextStream(stdout) << QString("好家伙,默认以当前目录为存储目录.\n");
            }

            if (parser.isSet(workFiles)) {
                QStringList localPositionalArguments;
                localPositionalArguments << parser.value(workFiles);
                localPositionalArguments << parser.positionalArguments();
                if (!localPositionalArguments.size()) goto _nosendfiles;
                QStringList filepaths;
                qint64 fullsize = 0;
                foreach(QString path, localPositionalArguments) {
                    QFileInfo info(path);
                    if (info.isFile()) {
                        filepaths.append(info.absoluteFilePath());
                        fullsize += info.size();
//                        QTextStream(stdout) << QString(" -- path: %1, size: %2 kb\n").arg(info.fileName()).arg(QString::number((double)info.size()/1024, 'f', 2));
                    }
                }
                if (filepaths.size() == 0) goto _zerosendfiles;
                f = true;
                QTextStream(stdout) << QString("好家伙, 准备发送 %1 个文件，共计 %2 kb.\n").arg(filepaths.size()).arg(fullsize/1024);
                fileReceiver.setSenderFiles(filepaths);
            } else {
            _nosendfiles:
            _zerosendfiles:
                if (m) QTextStream(stdout) << QString("好家伙,居然没指定任何可发送文件 \n");
            }


            _workSearchTarget:
            if (parser.isSet(workSearchTarget)) {
                QStringList localPositionalArguments;
                localPositionalArguments << parser.value(workSearchTarget);
                localPositionalArguments << parser.positionalArguments();
                if (!localPositionalArguments.size()) goto _nosearchfiles;
                QStringList files;
                foreach(QString path, localPositionalArguments) {
                    files.append(path);
                }
                fileReceiver.setSenderFiles(files);
            } else {
            _nosearchfiles:;
            }

            if (parser.isSet(workDownloadTarget)) {
                QStringList localPositionalArguments;
                localPositionalArguments << parser.value(workDownloadTarget);
                localPositionalArguments << parser.positionalArguments();
                if (!localPositionalArguments.size()) goto _nodownloadfiles;
                QStringList files;
                foreach(QString path, localPositionalArguments) {
                    files.append(path);
                }
                fileReceiver.setSenderFiles(files);
            } else {
            _nodownloadfiles:;
            }

            _showHost:
            _scanHost:
            _workHost:
            if (parser.isSet(workHost)) {
                QString host = parser.value(workHost);
                if (host.isEmpty()) goto _nohost;
                if(!fileReceiver.setWorkHost(host)) {
                    QTextStream(stdout) << QString("好家伙,指定主机不可用或未能连接成功\n");
                    return -1;
                }
            } else {
            _nohost:
              fileReceiver.setWorkHost("localhost");
            }

            if (l) {
                fileReceiver.showHostFiles();
                app.exec();
            }
            if (s) {
                fileReceiver.scanHost();
                app.exec();
            }
            if (w) {
                fileReceiver.showHostWork();
                app.exec();
            }
            if (search) {
                fileReceiver.searchHostFile();
                app.exec();
            }
            if (target) {
                fileReceiver.downloadHostFile();
                app.exec();
            }

            if (parser.isSet(workDownloadSpeed)) {
                QString speed = parser.value(workDownloadSpeed);
                if (speed.isEmpty()) goto _nodownspeed;
                bool isSuccess;
                int num = speed.toInt(&isSuccess);
                if (!isSuccess) goto _nodownspeed;
                fileReceiver.setWorkSpeed(num);
                QTextStream(stdout) << QString("好家伙,已设置指定下载的速度: %1\n").arg(num);
            } else {
             _nodownspeed:
                fileReceiver.setWorkSpeed(-1);
            }

            if (parser.isSet(workFilterFileSuffix)) {
                QString suffix = parser.value(workFilterFileSuffix);
                if (suffix.isEmpty()) goto _nofilterfilesuffix;
                fileReceiver.setFilterFileSuffix(suffix);
                QTextStream(stdout) << QString("好家伙,已设置指定文件扩展类型(%1).\n").arg(suffix);
            } else {
            _nofilterfilesuffix:
                ;
            }

            if (parser.isSet(workIgnoreFileSize)) {
                QString filesize = parser.value(workIgnoreFileSize);
                if (filesize.isEmpty()) goto _noignorefilesize;
                bool isSuccess;
                int num = filesize.toInt(&isSuccess);
                if (!isSuccess) goto _noignorefilesize;
                fileReceiver.setIgnoreFileSize(num);
                QTextStream(stdout) << QString("好家伙,已设置超过指定大小的文件(%1).\n").arg(num);
            } else {
             _noignorefilesize:
                ;
            }

            if (parser.isSet(workDownloadFinishExit)) {
                QString doneExit = parser.value(workDownloadFinishExit);
                if (doneExit != "yes") goto _nofinishexit;
                fileReceiver.setDownloadFinishExit(true);
                QTextStream(stdout) << QString("好家伙,已设置下载完成时自动退出(%1).\n").arg(doneExit);
            } else {
            _nofinishexit:
                ;
            }

            if (parser.isSet(workThreadNums)) {
                QString threadNums = parser.value(workThreadNums);
                if (threadNums.isEmpty()) goto _nothreadnums;
                bool isSuccess;
                int num = threadNums.toInt(&isSuccess);
                if (!isSuccess) goto _nothreadnums;
                QTextStream(stdout) << QString("好家伙,已设置同时下载数量限制(%1).\n").arg(num);
                if (!fileReceiver.setWorkDownloadThreadNums(num)) {
                    QTextStream(stdout) << QString("好家伙,下载数量限制设置失败(将使用单线程)\n");
                    fileReceiver.setWorkDownloadThreadNums(1);
                }
            } else {
             _nothreadnums:
                ;
            }

            // 以Daemon模式运行
            if (parser.isSet(workDaemon)) {
                QString asDaemon = parser.value(workDaemon);
                if (asDaemon.isEmpty()) goto _nodaemon;
                QTextStream(stdout) << QString("好家伙,已设置Daemon运行模式\n");
            } else {
            _nodaemon:
                ;
            }

            //
            if (parser.isSet(workSupporPlugin)) {
                QString asDaemon = parser.value(workSupporPlugin);
                if (asDaemon.isEmpty()) goto _nosupportplugin;
                QTextStream(stdout) << QString("好家伙,已设置支持插件模式\n");
            } else {
            _nosupportplugin:
                ;
            }

            if (parser.isSet(workSupporVirtualFeature)) {
                QString asDaemon = parser.value(workSupporVirtualFeature);
                if (asDaemon.isEmpty()) goto _nosupportvirtualimpl;
                QTextStream(stdout) << QString("好家伙,已设置支持虚拟逻辑实现模式\n");
            } else {
            _nosupportvirtualimpl:
                ;
            }

            if (!f && m) {
                QTextStream(stdout) << QString("好家伙,未指定上传文件 \n");
                return -1;
            }

            fileReceiver.start();
            return app.exec();
    }
}


/*********************************************
得想想法子：
Usage: ./FileCommands [options]
文件传输CLI命令行版.

Options:
  -h, --help                                 Displays this help.
  -v, --version                              Displays version information.
  -t, --type <Server|Client|Any|Daemon>      指定本程序运行模式(默认: Server)
  -m, --mode <Simple|Cloud｜Download|Upload>  指定文件处理模式(默认: Download、Simple)
                                                Server 模式下可用Simple、Cloud
                                                Client 模式下可用Download、Upload
  -p, --port <8888>                          指定工作端口(默认: 8888)
  -d, --directory <.>                        指定被接收文件存储到该目录(Client默认为当前目录)
                                                Server Simple模式未指定将不接收文件
                                                Server Cloud 模式未指定将退出
  -f, --files <file(s)>                      指定准备被发送的文件
                                                Server 模式未指定将退出，如有 -d 开启上传功能将不退出
                                                Client 模式应配合 --mode Upload
                                             才可进行文件上传
  --list                                     查看服务端提供的文件数量
  -n, --thread-num <1-64>                    指定工作的线程数量: 1-64(默认为1)
                                                Server 模式默认为为64
  -s, --host <localhost>                     Client模式指定要连接的主机地址(默认: localhost)
  --download-speed <0>                       Client模式指定下载的速度(默认无限制)
  --ignore-size <0>                          Client模式忽略超过指定大小的文件(默认无限制)
  --suffix <png|jpg|txt|...>                 Client模式指定文件扩展类型(默认无限制)
  --done-exit <no|yes>                       Client模式指定下载完成时自动退出(默认一直准备接收)
  --any <no|yes>                             Any模式标志以上功能完全可用,
                                             finish失效(Any模块默认未开启)
                                                此模式为混合Server、Client模式
                                                可进行功能扩展...
  --daemon <no|yes>                          以Daemon模式运行，本次执行将会退出(Deamon默认未开启)
  --plugin-support <no|yes>                  以Daemon模式运行时支持动态插件功能(默认未开启)
  --virtual-support <no|yes>                 以Daemon模式运行时支持虚拟逻辑实现(默认未开启)
                                                虚拟逻辑实现为客户端默认服务端已有该功能
                                                或服务端发现与客户端通信时有相对功能缺失
                                                此时将产生虚拟逻辑进行处理动作


好家伙，我任务管理器还是个架子，还没完成呢--- 已完成，有暴力上传处理bug
*********************************************/




