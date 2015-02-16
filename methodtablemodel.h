#ifndef METHODTABLEMODEL_H
#define METHODTABLEMODEL_H

#include <vector>
#include <stdlib.h>
#include <QVariant>
#include <QAbstractTableModel>
#include <QMessageBox>
#include <QTime>
#include <QDebug>
#include "ui_methodwindow.h"

#include "globals.h"

struct Table_Header
{
    int section;
    QVariant value;
};

struct Table_Data
{
    int row, column;
    QTime data;
};

class MethodTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:

    bool edited;
    QString m_FileName;

    MethodTableModel(QObject *parent, int p_rows = 1, int p_columns = 1);
    ~MethodTableModel();
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);
    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role = Qt::EditRole);

    Qt::ItemFlags flags(const QModelIndex & index) const;

    QString getDataAsCSV();
    void setDataFromCSV(QString in);

    void reinitialize();
    void refresh();

    int getActionSeconds(int row, int column);
    int getStepSeconds(int row);
    int getRunSeconds();

public slots:

    void updateSelection(QItemSelection, QItemSelection);
    void updateWasteTime(const QTime& time);
    void updateSampleTime(const QTime& time);
private:
    int m_rows, m_columns; // Holds the number of rows and columns the table has
    std::vector<std::vector<Table_Data> > m_gridData; // Hold inputted data
    std::vector<Table_Header> horizontalHeaders; // Hold headers

    std::vector<int> selectedRows; // Hold which rows are highlighted for editing by the user

    void addSelectedRow(int row);
    void removeSelectedRow(int row);

    void resize_data();
    void resize_headers();
};

#endif // METHODTABLEMODEL_H
