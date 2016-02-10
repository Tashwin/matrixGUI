#include "matrixmodel.h"


//Class Contructor
MatrixModel::MatrixModel(QObject *parent)
    :QAbstractTableModel(parent),
      _enableComparison(false),
      MAX_ROWS_DISPLAYED(100),
      DISPLAY_BUFFER(10)
{
}

//Class Contructor
MatrixModel::MatrixModel(QString csvFilename, QObject *parent)
    :QAbstractTableModel(parent),
      _fileName(csvFilename),
      _enableComparison(false),
      MAX_ROWS_DISPLAYED(100),
      DISPLAY_BUFFER(10)
{
    this->setTableDataFromFile();
}

//Class Destructor
MatrixModel::~MatrixModel()
{
}

//Returns the number of rows in the model at a given time
int MatrixModel::rowCount(const QModelIndex & /*parent*/) const
{
   int row = 0;
   if (!m_gridData.isEmpty())
      row = m_gridData.size();
   return row;
}

//Returns the number of columns in the model at a given time
int MatrixModel::columnCount(const QModelIndex & /*parent*/) const
{
    int col = 0;
    if (!m_gridData.isEmpty())
       col = m_gridData[0].size();
    return col;

}

//Returns the data for the DisplayRole in the model
QVariant MatrixModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole)
    {
        //Return StringList containing:
        //0-> data value
        //1-> 1/0 depending on whether the data is edited
        //2-> 1/0 depending on whether the data is different
        //      from input model
        if (!(m_gridData.size() <= index.row()))
        {
            QStringList output;
            output << m_gridData[index.row()][index.column()];
            output << (changedData[index.row()+currentMinRow][index.column()]?"1":"0");
            output << ((diffData[index.row()+currentMinRow][index.column()]&&_enableComparison)?"1":"0");
            return output;
        }
        else
        {
            //place Holder
            QStringList dummy;
            dummy << " " << "0" << "0";
            return dummy;
        }
    }
    return QVariant();
}

// Set header columns/ rows indices
QVariant MatrixModel::headerData(int section, Qt::Orientation orientation,
                                 int role /*Qt::DisplayRole*/) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation==Qt::Horizontal)
            return section;
        else if (orientation==Qt::Vertical)
            return section + currentMinRow;
    }
    return QVariant();
}

//Returns True, if input data value is set for an index
//Returns False, if input data value is not set
bool MatrixModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
    if (role == Qt::EditRole && !_enableComparison)
    {
        bool ok;
        QString valueInStr = value.toString();
        valueInStr.toDouble(&ok);

        if (!ok)
        {
            emit notifyUser("[MatrixModel::setData] Error: Only double Precision Numbers allowed");
            return false;
        }
        //save value from editor to member m_gridData
        m_gridData[index.row()][index.column()] = valueInStr.toUtf8();
        // Set bit for edited data at given Index
        changedData[index.row()+currentMinRow][index.column()]= true;

        editedGridData[QPair<int,int>(index.row()+currentMinRow,index.column())] = valueInStr.toUtf8();
        for (int row= 0; row < _rows; row++)
        {
            diffData[row].fill(false);
        }
    }
    return true;
}

// Set Flags so the data is editable
Qt::ItemFlags MatrixModel::flags(const QModelIndex &index) const
{
    return Qt::ItemIsEditable | QAbstractTableModel::flags(index);
}


//Set the csv file for the model and extract and display data
void MatrixModel::setFile(QString csvFile)
{
    if (csvFile != "")
    {
        _fileName = csvFile;
        this->setTableDataFromFile();
    }
}

// Save the Edited data to Disk
void MatrixModel::saveDataCsv(bool close/*true*/)
{
    //Check if there any existing changes
    if (changedData.isEmpty())
        return;
    bool dataChanged = false;
    for (int r=0; r< _rows; r++)
        if (changedData[r].count(true))
            dataChanged = true;
    if (!dataChanged)
    {
        this->notifyUser("No changes detected!!!");
        return;
    }
    this->notifyUser("Saving your changes now\n Might take a minute!!!");

    this->layoutAboutToBeChanged();
    FILE* pFile = NULL;
    QString fileName = _fileName;
    fileName.insert(fileName.size()-4,"_cpy");
    pFile = fopen(fileName.toStdString().c_str(), "w+");
    if (pFile != NULL)
    {
        QByteArray dataToBeWritten;
        QList<QByteArray>  rowData;
        for (int eachRow = 0; eachRow <_rows; eachRow++)
        {
            rowData = this->getRow(eachRow)[0];

            //Cheack and take care of all edits
            if (!changedData.isEmpty() && changedData[eachRow].count(true)!=0)
                for (int c= 0; c < _columns; c++)
                    if (changedData[eachRow][c])
                        rowData[c] = editedGridData[QPair<int,int>(eachRow,c)];

            dataToBeWritten.append(rowData.join(','));
            if (eachRow != _rows-1)
                dataToBeWritten += '\n';
            changedData[eachRow].fill(false);
            fprintf(pFile,"%s", dataToBeWritten.constData());
            dataToBeWritten.clear();
            rowData.clear();
        }

        fflush (pFile);
        fclose(pFile);

        if( remove( _fileName.toStdString().c_str() ) == 0 )
            rename(fileName.toStdString().c_str(), _fileName.toStdString().c_str());
        if (close)
            this->clear();
        else
            //This means reading the complete files and resetting row reading....
            // Time consuming for big Fies
            this->setRowBeginings();
        this->layoutChanged();
    }
}

//Returns true if all the members of the String list are doubles
//else returns false
bool MatrixModel::isStrListOfDoubles(QList<QByteArray> inputList)
{
    bool ok = true;
    foreach(QByteArray str, inputList) {
        str.toDouble(&ok);
        if (!ok)
        {
            return ok;
        }
    }
    return ok;
}


//Load the next block of rows from file to display
//Returns true, if loading next block is possible, else false
bool MatrixModel::loadNext()
{
    int rowStart = currentMinRow+m_gridData.size();
    if (rowStart >= _rows)
        return false;
    this->loadRequiredRowData(rowStart);
    if (currentMinRow < _rows)
        return true;
    return false;
}

//Load the next block of rows from file to display
//Returns true, if loading prev block is possible, else false
bool MatrixModel::loadPrev()
{
    if (currentMinRow == 0)
        return false;
    int rowStart = currentMinRow-m_gridData.size();
    if (rowStart < 0)
        rowStart = 0;
    this->loadRequiredRowData(rowStart);
    if (currentMinRow <= 0)
        return false;
    return true;
}

//Load row-wise data into the model
void MatrixModel::loadRequiredRowData(int rowStart, int rowEnd /*-1*/)
{
    emit this->layoutAboutToBeChanged();
    currentMinRow = rowStart;
    if (rowEnd == -1)
        rowEnd = rowStart + MAX_ROWS_DISPLAYED;
    m_gridData.clear();
    for (int r= rowStart; m_gridData.size() < MAX_ROWS_DISPLAYED && r < _rows && r < rowEnd; r++)
    {
        QList<QByteArray> rowData = this->getRow(r)[0];
        m_gridData << rowData;

        if (!changedData.isEmpty() && changedData[r].count(true) > 0)
            //edited Data
            for (int c= 0; c < _columns; c++)
                if (changedData[r][c])
                    m_gridData[r-currentMinRow][c] = editedGridData[QPair<int,int>(r,c)];

    }
    emit this->layoutChanged();
}

//Private method to extract csv data and set data for the table
void MatrixModel::setTableDataFromFile()
{
    this->clear();
    emit this->layoutChanged();
    this->setRowBeginings();

    this->notifyUser("Loading Input File ...");

    currentMinRow = 0;
    _rows = rowList.size();

    this->loadRequiredRowData(currentMinRow);
    if (!m_gridData.isEmpty())
    {
        _columns = m_gridData[0].size();

        for(int i=0; i<_rows;i++)
        {
            changedData.append(QBitArray(_columns));
            diffData.append(QBitArray(_columns));
        }
        emit this->notifyUser(QString("Loaded File: %1").arg(_fileName));
    }
    emit this->layoutChanged();
}

//Set a list of file locations for the row beginings
void MatrixModel::setRowBeginings()
{
    FILE * m_fp;

    int BUF_SIZE = 1024*1000*2;
    m_fp = fopen(_fileName.toStdString().c_str(), "rb"); // open in binary mode for faster parsing
    if (m_fp != NULL)
    {
      // read the file to get the row pointers
      char buf[BUF_SIZE+1];
      qlonglong pos = 0;
      rowList << pos;
      int nr = 0;
      while ((nr = fread(buf, 1, BUF_SIZE, m_fp)))
      {
        buf[nr] = 0;
        // null-terminate the last line of data
        // find new lines in the buffer
        char *c = buf;
        while ((c = strchr(c, '\n')) != NULL)
        {
          rowList.push_back(qlonglong(pos + c-buf+1));
          c++;
        }
        pos += nr;
      }
      //Accounting for last '\n'
      fseek(m_fp, rowList.at(rowList.size()-1), SEEK_SET);
      if (fgetc(m_fp) == EOF || fgetc(m_fp) == ' ')
      {
        rowList.pop_back();
      }
      fclose(m_fp);
    }
}

//Returns the list of Bytes in the row
QList< QList<QByteArray> > MatrixModel::getRow(int row, int numRows/*1*/)
{
    if (row >= rowList.size())
        return QList< QList< QByteArray> >();

    // optimized line reading procedure
    FILE * m_fp = NULL;

    int BUF_SIZE = 1024*1000;
    m_fp = fopen(_fileName.toStdString().c_str(), "r");
    // open in binary mode for faster parsing
    QList<QList<QByteArray> > rowsRead;
    QByteArray str;
    if (m_fp != NULL)
    {
        for(int r = row; r< rowList.size() && r < row+numRows; r++)
        {
            char buf[BUF_SIZE+1];
            fseek(m_fp, rowList[row], SEEK_SET);
            int nr = 0;
            while ((nr = ::fread(buf, 1, BUF_SIZE, m_fp)))
            {
              buf[nr] = 0;
              // null-terminate the string
              // find new lines in the buffer
              char *c = ::strchr(buf, '\n');
              if (c != NULL)
              {
                *c = 0;
                str += buf;
                break;
              }
              str += buf;
            }
            rowsRead << str.split(',');
        }
        fclose(m_fp);
    }
    return rowsRead;
}

//Set/Reset enableComparison 
void MatrixModel::showComparison(bool compare)
{
    _enableComparison = compare;
}

// Compare input model to this model
bool MatrixModel::compareModels(MatrixModel &model)
{
    QList<QList<QByteArray> > m2_data = model.getData(currentMinRow,currentMinRow+m_gridData.size());
    //match the view ports--row ranges--^
    if (m_gridData.size() == m2_data.size() && m_gridData[0].size() == m2_data[0].size())
    {
        emit this->notifyUser("Differences highlighted in green");
        emit this->layoutAboutToBeChanged();
        bool diffFound= false;
        for (int row=0; row < m_gridData.size(); row++)
            for(int col=0; col < m_gridData[0].size(); col++)
                if (m_gridData[row][col] != m2_data[row][col])
                {
                    diffFound = true;
                    diffData[row+currentMinRow][col]=true;
                }
        if (!diffFound)
            this->notifyUser("No Difference Found");
        model.setComparisonData(diffData);
        emit this->layoutChanged();
        return true;
    }
    else
        emit this->notifyUser("Comparison only works between 2 matrics of same dimensions");
    return false;
}

// Set diffData array for the model
bool MatrixModel::setComparisonData(QList<QBitArray> cD)
{
    if (cD.size() == _rows and cD[0].size())
    {
        emit this->layoutAboutToBeChanged();
        diffData = cD;
        emit this->layoutChanged();
        return true;
    }
    else
        this->notifyUser("[MatrixModel::setComparisonData] Size of input array does not match the dimensions of the matrix");
    return false;
}

// Return model data
QList<QList < QByteArray > > MatrixModel::getData(int rowStart, int rowEnd)
{
    this->loadRequiredRowData(rowStart, rowEnd);
    return m_gridData;
}

//Clear all data from the model and reset
void MatrixModel::clear()
{
    this->beginResetModel();
    rowList.clear();
    m_gridData.clear();
    changedData.clear();
    diffData.clear();
    _rows = 0;
    _columns = 0;
    _enableComparison = false;
    this->endResetModel();
}
