#ifndef NOTE_H
#define NOTE_H

#include <QString>
#include <QDateTime>

class SQLNote
{
public:
    SQLNote();

    static QList<SQLNote> getAll();
    static SQLNote getById(quint64 id);

    quint64 getId() const;
    void setId(quint64 id);

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
    quint64 id;
    QString title;
    QString content;
    QDateTime createdAt;
    QDateTime updatedAt;
};

#endif // NOTE_H
