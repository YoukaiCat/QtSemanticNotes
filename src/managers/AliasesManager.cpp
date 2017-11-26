#include "AliasesManager.h"

#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlRelation>

using std::make_unique;

AliasesManager::AliasesManager(QObject* parent)
    : QObject(parent)
{}

void AliasesManager::setup(QTableView* view)
{
    this->view = view;
    model = createModel();
    setupView(view, model.get());

    connect(this, &AliasesManager::aliasAdded,
            model.get(), &QSqlTableModel::select);
    connect(this, &AliasesManager::aliasRemoved,
            model.get(), &QSqlTableModel::select);
    connect(view->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &AliasesManager::onSelectionChange);
}

void AliasesManager::clearModel()
{
    model->clear();
}

void AliasesManager::filterModel(shared_ptr<Note> note)
{
    QString idFilter = QString("note_id = %1").arg(note->getId());
    model->setFilter(idFilter);
    selectFromModel();
}

void AliasesManager::selectFromModel()
{
    model->select();
}

void AliasesManager::addAlias(shared_ptr<Note> note, const QString& alias)
{
    Note::addNoteAlias(note.get(), alias);
    emit aliasAdded(alias, note->getId());
}

QString AliasesManager::aliasByIndex(const QModelIndex& index)
{
    return model->record(index.row()).value(1).toString();
}

Id AliasesManager::noteIdByIndex(const QModelIndex& index)
{
    return model->record(index.row()).value(2).toUInt();
}

void AliasesManager::removeAlias(const QModelIndex& index)
{
    model->removeRows(index.row(), 1);
    emit aliasRemoved(aliasByIndex(index), noteIdByIndex(index));
}

void AliasesManager::removeSelectedAliases()
{
    QModelIndexList indexes = view->selectionModel()->selectedIndexes();
    for(QModelIndex& index : indexes) {
        removeAlias(index);
    }
}

void AliasesManager::onSelectionChange()
{
    if(view->selectionModel()->selectedIndexes().size() > 0) {
        emit aliasSelected();
    } else {
        emit aliasNotSelected();
    }
}

unique_ptr<QSqlTableModel> AliasesManager::createModel()
{
    auto model = make_unique<QSqlTableModel>();
    model->setTable("aliases");
    model->setEditStrategy(QSqlTableModel::OnFieldChange);
    return model;
}

void AliasesManager::setupView(QTableView* view, QSqlTableModel* model)
{
    view->setModel(model);
    view->hideColumn(0);
    view->hideColumn(2);
    view->hideColumn(3);
}
