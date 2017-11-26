#ifndef TABSMANAGER_H
#define TABSMANAGER_H

#include "Tab.h"

#include <QTabWidget>

#include <unordered_map>
#include <memory>
using std::unique_ptr;
using std::unordered_map;

class TabsManager : public QObject
{
    Q_OBJECT
public:
    explicit TabsManager(QObject* parent = nullptr);
    void setup(QTabWidget* tabWidget,
               LinksManager* linksManager);
    void setupNavigation(QAction* backwardAction, QAction* forwardAction);

signals:
    void modified();
    void noteOpenedFromHistory(shared_ptr<Note> note);

public slots:
    void openNote(shared_ptr<Note> note);
    void saveNote();
    void closeNote();
    void removeNote(shared_ptr<Note> note);

    void switchMode();

    //void openInCurrentTab(shared_ptr<Note> note);
    //void closeTab(shared_ptr<Note> note);

    //void onCurrentChanged(int index);
    //void onTabCloseRequested(int index);

private:
    //Tab* findOrCreate(shared_ptr<Note> note);
    //Tab* addNewTab(shared_ptr<Note> note);

    unique_ptr<Tab> current;
//    optional<Tab*> current;
//    unordered_map<shared_ptr<Note>, Tab*> tabByNote;
//    unordered_map<int, Tab*> tabByIndex;
    QTabWidget* tabWidget;
    LinksManager* linksManager;
};

#endif // TABSMANAGER_H
