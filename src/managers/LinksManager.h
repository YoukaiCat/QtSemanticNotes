#ifndef LINKSMANAGER_H
#define LINKSMANAGER_H

#include "../entities/Note.h"

#include <QSqlRelationalTableModel>
#include <QTableView>

#include <memory>
using std::unique_ptr;
using std::shared_ptr;

class LinksManager : public QObject
{
    Q_OBJECT
public:
    explicit LinksManager(QObject* parent = nullptr);
    QString makeLinks(QString rightPart);
    void loadPossibleLinks();

signals:
    void possibleLinksLoaded();
    void linksUpdated();
    void backLinksUpdated();
//    void linkSelected(Id noteId);

public slots:
    void setup(QTableView* linkedFromView,
               QTableView* hasLinksToView);

    void clearModel();
    void filterModel(shared_ptr<Note> note);
    void selectFromModel();

    void updateLinks(shared_ptr<Note> note);
    void updateBackLinks(shared_ptr<Note> note);
    void updateBackLinksByTitleAndId(const QString& title, Id noteId);

//private slots:
//    void onLinkedFromSelectionChange();
//    void onLinksToSelectionChange();

private:
    unique_ptr<QSqlRelationalTableModel> createLinkedFromModel();
    void setupViewLinkedFrom(QTableView* view, QSqlRelationalTableModel* model);
    unique_ptr<QSqlRelationalTableModel> createHasLinksToModel();
    void setupViewHasLinksTo(QTableView* view, QSqlRelationalTableModel* model);

    unique_ptr<QSqlRelationalTableModel> linkedFromModel;
    unique_ptr<QSqlRelationalTableModel> hasLinksToModel;
    QTableView* linkedFromView;
    QTableView* hasLinksToView;

    QList<Id> findLinks(const QString& text);

    QPair<QHash<QString,Id>,QString> possibleLinks;
};

#endif // LINKSMANAGER_H
