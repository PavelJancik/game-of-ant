#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCore>
#include <QtGui>
#include <QListWidgetItem>
#include <QPushButton>
#include <QLayout>
#include <QDebug>
#include <QTimer>
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>

#include <vector>
#include "ant.h"

#include <QEvent>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
    unsigned long long int m_cols;
    unsigned long long int m_rows;
    unsigned long long int m_speed;
    std::vector<std::vector<bool>> m_live_tiles;
    std::vector<std::vector<bool>> m_live_tiles_at_next_step;
    std::vector<std::vector<bool>> m_init_state;
    std::vector<std::vector<QPushButton *>> m_tile_buttons;
    QGridLayout *m_layout;
    QTimer *m_timer;
    bool m_game_paused;
    unsigned long long int m_generation;
    unsigned long long int m_ant_gen;
    Ant *m_ant;
    QString m_add_pattern;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_start_clicked();
    void on_next_step_clicked();
    void on_speed_valueChanged(int value);
    void on_reset_clicked();
    void on_clear_clicked();
    void on_add_ant_clicked();
    void on_start_ant_clicked();
    void on_move_ant_clicked();
    void on_save_init_state_clicked();
    void on_save_file_clicked();
    void on_load_file_clicked();
    void on_about_button_clicked();
    //void on_show_grid_clicked();

    void on_patterns_clicked();
    void on_glider_d_r_clicked();
    void on_glider_d_l_clicked();
    void on_glider_u_l_clicked();
    void on_glider_u_r_clicked();
    void on_pulsar_pattern_clicked();
    void on_spaceship_right_clicked();
    void on_spaceship_left_clicked();
    void on_spaceship_down_clicked();
    void on_spaceship_up_clicked();

private:
    // Both
    Ui::MainWindow *ui;
    void initTiles();
    QPushButton* createTile(unsigned long long int objectID);
    void setTileColor(unsigned long long int row, unsigned long long int col);
    void deleteTimer(); // both?

    // Game of Life
    void onTileClicked();
    bool changeTileState(unsigned long long int row, unsigned long long int col);
    void debug_changeTileState(unsigned long long int row, unsigned long long int col);
    void nextStep();
    void changeTileColor(unsigned long long int row, unsigned long long int col);
    bool is_tile_alive(unsigned long long int row, unsigned long long int col);
    unsigned short int alive_neighbours(unsigned long long int row, unsigned long long int col);

    // Ant
    void addAnt(unsigned long long int row, unsigned long long int col);
    void setAntHover(unsigned long long int row, unsigned long long int col);
    void deleteAnt();
    void chooseAntTile();
    void moveAnt();
    void initAntGeneration();

    // patterns
    void addGlider(unsigned long long int row, unsigned long long int col);
    void addPulsar(unsigned long long int row, unsigned long long int col);
    void addSpaceship(unsigned long long int row, unsigned long long int col);
    void on_add_pattern_clicked();
    void choosePatternPosition();
    void setTrueTile(unsigned long long int row, unsigned long long int col);

};
#endif // MAINWINDOW_H
