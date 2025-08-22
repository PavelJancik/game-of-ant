#ifndef ANT_H
#define ANT_H

#include <QString>
#include <QDebug>

class Ant
{
    unsigned long long int m_row;
    unsigned long long int m_col;
    unsigned long long int m_init_row;
    unsigned long long int m_init_col;
    // ciselny m_direction typ zajisti jednoduche otaceni o 90 stupnu (pomoci ++ nebo --)
    unsigned short int m_direction; // 1 - up; 2 - right; 3 - down; 4 - left;

public:
    Ant(unsigned long long int p_row, unsigned long long int p_col);
    void changeDirection(QString where);
    QString getDirection();
    void setPosition(unsigned long long int p_row, unsigned long long int p_col);
    unsigned long long int getPositionIndex(unsigned long long int p_gridCols);
    unsigned long long int getInitPositionIndex(unsigned long long int p_gridCols);
};

#endif // ANT_H
