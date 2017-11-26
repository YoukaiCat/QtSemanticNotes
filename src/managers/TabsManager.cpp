#include "TabsManager.h"

using std::make_unique;

TabsManager::TabsManager(QObject* parent)
    : QObject(parent)
{}

void TabsManager::setup(QTabWidget* tabWidget,
                        LinksManager* linksManager)
{
    this->tabWidget = tabWidget;
    this->linksManager = linksManager;

    current = make_unique<Tab>();
    current->setLinksManager(linksManager);

    tabWidget->insertTab(0, current->getWidget(), tr("Note"));
    tabWidget->setCurrentWidget(current->getWidget());

    connect(current.get(), &Tab::modified,
            this, &TabsManager::modified);
    connect(current.get(), &Tab::noteOpenedFromHistory,
            this, &TabsManager::noteOpenedFromHistory);
}

#include <QAction>
void TabsManager::setupNavigation(QAction* backwardAction, QAction* forwardAction)
{
    connect(current.get(), &Tab::backwardAwailable,
            backwardAction, &QAction::setEnabled);
    connect(current.get(), &Tab::forwardAwailable,
            forwardAction, &QAction::setEnabled);
    connect(backwardAction, &QAction::triggered,
            current.get(), &Tab::goBackward);
    connect(forwardAction, &QAction::triggered,
            current.get(), &Tab::goForward);
}

void TabsManager::openNote(shared_ptr<Note> note)
{
    current->openNote(note);
    tabWidget->setTabText(0, note->getTitle());
}

void TabsManager::saveNote()
{
    current->saveNote();
}

void TabsManager::closeNote()
{
    current->closeNote();
    tabWidget->setTabText(0, tr("Note"));
}

void TabsManager::removeNote(shared_ptr<Note> note)
{
    current->noteRemoved(note);
}

void TabsManager::switchMode()
{
    current->switchMode();
}

//void TabsManager::openInCurrentTab(shared_ptr<Note> note)
//{
//    auto tab = current.value_or(addNewTab(note));
//    tab->openNote(note);
//}

//void TabsManager::closeTab(shared_ptr<Note> note)
//{
//    auto noteIt = tabByNote.find(note);
//    Q_ASSERT(noteIt != tabByNote.end());
//}

//void TabsManager::onCurrentChanged(int index)
//{
//    auto indexIt = tabByIndex.find(index);
//    Q_ASSERT(indexIt != tabByIndex.end());
//    current = indexIt->second;
//}

//void TabsManager::onTabCloseRequested(int index)
//{
//    auto indexIt = tabByIndex.find(index);
//    Q_ASSERT(indexIt != tabByIndex.end());
//    auto note = indexIt->second->getNote();
//    auto noteIt = tabByNote.find(note);
//    tabByIndex.erase(indexIt);
//    Q_ASSERT(noteIt != tabByNote.end());
//    tabByNote.erase(noteIt);
//}

//Tab* TabsManager::findOrCreate(shared_ptr<Note> note)
//{
//    auto search = tabByNote.find(note);
//    if (search != tabByNote.end()) {
//        return search->second;
//    } else {
//        return addNewTab(note);
//    }
//}

//Tab* TabsManager::addNewTab(shared_ptr<Note> note)
//{
//    Tab* tab = new Tab(note);
//    tabByNote.emplace(note, tab);
//    int index = widget->addTab(tab->getWidget(), tab->getLabel());
//    tabByIndex.emplace(index, tab);
//    return tab;
//}
