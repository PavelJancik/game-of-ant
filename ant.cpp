#include "ant.h"

Ant::Ant(unsigned long long int p_row, unsigned long long int p_col)
{
    m_row = p_row;
    m_col = p_col;
    m_init_row = p_row;
    m_init_col = p_col;
    m_direction = 1;
}

void Ant::changeDirection(QString where){
    if (where == "left") {
        m_direction--;
        if (m_direction == 0) m_direction = 4;
    } else if (where == "right") {
        m_direction++;
        if (m_direction == 5) m_direction = 1;
    } else qDebug() << "wrong direction parameter in function Ant::changeDirection(QString where)";
}

QString Ant::getDirection(){
    if (m_direction == 1) return "up";
    if (m_direction == 2) return "right";
    if (m_direction == 3) return "down";
    if (m_direction == 4) return "left";
    return "Ant::getDirection() return fail";
}

void Ant::setPosition(unsigned long long int p_row, unsigned long long int p_col){
    m_row = p_row;
    m_col = p_col;
}

unsigned long long int Ant::getPositionIndex(unsigned long long int p_gridCols){
    return m_row * p_gridCols + m_col;
}


unsigned long long int Ant::getInitPositionIndex(unsigned long long int p_gridCols){
    return m_init_row * p_gridCols + m_init_col;
}
