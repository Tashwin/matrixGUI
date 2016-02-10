#include "matrixDelegate.h"
#include <QDebug>

//Class Contructor
MatrixDelegate::MatrixDelegate(QObject *parent)
        : QStyledItemDelegate(parent)
{
}

//Class Destructor
MatrixDelegate::~MatrixDelegate()
{
}

//Overloads the paint method for QStyledItemDelegate
//Provides Highlight support for editing and comparing 
//data, input from MatrixModel
void MatrixDelegate::paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
    //Data at the index:
    //  List of String: [value, 
    //                   1/0 value -> edited/unedited, 
    //                   1/0 value -> comparison result/not]
    QStringList inData = index.model()->data(index.model()->index(index.row(), index.column())).toStringList();
    bool edited = inData[1] =="1";
    bool diffData = inData[2] == "1";
    QString inDataNum = inData[0];

    QStyleOptionViewItem opt = option;
    
    opt.state &= ~QStyle::State_HasFocus;
    painter->save();
    //Set color for the cell according to the conditions    
    if (diffData)
        painter->fillRect( opt.rect, Qt::green );
    else if (edited)
        painter->fillRect( opt.rect, Qt::yellow );
    else
        painter->fillRect( opt.rect, Qt::white );
    //Draw text in Bold
    QFont f;
    f.setBold(true);
    painter->setFont(f); 
    painter->drawText(QRect(opt.rect.left(), opt.rect.top(), opt.rect.width(), opt.rect.height()/2),
                      opt.displayAlignment, inDataNum);
    
    painter->restore();
//    qDebug() << inData;
}

