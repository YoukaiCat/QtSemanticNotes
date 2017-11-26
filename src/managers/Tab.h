#ifndef TAB_H
#define TAB_H

#include "History.h"
#include "LinksManager.h"

#include <QTabWidget>
#include <QTextBrowser>

class Tab : public History
{
    Q_OBJECT
public:
    explicit Tab(QObject* parent = nullptr);
    explicit Tab(shared_ptr<Note> note, QObject* parent = nullptr);
    ~Tab();

    QString getLabel();
    QWidget* getWidget() const;

    shared_ptr<Note> getNote() const;

    LinksManager* getLinksManager() const;
    void setLinksManager(LinksManager* linksManager);

    QTextBrowser* getBrowser() const;

signals:
    void modified();

public slots:
    void openNote(const shared_ptr<Note>& note);
    void saveNote();
    void closeNote();
    void noteRemoved(shared_ptr<Note> note);

    void switchMode();

private:
    void enableViewMode();
    void enableEditMode();
    void chooseModeToEnable();
    void setupBrowser();
    void saveLinks();

    shared_ptr<Note> note;
    QTextDocument editDoc;
    QTextDocument viewDoc;
    unique_ptr<QTextBrowser> browser;
    LinksManager* manager;
    void maybeSaveNote();
};

#endif // TAB_H
