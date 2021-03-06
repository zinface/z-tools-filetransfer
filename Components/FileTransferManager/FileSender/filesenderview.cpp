#include "filesenderitemdelegate.h"
#include "filesendermodel.h"
#include "filesenderview.h"

FileSenderView::FileSenderView(QWidget *parent) : QListView(parent)
  ,mfileSenderItemDelegate(new FileSenderItemDelegate)
  ,mfileSenderModel(new FileSenderModel)
  ,currentRow(-1)
{
    this->setItemDelegate(mfileSenderItemDelegate);
    this->setModel(mfileSenderModel);

    connect(this, &QListView::clicked, this, &FileSenderView::onclicked);
}

void FileSenderView::appendFilse(const QStringList &filepaths)
{
    this->mfileSenderModel->appendFilse(filepaths);
}

void FileSenderView::filesDeleted(QString filepath)
{
    this->mfileSenderModel->filesDeleted(filepath);
}

void FileSenderView::filesCleanded()
{
    this->mfileSenderModel->filesCleanded();
}

void FileSenderView::onclicked(const QModelIndex &index)
{
    currentRow = index.row();
}

int FileSenderView::count()
{
    return this->mfileSenderModel->count();
}

FileItemInfo *FileSenderView::item(int i)
{
    return this->mfileSenderModel->item(i);
}

FileSenderModel *FileSenderView::getModel()
{
    return this->mfileSenderModel;
}
