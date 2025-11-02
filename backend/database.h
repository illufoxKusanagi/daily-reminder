#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include <QString>

class Database
{
public:
    static Database &instance();
    bool initialize();
    QSqlDatabase &db() { return m_db; }

private:
    Database();
    ~Database();
    Database(const Database &) = delete;
    Database &operator=(const Database &) = delete;

    bool createTables();

    QSqlDatabase m_db;
};

#endif