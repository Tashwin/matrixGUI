#include "matrixwindow.h"
#include "ui_matrixwindow.h"

//Class Constructor
MatrixWindow::MatrixWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MatrixWindow)
{
    ui->setupUi(this);
    this->setupUI();
    ui->groupBox_2->setHidden(true);
}

//Class Destructor
MatrixWindow::~MatrixWindow()
{
    if (model)
        delete model;
    if (model_2)
        delete model_2;
    delete ui;
}

// Setup model and delegate for tableview and 
// update the window components
// Connect notifier model signal to the allocated
// slot
void MatrixWindow::setupUI()
{
    model = new MatrixModel(this);
    model_2 = new MatrixModel(this);

    ui->tableView->setItemDelegate(&delegate);
    ui->tableView_2->setItemDelegate(&delegate_2);

    ui->tableView->setModel(model);
    ui->tableView_2->setModel(model_2);

    ui->tableView->setHidden(true);
    ui->tableView_2->setHidden(true);
    ui->groupBox_2->setVisible(false);

    ui->next->setEnabled(false);
    ui->prev->setEnabled(false);
    ui->next_2->setEnabled(false);
    ui->prev_2->setEnabled(false);

    ui->saveChanges->setEnabled(false);
    ui->saveChanges_2->setEnabled(false);

    connect(model, SIGNAL(notifyUser(QString)), this, SLOT(onError(QString)));
    connect(model_2, SIGNAL(notifyUser(QString)), this, SLOT(onError(QString)));
}

// Slot for upload button click
// Uploads csv matrix file to table
void MatrixWindow::on_upload_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                    "./test_samples",
                                                    tr("csv Files(*.csv)"));
    if (fileName != "")
    {
        ui->next->setEnabled(true);
        ui->prev->setEnabled(true);
        ui->saveChanges->setEnabled(true);
        ui->editRadio->setChecked(true);
        model->setFile(fileName);
        ui->tableView->setVisible(true);
        ui->tableView->horizontalHeader()->setStretchLastSection(true);
    }
}

// Slot for upload button click
// Uploads csv matrix file to table
void MatrixWindow::on_upload_2_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                    "./test_samples",
                                                    tr("csv Files(*.csv)"));
    if (fileName != "")
    {
        ui->next_2->setEnabled(true);        
        ui->prev_2->setEnabled(true);
        ui->saveChanges_2->setEnabled(true);
        ui->editRadio->setChecked(true);
        model_2->setFile(fileName);
        ui->tableView_2->setVisible(true);
        ui->tableView_2->horizontalHeader()->setStretchLastSection(true);
    }
}

// Save chages to file loded in table1
void MatrixWindow::on_saveChanges_clicked()
{
    model->saveDataCsv(ui->closeFileOnSave->isChecked());
}

// Save chages to file loded in table1
void MatrixWindow::on_saveChanges_2_clicked()
{
    model_2->saveDataCsv(ui->closeFileOnSave->isChecked());
}

// Checkbox response for loading multiple matrices
// When checked table2 becomes accesible from the window
// Also, enables comparison
void MatrixWindow::on_loadMultiple_clicked(bool checked)
{
    if (checked)
    {
        ui->compareRadioButton->setEnabled(true);

        if (ui->tableView_2->isHidden())
        {
            ui->groupBox_2->setVisible(true);
            ui->tableView_2->setHidden(true);
        }
        else
            ui->groupBox_2->setVisible(true);
    }
    else
    {
        ui->compareRadioButton->setEnabled(false);
        ui->groupBox_2->setVisible(false);
    }
}

//Enables File Editing, when checked
void MatrixWindow::on_editRadio_toggled(bool checked)
{
    if (checked)
    {

        ui->next_2->setEnabled(true);
        ui->prev_2->setEnabled(true);

        model->showComparison(false);
        model_2->showComparison(false);
        model->layoutChanged();
        model_2->layoutChanged();
    }
}

//Enables comparison between 2 matrices with
//similar dimensions
void MatrixWindow::on_compareRadioButton_clicked(bool checked)
{
    if (checked)
    {
        ui->next_2->setEnabled(false);
        ui->prev_2->setEnabled(false);

        model->showComparison(true);
        model_2->showComparison(true);
        if (!model->compareModels(*model_2))
            ui->editRadio->setChecked(true);
    }
}

//Allows auto scrolling of table2 according to table1
//scroll control while comparing
void MatrixWindow::on_tableView_entered(const QModelIndex &index)
{
    if (ui->compareRadioButton->isChecked() && ui->tableView_2->model()->hasIndex(index.row(),index.column()))
    {
        ui->tableView_2->scrollTo(ui->tableView_2->model()->index(index.row(),index.column()),QAbstractItemView::PositionAtCenter);
    }
}

//Allows auto scrolling of table1 according to table2
//scroll control while comparing
void MatrixWindow::on_tableView_2_entered(const QModelIndex &index)
{
    if (ui->compareRadioButton->isChecked() && ui->tableView->model()->hasIndex(index.row(),index.column()))
    {
        ui->tableView->scrollTo(ui->tableView->model()->index(index.row(),index.column()),QAbstractItemView::PositionAtCenter);
    }
}

// Slot for notification signal
void MatrixWindow::onError(const QString &msg)
{
    ui->notifier->setText(msg);
}

//Load prev block of data
void MatrixWindow::on_prev_clicked()
{
    model->loadPrev();
    this->on_compareRadioButton_clicked(ui->compareRadioButton->isChecked());
}

//Load next block of data
void MatrixWindow::on_next_clicked()
{
    model->loadNext();

    this->on_compareRadioButton_clicked(ui->compareRadioButton->isChecked());
}

//Load prev block of data
void MatrixWindow::on_prev_2_clicked()
{

    model_2->loadPrev();
    this->on_compareRadioButton_clicked(ui->compareRadioButton->isChecked());
}

//Load next block of data
void MatrixWindow::on_next_2_clicked()
{
    model_2->loadNext();

    this->on_compareRadioButton_clicked(ui->compareRadioButton->isChecked());
}
