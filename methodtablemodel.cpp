#include "methodtablemodel.h"

// Const. / Dest.

MethodTableModel::MethodTableModel(QObject *parent, int p_rows, int p_columns)
    : QAbstractTableModel(parent),
      edited(false),
      m_FileName(""),
      m_rows(p_rows),
      m_columns(p_columns),
      pasteRow(0)
{
    resize_data();
    resize_headers();
    reinitialize();
}
MethodTableModel::~MethodTableModel() {}

// Public Funcs

int MethodTableModel::rowCount(const QModelIndex &parent) const
{
    return m_rows;
}

int MethodTableModel::columnCount(const QModelIndex &parent) const
{
    return m_columns;
}

QVariant MethodTableModel::data(const QModelIndex &index, int role) const
{
    switch(role)
    {
        case Qt::DisplayRole:
        {
            for(std::vector< std::vector<Table_Data> >::const_iterator it = m_gridData.begin(); it != m_gridData.end(); ++it)
                for(std::vector<Table_Data>::const_iterator iter = it->begin(); iter != it->end(); ++iter)
                    if(index.row() == iter->row && index.column() == iter->column)
                        return ((iter->data == QTime(0,0,0)) ?
                                    QVariant() :
                                    (static_cast<QVariant>(iter->data.toString("hh:mm"))));
        }
            break;
        default:
            return QVariant();
            break;
    }
}

QVariant MethodTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch(role)
    {
        case Qt::DisplayRole:
        {
            if(orientation == Qt::Horizontal)
                for(std::vector<Table_Header>::const_iterator it = horizontalHeaders.begin(); it != horizontalHeaders.end(); ++it)
                    if(it->section == section)
                        return it->value.toString();
            if(orientation == Qt::Vertical)
                return QString("Sample %1").arg(section+1);
        }
            break;
        default:
            return QVariant();
            break;
    }
}

bool MethodTableModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
    if (role == Qt::EditRole)
    {
        // Confirm that the document has (probably) been edited.
        edited = true;
        // Save value from editor to member m_gridData
        for(std::vector< std::vector<Table_Data> >::iterator it = m_gridData.begin(); it != m_gridData.end(); ++it)
            for(std::vector<Table_Data>::iterator iter = it->begin(); iter != it->end(); ++iter)
                if(index.row() == iter->row && index.column() == iter->column)
                {
                    // Update data in vector
                    if(value.toTime().isValid() && !value.toTime().isNull())
                        iter->data = value.toTime();
                    else
                        iter->data = QTime(0,0,0);
                    // Update total time
                    float hours = 0, minutes = 0, seconds = 0;
                    for(iter = it->begin(); iter != it->end(); ++iter)
                        if(iter->column != 2)
                        {
                            hours += iter->data.hour();
                            minutes += iter->data.minute();
                            seconds += iter->data.second();
                        }
                    for(iter = it->begin(); iter != it->end(); ++iter)
                        if(iter->column == 2)
                            iter->data = QTime(hours, minutes, seconds);
                    return true;
                }
                return false;   //added this to create a definite return???
    }
}

bool MethodTableModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    if (role == Qt::EditRole)
    {
        //Check if data for given section header already exists
        if(orientation == Qt::Horizontal)
            for(std::vector<Table_Header>::iterator it = horizontalHeaders.begin(); it != horizontalHeaders.end(); ++it)
                if(it->section == section)
                    it->value = value;
    }
    return true;
}

Qt::ItemFlags MethodTableModel::flags(const QModelIndex & index) const
{
    return Qt::ItemIsSelectable |  Qt::ItemIsEditable | Qt::ItemIsEnabled ;
}

QString MethodTableModel::getDataAsCSV()
{
    QString temp;
    int rows = 0, columns = 0;
    for(std::vector<std::vector<Table_Data> >::iterator it = m_gridData.begin(); it != m_gridData.end(); ++it, ++rows)
    {
        for(std::vector<Table_Data>::iterator iter = it->begin(); iter != it->end(); ++iter, ++columns)
        {
            temp += QString("%1:%2:%3,").arg(iter->data.hour()).arg(iter->data.minute()).arg(iter->data.second());
        }
        temp += '\n';
        columns = 0;
    }
    return temp;
}

void MethodTableModel::setDataFromCSV(QString in)
{
    m_gridData.resize(m_rows);
    int rows = 0, columns = 0;
    QStringList rowData = in.split('\n');
    QStringList columnData, cellData;
    if(rowData.length() < m_rows)
    {
        QMessageBox msgBox;
        msgBox.setText(QString("Invalid File Format."));
        msgBox.exec();
        return;
    }
    for(std::vector<std::vector<Table_Data> >::iterator it = m_gridData.begin(); it != m_gridData.end(); ++it, ++rows)
    {
        columnData = rowData.value(rows).split(',');
        it->resize(m_columns);
        if(columnData.length() < m_columns)
        {
            QMessageBox msgBox;
            msgBox.setText(QString("Invalid File Format."));
            msgBox.exec();
            return;
        }
        for(std::vector<Table_Data>::iterator iter = it->begin(); iter != it->end(); ++iter, ++columns)
        {
            cellData = columnData.value(columns).split(':');
            iter->data.setHMS(atoi(cellData.value(0).toStdString().c_str()),
                              atoi(cellData.value(1).toStdString().c_str()),
                              atoi(cellData.value(2).toStdString().c_str()));
        }
        columns = 0;
    }
    refresh();
}

void MethodTableModel::refresh()
{
    dataChanged(index(0, 0), index(SAMPLES, STEPS));
}

void MethodTableModel::reinitialize()
{
    for (int row = 0; row < m_rows; ++row)
        for (int column = 0; column < m_columns; ++column)
            setData(index(row, column), QVariant(0));
}

int MethodTableModel::getActionSeconds(int row, int column)
{
    QTime temp = qvariant_cast<QTime>(data(index(row,column)));
    int time = 0;
    time += temp.hour()*360;
    time += temp.minute()*60;
    time += temp.second();
    return time;
}
int MethodTableModel::getStepSeconds(int row)
{
    QTime temp = qvariant_cast<QTime>(data(index(row,STEPS)));
    int time = 0;
    time += temp.hour()*360;
    time += temp.minute()*60;
    time += temp.second();
    return time;
}
int MethodTableModel::getRunSeconds()
{
    int temp = 0;
    for(int i = 0; i < SAMPLES; ++i)
    {
        temp += getStepSeconds(i);
    }
    return temp;
}

// Private Funcs

void MethodTableModel::resize_data()
{
    m_gridData.resize(m_rows);
    int rows = 0, columns = 0;
    for(std::vector<std::vector<Table_Data> >::iterator it = m_gridData.begin(); it != m_gridData.end(); ++it, ++rows)
    {
        it->resize(m_columns);
        for(std::vector<Table_Data>::iterator iter = it->begin(); iter != it->end(); ++iter, ++columns)
        {
            iter->row       = rows;
            iter->column    = columns;
        }
        columns = 0;
    }
}

void MethodTableModel::resize_headers()
{
    horizontalHeaders.resize(m_columns);
    int columns = 0;

    for(std::vector<Table_Header>::iterator cols = horizontalHeaders.begin(); cols != horizontalHeaders.end(); ++cols, ++columns)
        cols->section = columns;
}

void MethodTableModel::addSelectedRow(int row)
{
    for(std::vector<int>::iterator it = selectedRows.begin(); it != selectedRows.end(); it++)
        if(row == *it)
            return;
    selectedRows.push_back(row);
    if(selectedRows.size() == 1)
    {
        setBaseWasteTime(index(row, 0).data() == QVariant() ? QTime(0,0,0) : index(row, 0).data().toTime());
        setBaseSampleTime(index(row, 1).data() == QVariant() ? QTime(0,0,0) : index(row, 1).data().toTime());
    }

}

void MethodTableModel::removeSelectedRow(int row)
{
    selectedRows.erase(std::remove(selectedRows.begin(), selectedRows.end(), row), selectedRows.end());
}

// *** SLOTS ***

void MethodTableModel::updateSelection(QItemSelection selected, QItemSelection deselected)
{
    QModelIndexList indexes = selected.indexes();
    QModelIndexList deindexes = deselected.indexes();
    for (int i = 0; i < deindexes.count(); ++i)
        removeSelectedRow(deindexes.at(i).row());
    for (int i = 0; i < indexes.count(); ++i)
        addSelectedRow(indexes.at(i).row());
}

void MethodTableModel::updateWasteTime(const QTime& time)
{
    for(std::vector<int>::iterator it = selectedRows.begin(); it != selectedRows.end(); it++)
        setData(index(*it, 0), time);
    refresh();
}
void MethodTableModel::updateSampleTime(const QTime& time)
{
    for(std::vector<int>::iterator it = selectedRows.begin(); it != selectedRows.end(); it++)
        setData(index(*it, 1), time);
    refresh();
}

void MethodTableModel::setPasteRow(int row)
{
    pasteRow = row;
}

void MethodTableModel::paste()
{
    int i = 0;
    for(std::vector<int>::iterator it = selectedRows.begin(); it != selectedRows.end(); it++)
    {
        setData(index(pasteRow+i, 0), index(*it, 0).data());
        setData(index(pasteRow+i, 1), index(*it, 1).data());
        i++;
    }
    refresh();
}
