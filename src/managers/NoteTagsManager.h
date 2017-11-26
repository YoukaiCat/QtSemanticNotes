#ifndef NOTETAGSMANAGER_H
#define NOTETAGSMANAGER_H

#include "../entities/Note.h"
#include "../entities/Tag.h"

#include <QSqlRelationalTableModel>
#include <QTableView>

#include <memory>
using std::unique_ptr;
using std::shared_ptr;

class NoteTagsManager : public QObject
{
    Q_OBJECT
public:
    explicit NoteTagsManager(QObject* parent = nullptr);

signals:
    void noteTagAdded();
    void noteTagRemoved();
    void noteTagSelected();
    void noteTagNotSelected();

public slots:
    void setup(QTableView* view);

    void clearModel();
    void filterModel(shared_ptr<Note> note);
    void selectFromModel();

    void addNoteTag(shared_ptr<Note> note, const QString& name);
    void removeNoteTag(const QModelIndex& index);
    void removeSelectedNoteTags();

private slots:
    void onSelectionChange();

private:
    unique_ptr<QSqlRelationalTableModel> createModel();
    void setupView(QTableView* view, QSqlRelationalTableModel* model);
    void removeTagByIndex(const QModelIndex& index);

    unique_ptr<QSqlRelationalTableModel> model;
    QTableView* view;
};

#endif // NOTETAGSMANAGER_H
