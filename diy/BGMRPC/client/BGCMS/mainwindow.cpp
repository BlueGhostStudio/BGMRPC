#include "mainwindow.h"
#include <QListView>
#include <QTableView>
#include <QHeaderView>
#include <QInputDialog>
#include <QDebug>
#include <QMainWindow>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)//, dlgContent (this)
{
    setupUi(this);

    lvCollections->setModel (&CollectionsModel);
    lvContents->setModel (&ContentsModel);
    loadCollections ();

    lvTagFilter->setModel (&CollTagsModel);
    //dlgContent.setTagSelectModel (&CollTagsModel);

    splitter0->setStretchFactor (0, 1);
    splitter0->setStretchFactor (1, 4);
    splitter1->setStretchFactor (0, 5);
    splitter1->setStretchFactor (1, 1);
    
    CollectionsModel.setColumnCount (5);
    lvCollections->setModelColumn (1);

    initialRemoteSignal ();
    QObject::connect (this, &MainWindow::cmsObjChanged,
                      [=]() {
        actionAdd_Content->setEnabled (!CurrentCmsObj.isEmpty ());
        actionAdd_Image->setEnabled (!CurrentCmsObj.isEmpty ());
    });
    QObject::connect (lvContents->selectionModel (),
                      &QItemSelectionModel::selectionChanged,
                      [=](const QItemSelection &selected,
                          const QItemSelection &deselected) {
        ContentsSelection.merge (selected, QItemSelectionModel::Select);
        ContentsSelection.merge (deselected, QItemSelectionModel::Deselect);
        actionRemove_Content->setEnabled (
                    ContentsSelection.indexes ().count () > 0);
        actionRearrange->setEnabled (
                    ContentsSelection.indexes ().count () > 0);
    });
    QObject::connect (lvCollections->selectionModel (),
                      &QItemSelectionModel::selectionChanged,
                      [=](const QItemSelection &selected) {
        if (selected.count () > 0) {
            setCurrentCmsObj (CollectionsModel.item (
                                  selected.indexes ()[0].row (),2)
                    ->data (Qt::DisplayRole).toString ());
        }
        actionRemove_Collection->setEnabled (selected.count ());
        actionRename_Collection->setEnabled (selected.count ());
    });
    QObject::connect (&ContentsModel, &QStandardItemModel::modelReset,
                      [=]() {
        actionRemove_Content->setEnabled (false);
        actionRearrange->setEnabled (false);
    });
    QObject::connect (lvTagFilter->selectionModel (),
                      &QItemSelectionModel::selectionChanged,
                      [=](const QItemSelection& selected,
                          const QItemSelection& deselected) {
        TagFilterSelection.merge (selected, QItemSelectionModel::Select);
        TagFilterSelection.merge (deselected, QItemSelectionModel::Deselect);

        QJsonArray tagsArg;
        foreach (QModelIndex index, TagFilterSelection.indexes ())
            tagsArg.append (index.data ().toString ());

        loadContents (CurrentCmsObj, tagsArg, false);
    });
}

void MainWindow::loadCollections()
{
    CollectionsModel.clear ();
    QVariantMap ret = RPC->callMethod ("CMS", "js",
    {"listCollections"}).toMap ();
    QVariantList collList = ret["rows"].toList ();
    for (int row = 0; row < collList.size (); ++row) {
        QVariantMap rowData = collList[row].toMap ();

        CollectionsModel.appendRow (collectionRow (rowData));
    }
}

void MainWindow::loadContents(const QString& cmsObj,
                              const QJsonArray& tags, bool resetTagsList)
{
    if (cmsObj.isEmpty ())
        return;
    QVariantList ret = RPC->callMethod (cmsObj, "js",
    {"listContents", tags}).toList ();

    if (ret.count () == 0)
        return;

    if (resetTagsList) {
        CollTagsModel.setStringList (ret[1].toStringList ());
        TagFilterSelection.clear ();
    }

    setWindowTitle ("BGCMS - "
                    + lvCollections->currentIndex ().data ().toString ());
    ContentsModel.clear ();
    ContentsModel.setColumnCount (5);
    ContentsModel.setHorizontalHeaderItem (0, new QStandardItem("Id"));
    ContentsModel.setHorizontalHeaderItem (1, new QStandardItem("#"));
    ContentsModel.setHorizontalHeaderItem (2, new QStandardItem("Title"));
    ContentsModel.setHorizontalHeaderItem (3, new QStandardItem("Date"));
    ContentsModel.setHorizontalHeaderItem (4, new QStandardItem("Author"));
    ContentsModel.setHorizontalHeaderItem (5, new QStandardItem("Tag"));
    
    QHeaderView* contentsHeader = lvContents->horizontalHeader ();
    contentsHeader->setSectionResizeMode (QHeaderView::ResizeToContents);
    contentsHeader->setSectionResizeMode (2, QHeaderView::Stretch);
    contentsHeader->setSectionHidden (0, true);
    
    QVariantList contentList = ret[0].toMap ()["rows"].toList ();
    //QVariantList contentList = ret["rows"].toList ();
    for (int row = 0; row < contentList.size (); ++row) {
        QVariantMap rowData = contentList[row].toMap ();
        ContentsModel.appendRow (contentRow (rowData));
        
        //appendContentRow (rowData);
    }
    ContentsModel.sort (1);
    ContentsModel.sort (5);
}

void MainWindow::on_lvContents_doubleClicked(const QModelIndex &index)
{
    if (CurrentCmsObj.isEmpty ())
        return;
    
    ContentDialog dlgContent (this);
    dlgContent.setTagSelector (CollTagsModel.stringList ());
    
    QStandardItem* idItem = ContentsModel.item (index.row (), 0);
    dlgContent.openContent (CurrentCmsObj,
                            idItem->data (Qt::DisplayRole).toInt ());
}

void MainWindow::on_actionAdd_Content_triggered()
{
    QVariant ret = RPC->callMethod (CurrentCmsObj, "js",
    {"addContent", "No Name", "no Content"});
}

void MainWindow::on_actionAdd_Image_triggered()
{
    QFile imgFile ("/home/bgstudio/Pictures/logo.png");
    imgFile.open (QIODevice::ReadOnly);
    qDebug () << RPC->callMethod (CurrentCmsObj, "js", {
                                      "addImage", "logo.png",
                                      QString (imgFile.readAll ().toBase64 ())
                     });
}

void MainWindow::setCurrentCmsObj(const QString& cmsObj)
{
    CurrentCmsObj = cmsObj;
    loadContents (CurrentCmsObj);
    cmsObjChanged ();
}

void MainWindow::on_actionRemove_Content_triggered()
{
    if (QMessageBox::question (this, "Remove Contents",
                               "Are you sure you want to "
                               "delete the selected content?")
            == QMessageBox::Yes) {
        QList < int > willRemove;
        foreach (QModelIndex item, ContentsSelection.indexes ()) {
            if (item.column () == 0) {
                willRemove << item.data () .toInt ();
            }
        }

        foreach (int index, willRemove) {
            QVariant ret = RPC->callMethod (CurrentCmsObj, "js",
            {"removeContent", index });
        }
    }
}

void MainWindow::on_actionRearrange_triggered()
{
    int currentRow = lvContents->currentIndex ().row ();
    int orgSeq = ContentsModel.item (currentRow, 1)
                 ->data (Qt::DisplayRole).toInt ();
    int cntID = ContentsModel.item (currentRow, 0)
                ->data (Qt::DisplayRole).toInt ();
    int maxSeq = ContentsModel.item (ContentsModel.rowCount () - 1, 1)
                 ->data (Qt::DisplayRole).toInt ();
    int seq = QInputDialog::getInt(this, tr("Rearrange"),
                               tr("Sequence:"), orgSeq, 1, maxSeq);
    if (seq != orgSeq) {
        RPC->callMethod (CurrentCmsObj, "js", {"rearrange", cntID, seq});
        ContentsModel.sort (1);
    }
}

void MainWindow::on_actionAdd_Collection_triggered()
{
    QString collTitle = QInputDialog::getText (this, tr("New Collection"),
                                               tr("Name"));
    if (!collTitle.isEmpty ())
        RPC->callMethod ("CMS", "js", {"createCollection", collTitle});
}

void MainWindow::on_actionRemove_Collection_triggered()
{
    if (QMessageBox::question (this, "Remove Collection",
                               "Are you sure you want to "
                               "delete the selected collection?")
            == QMessageBox::Yes) {
        int collID = CollectionsModel.item (
                         lvCollections->currentIndex ().row ()
                         )->data (Qt::DisplayRole).toInt ();
        RPC->callMethod ("CMS", "js", {"removeCollection", collID});
    }
}

void MainWindow::on_actionRename_Collection_triggered()
{
    QString collTitle
            = QInputDialog::getText (
                  this, tr("Rename"),
                  tr ("Name"),
                  QLineEdit::Normal,
                  lvCollections->currentIndex ().data ().toString ());

    if (!collTitle.isEmpty ()) {
        int collID = CollectionsModel.item (
                         lvCollections->currentIndex ().row ()
                         )->data (Qt::DisplayRole).toInt ();
        qDebug () << RPC->callMethod ("CMS", "js",
            { "renameCollection", collID, collTitle });
    }
}

void MainWindow::initialRemoteSignal()
{

    QObject::connect (RPC, &BGMRPCClient::remoteSignal,
                      [=] (const QString& obj,const QString& sig,
                      const QJsonArray& args) {
        if (obj == CurrentCmsObj) {
            if (sig == "added")
                //appendContentRow (args.toVariantList ()[0].toMap ());
                ContentsModel.appendRow (
                            contentRow (args.toVariantList ()[0].toMap ())
                        );
            else if (sig == "updated") {
                QVariantMap rowData = args.toVariantList ()[2].toMap ();
                int row = ContentsModel.findItems (
                              QString::number (args[0].toInt ())
                          )[0]->row ();
                ContentsModel.item (row, 2)->setData (rowData["title"],
                        Qt::DisplayRole);
            } else if (sig == "tagUpdated") {
                CollTagsModel.setStringList (
                            args[2].toVariant ().toStringList ());

                int row = ContentsModel.findItems (
                              QString::number (args[0].toInt ())
                          )[0]->row ();
                ContentsModel.item (row, 5)->setData (args[1].toString (),
                        Qt::DisplayRole);

                //dlgContent.updateTags (obj, args);
            } else if (sig == "removed") {
                QList<QStandardItem *> removedItem
                        = ContentsModel.findItems (
                              QString::number (args[0].toInt ()));
                if (removedItem.count () > 0) {
                    ContentsModel.removeRow (removedItem[0]->row ());
                }
            } else if (sig == "rearranged") {
                QJsonArray arranged = args[0].toArray ();
                for (int i = 0; i < ContentsModel.rowCount (); ++i) {
                    int id = ContentsModel.item (i)->data (Qt::DisplayRole)
                             .toInt ();
                    for (int j = 0; j < arranged.count (); j++) {
                        QJsonObject arrItem = arranged[j].toObject ();
                        if (id == arrItem["id"].toInt ()) {
                            ContentsModel.item (i, 1)
                                    ->setData (arrItem["seq"].toInt (),
                                        Qt::DisplayRole);
                            arranged.removeAt (j);
                            break;
                        }
                    }
                }
                ContentsModel.sort (1);
            }
        } else if (obj == "CMS") {
            if (sig == "created")
                CollectionsModel.appendRow (
                            collectionRow (args.toVariantList ()[0].toMap ())
                        );
            else if (sig == "removed") {
                qDebug () << CollectionsModel.rowCount ();
                QList < QStandardItem* > removedItem
                        = CollectionsModel.findItems (
                              QString::number (args[0].toInt ()));
                if (removedItem.count () > 0) {
                    int rmRow = removedItem[0]->row ();
                    int currentRow = lvCollections->currentIndex ().row ();
                    CollectionsModel.removeRow (rmRow);
                    if (rmRow == currentRow) {
                        CurrentCmsObj = "";
                        lvCollections->clearSelection ();
                    }
                }
            } else if (sig == "renamed") {
                QList < QStandardItem* > renamedItem
                        = CollectionsModel.findItems (
                              QString::number (args[0].toInt ()));
                if (renamedItem.count () > 0)
                    CollectionsModel.item (renamedItem[0]->row (),
                            1)->setData (args[1].toString (),
                            Qt::DisplayRole);
            }
        }
    });
}

QList<QStandardItem*> MainWindow::collectionRow(const QVariantMap& rowData) const
{
    QStandardItem* idItem = new QStandardItem ();
    idItem->setData (rowData["id"], Qt::DisplayRole);

    QStandardItem* titleItem = new QStandardItem ();
    titleItem->setData (rowData["title"], Qt::DisplayRole);

    QStandardItem* cmsObjItem = new QStandardItem ();
    cmsObjItem->setData (rowData["cmsObj"], Qt::DisplayRole);

    QStandardItem* authorItem = new QStandardItem ();
    authorItem->setData (rowData["author"], Qt::DisplayRole);

    return { idItem, titleItem, cmsObjItem, authorItem };
}

QList<QStandardItem*> MainWindow::contentRow(const QVariantMap& rowData) const
{
    QStandardItem* idItem = new QStandardItem ();
    idItem->setData (rowData["id"], Qt::DisplayRole);

    QStandardItem* titleItem = new QStandardItem ();
    titleItem->setData (rowData["title"], Qt::DisplayRole);

    QStandardItem* dateItem = new QStandardItem ();
    dateItem->setData (rowData["date"], Qt::DisplayRole);

    QStandardItem* authorItem = new QStandardItem ();
    authorItem->setData (rowData["author"], Qt::DisplayRole);

    QStandardItem* seqItem = new QStandardItem ();
    seqItem->setData (rowData["seq"], Qt::DisplayRole);

    QStandardItem* tagItem = new QStandardItem ();
    tagItem->setData (rowData["tag"], Qt::DisplayRole);

    return {idItem, seqItem, titleItem, dateItem, authorItem, tagItem};
}

