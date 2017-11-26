#ifndef ALIASESMANAGER_H
#define ALIASESMANAGER_H

#include "../entities/Note.h"

#include <QSqlTableModel>
#include <QTableView>

#include <memory>
using std::unique_ptr;
using std::shared_ptr;

class AliasesManager : public QObject
{
    Q_OBJECT
public:
    explicit AliasesManager(QObject* parent = nullptr);

signals:
    void aliasAdded(const QString& alias, Id noteId);
    void aliasRemoved(const QString& alias, Id noteId);
    void aliasSelected();
    void aliasNotSelected();

public slots:
    void setup(QTableView* view);

    void clearModel();
    void filterModel(shared_ptr<Note> note);
    void selectFromModel();

    void addAlias(shared_ptr<Note> note, const QString& alias);
    void removeAlias(const QModelIndex& index);
    void removeSelectedAliases();

private slots:
    void onSelectionChange();

private:
    unique_ptr<QSqlTableModel> createModel();
    void setupView(QTableView* view, QSqlTableModel* model);
    QString aliasByIndex(const QModelIndex& index);
    Id noteIdByIndex(const QModelIndex& index);

    unique_ptr<QSqlTableModel> model;
    QTableView* view;
};

#endif // ALIASESMANAGER_H
