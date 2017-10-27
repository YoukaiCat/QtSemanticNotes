#ifndef NOTE_H
#define NOTE_H

#include <QString>
#include <QDateTime>

class SQLNote
{
public:
    SQLNote();

    static QList<SQLNote> getAll();
    static SQLNote getById(int id);

    int getId() const;
    void setId(int id);

    QString getTitle() const;
    void setTitle(const QString& title);

    QString getContent() const;
    void setContent(const QString& content);

    QDateTime getCreatedAt() const;
    void setCreatedAt(const QDateTime& datetime);

    QDateTime getUpdatedAt() const;
    void setUpdatedAt(const QDateTime& datetime);

    void create();
    void update();

    QString toString();

private:
    int id;
    QString title;
    QString content;
    QDateTime createdAt;
    QDateTime updatedAt;
};

#endif // NOTE_H
