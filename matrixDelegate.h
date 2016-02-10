/*
 * Author: Tashwin Khurana
 *
 * MatrixDelegate: 
 *
 * Inherited from QStyledItemDelegate class;
 * Overloaded paint method inorder to display the edits and 
 * differences while comparing.
 *
 */

#ifndef MATRIXDELEGATE_H
#define MATRIXDELEGATE_H

#include <QStyledItemDelegate>
#include <QPainter>
#include <QBrush>
#include <QStyleOptionViewItemV4>

namespace Ui {
    class MatrixDelegate;
}

class MatrixDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:

    MatrixDelegate(QObject *parent = 0);
    ~MatrixDelegate();
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;  
};

#endif
