/*
 * Author: Tashwin Khurana
 *
 * MatrixModel:
 *  Inherits from QAbstractTableModel;
 *  Consists of overloaded function to handle 
 *  loading in matrices using csv files;
 *  Supports Highlighting edits to the file;
 *  Saves the editted file to the disk
 *  Compares data from other models;
 *
 * Reference: Model/View Tutorial for Qt5.5
 */

#ifndef MATRIXMODEL_H
#define MATRIXMODEL_H

#include <QFile>
#include <QBitArray>
#include <QAbstractTableModel>
#include <QPair>
#include <QByteArrayList>
#include <stdio.h>

namespace Ui {
    class MatrixModel;
}

class MatrixModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    MatrixModel(QObject *parent);
    MatrixModel(QString csvFile, QObject *parent);
    ~MatrixModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE ;
    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

    bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole) Q_DECL_OVERRIDE;
    Qt::ItemFlags flags(const QModelIndex & index) const Q_DECL_OVERRIDE ;

    void setFile(QString csvFile);
    void saveDataCsv(bool close=true);
    
    void showComparison(bool compare);
    bool compareModels(MatrixModel &model);
    bool setComparisonData( QList<QBitArray> cD);
    
    QList<QList<QByteArray> > getData(int rowStart, int rowEnd);
    bool loadNext();
    bool loadPrev();

    void clear();

private:

    void setTableDataFromFile();
    bool isStrListOfDoubles(QList<QByteArray> list);

    void loadRequiredRowData(int rowStart, int rowEnd=-1);
    void setRowBeginings();
    QList< QList<QByteArray> > getRow(int row,  int numRows= 1);

signals:

    void notifyUser(const QString &);

private:
    int _rows, _columns;

    QList< QList< QByteArray> > m_gridData;

    QHash<QPair<int, int>, QByteArray > editedGridData;

    QList< QBitArray > changedData, diffData;

    QList< qlonglong> rowList;

    int currentMinRow;

    QString _fileName;

    bool _enableComparison;

    const int  MAX_ROWS_DISPLAYED, DISPLAY_BUFFER;
};

#endif // MATRIXMODEL_H
