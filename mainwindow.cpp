#include "mainwindow.h"
#include "ui_mainwindow.h"

const unsigned short int TILE_SIZE = 20;
const int BASE_SPEED = 500;
const QString DEAD_COLOR = "00000000";
const QString ALIVE_COLOR = "dddddd";
const QString HOVER_COLOR = "aaaaaa";
const QString ANT_COLOR = "bb1122";
const QString HOVER_ANT_COLOR = "991122";
const QString GLIDER_HOVER_COLOR = "abcdef";
const int PATTERNS_Y_UNDER_SCREEN = 800;
const int PATTERNS_Y_ON_SCREEN = 200;
const int ABOUT_X_OUT_OF_SCREEN = 1300;
const int ABOUT_X_ON_SCREEN = 940;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    this->setFixedSize(QSize(1280, 720));
    ui->setupUi(this);

    m_cols = 64;
    m_rows = 32;
    m_speed = BASE_SPEED / ui->speed->value();
    m_game_paused = true;
    m_generation = 0;
    m_ant_gen = 0;
    m_timer = nullptr;
    m_ant = nullptr;
    m_add_pattern = "";

    ui->speed_label->setText(QString::number(ui->speed->value())+"x");
    ui->patterns_group->setGeometry(ui->patterns_group->geometry().x(), PATTERNS_Y_UNDER_SCREEN,  ui->patterns_group->geometry().width(),  ui->patterns_group->geometry().height());
    ui->about_article->setGeometry(ABOUT_X_OUT_OF_SCREEN, ui->about_article->geometry().y(), ui->about_article->geometry().width(), ui->about_article->geometry().height());
    ui->generation->setFontPointSize(9.0);
    ui->ant_gen->setFontPointSize(9.0);

    initTiles();

    // vytvoreni OnClick funkci pro dynamicke widgety, ktere jsem vytvoril v initTiles();   
    for (unsigned long long int row=0; row<m_rows; row++){
        for (unsigned long long int col=0; col<m_cols; col++){
            connect(m_tile_buttons[row][col], &QPushButton::clicked, this, &MainWindow::onTileClicked);
        }
    }

//    QString style = "background-color: rgba(255, 255, 255,0);"
//                    "color: red;"
//                    "font: bold;"
//                    "border: none;"
//                    "font-size: 8px;";
//    ui->life_gen_label->setStyleSheet(style);

}

MainWindow::~MainWindow()
{
    delete ui;
}

//-------- init state -----------//

void MainWindow::onTileClicked(){
    if (m_generation == 0) {
        QObject *senderObj = sender();
        unsigned long long index = senderObj->objectName().toULongLong();
        unsigned long long int row, col;
        row = index / m_cols;
        col = index % m_cols;
        changeTileColor(row, col);
        m_init_state[row][col]=!m_init_state[row][col];
        m_live_tiles[row][col]=!m_live_tiles[row][col];
    } else {
        QMessageBox::information(this, "Oops!", "You have to save or reset this state first");
    }
}

void MainWindow::initTiles(){
    // Struktura: pages > page > layout > item > tile(button)
    QWidget *page = new QWidget();
    page->setObjectName("game_page");
    QGridLayout *layout = new QGridLayout();
    //layout->setSizeConstraint(QLayout::SetDefaultConstraint);
    //layout->setSpacing(0);
    //layout->setMargin(0);

    unsigned long long int objectID=0;
    for (unsigned long long int row=1; row<=m_rows; row++){
        std::vector<bool> rowVector;
        std::vector<QPushButton *> rowButtons;
        for (unsigned long long int col=1; col<=m_cols; col++){
            rowVector.push_back(false);
            QPushButton *tile = createTile(objectID++);
            QLayoutItem *item = new QWidgetItem(tile);
            layout->addItem(item, row, col, 1, 1);
            rowButtons.push_back(tile);
        }
        m_live_tiles.push_back(rowVector);
        m_live_tiles_at_next_step.push_back(rowVector);
        m_init_state.push_back(rowVector);
        m_tile_buttons.push_back(rowButtons);
    }
    page->setLayout(layout);      
    m_layout = layout;
    ui->pages->addWidget(page);
    ui->pages->setCurrentIndex(2);
}

QPushButton* MainWindow::createTile(unsigned long long int objectID){
        QPushButton *tile = new QPushButton();
        tile->setObjectName(QString::number(objectID));
        tile->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        tile->setMaximumSize(TILE_SIZE,TILE_SIZE);
        tile->setMinimumSize(TILE_SIZE,TILE_SIZE);

        tile->setCursor(Qt::PointingHandCursor);        
        tile->setStyleSheet("QPushButton#" + tile->objectName() +
                            "{background: #" + DEAD_COLOR + "}"
                            "QPushButton#" + tile->objectName() + ":hover "
                            "{background: #" + HOVER_COLOR + "}");
        // tile->setText(QString::number(objectID));
        return tile;
}

//void MainWindow::on_show_grid_clicked()
//{
//    for (unsigned long long int row=0; row<m_rows; row++){
//        for (unsigned long long int col=0; col<m_cols; col++){
//            unsigned long long int i = row * m_cols + col;
//            if (m_live_tiles[row][col]) m_layout->itemAt(i)->widget()->setStyleSheet("QPushButton#" + m_layout->itemAt(i)->widget()->objectName() +
//                                                                                     "{background: #" + ALIVE_COLOR + "; border: 0.5px solid grey}"
//                                                                                     "QPushButton#" +  m_layout->itemAt(i)->widget()->objectName() + ":hover "
//                                                                                     "{background: #" + HOVER_COLOR + "}");
//            else m_layout->itemAt(i)->widget()->setStyleSheet("QPushButton#" + m_layout->itemAt(i)->widget()->objectName() +
//                                                              "{background: #" + DEAD_COLOR + "; border: 0.5px solid grey}"
//                                                              "QPushButton#" +  m_layout->itemAt(i)->widget()->objectName() + ":hover "
//                                                              "{background: #" + HOVER_COLOR + "}");

//        }
//    }
//}

//-------- Game of life -----------//

void MainWindow::nextStep(){
    m_generation++;
    ui->generation->setText(QString::number(m_generation));
    unsigned long long int i=0;
    for (unsigned long long int row=0; row<m_rows; row++){
        for (unsigned long long int col=0; col<m_cols; col++){
            if (changeTileState(row, col)) { // pokud ma nastat zmena policka
                // debug_changeTileState(row, col); // pro debug
                m_live_tiles_at_next_step[row][col]=!m_live_tiles[row][col]; // zmena zivota policka
                changeTileColor(row, col);
            }
            i++;
        }
    }    
    m_live_tiles = m_live_tiles_at_next_step;
}

bool MainWindow::changeTileState(unsigned long long int row, unsigned long long int col){
    unsigned short int neighbours_alive_sum = alive_neighbours(row, col); // pocet zivych sousedu
    // zmena nastane pokud
    if (m_live_tiles[row][col]){ // je nazivu
        if ((neighbours_alive_sum < 2) or (neighbours_alive_sum > 3)) return true; // a ma MENE nez 2 nebo VICE nez 3
    } else { // pokud je mrtve
        if (neighbours_alive_sum == 3) return true; //  a ma PRAVE 3 zive sousedy
    }
    return false; // jinak beze zmeny
}


unsigned short int MainWindow::alive_neighbours(unsigned long long int row, unsigned long long int col){

    unsigned short int alive = 0;

    if (is_tile_alive(row-1, col-1)) alive++;
    if (is_tile_alive(row-1, col)) alive++;
    if (is_tile_alive(row-1, col+1)) alive++;

    if (is_tile_alive(row, col-1)) alive++;
    if (is_tile_alive(row, col+1)) alive++;

    if (is_tile_alive(row+1, col-1)) alive++;
    if (is_tile_alive(row+1, col)) alive++;
    if (is_tile_alive(row+1, col+1)) alive++;

    return alive;
}

void MainWindow::debug_changeTileState(unsigned long long int row, unsigned long long int col){
    // predstav si numerickou klavesnici, 5 je prvek se sourednicemi [row][col]
    // ostatni cisla predstavuji sousedni prvky
    qDebug() << "Prvek: [" << row << "][" << col << "]";
    qDebug() << "index: " << row*m_cols+col;
    if (is_tile_alive(row-1, col-1))  qDebug() << "7";
    if (is_tile_alive(row-1, col))  qDebug() << "8";
    if (is_tile_alive(row-1, col+1))  qDebug() << "9";
    if (is_tile_alive(row, col-1))  qDebug() << "4";
    if (is_tile_alive(row, col+1))  qDebug() << "6";
    if (is_tile_alive(row+1, col-1))  qDebug() << "1";
    if (is_tile_alive(row+1, col))  qDebug() << "2";
    if (is_tile_alive(row+1, col+1))  qDebug() << "3";
}

bool MainWindow::is_tile_alive(unsigned long long int row, unsigned long long int col){
    // uprava souradnic v pripade hranicnich bodu
    if (row >= m_rows){
        if (row == m_rows) row=0; // prekroceni spodni hrany
        else row = m_rows-1; // prekroceni horni hrany a preteceni
    }
    if (col >= m_cols){
        if (col == m_cols) col=0; // prekroceni prave hrany
        else col = m_cols-1; // prekroceni leve hrany a preteceni
    }
    // zjisteni zda je policko na zivu
    if (m_live_tiles[row][col]) return true;
    else return false;
}

void MainWindow::on_start_clicked()
{

//    if (m_live_tiles != m_init_state){
//        QMessageBox::StandardButton reply = QMessageBox::question(this, "This is not an init state", "To start Game of Life ?", QMessageBox::Yes|QMessageBox::No);
//        if (reply == QMessageBox::Yes) {

//        };
//    }


    if (m_generation == 0 and m_ant_gen == 0) {
        m_live_tiles = m_init_state;
        m_live_tiles_at_next_step = m_init_state;
    }
    if (m_game_paused){
        m_timer = new QTimer(this);
        connect(m_timer, SIGNAL(timeout()), this, SLOT(on_next_step_clicked()));
        m_timer->start(m_speed);
        m_game_paused = false;
        ui->start->setText("Stop Life");
        ui->reset->setEnabled(false);
        ui->clear->setEnabled(false);
        ui->save_init_state->setEnabled(false);
        ui->initAntGeneration->setEnabled(false);
        ui->start_ant->setEnabled(false);
        ui->move_ant->setEnabled(false);
        ui->add_ant->setEnabled(false);
        ui->save_file->setEnabled(false);
        ui->load_file->setEnabled(false);
    } else {
        m_timer->stop();
        deleteTimer();
        m_game_paused = true;
        ui->start->setText("Start Life");
        ui->reset->setEnabled(true);
        ui->clear->setEnabled(true);
        ui->save_init_state->setEnabled(true);
        ui->save_file->setEnabled(true);
        ui->load_file->setEnabled(true);
    }
}

void MainWindow::on_next_step_clicked()
{
    nextStep();
    deleteAnt();
}

void MainWindow::on_speed_valueChanged(int value)
{
    if (m_timer != nullptr) m_timer->stop();
    m_speed = BASE_SPEED / value;
    if (!m_game_paused) m_timer->start(m_speed);
    ui->speed_label->setText(QString::number(value) + "x");
}

void MainWindow::on_reset_clicked()
{
    m_generation = 0;
    deleteTimer();
    deleteAnt();
    m_game_paused = true;
    ui->start->setText("Start Life");
    ui->generation->setText(QString::number(m_generation));

    m_live_tiles = m_init_state;
    m_live_tiles_at_next_step = m_init_state;
    unsigned long long int i=0;
    for (unsigned long long int row=0; row<m_rows; row++){
        for (unsigned long long int col=0; col<m_cols; col++){
            setTileColor(row, col);
            i++;
        }
    }

    ui->initAntGeneration->setEnabled(true);
    ui->add_ant->setEnabled(true);
    ui->save_file->setEnabled(true);
}

void MainWindow::changeTileColor(unsigned long long int row, unsigned long long int col){
    unsigned long long int i = row * m_cols + col;
    if (m_live_tiles[row][col]) m_layout->itemAt(i)->widget()->setStyleSheet("QPushButton#" + m_layout->itemAt(i)->widget()->objectName() +
                                                                             "{background: #" + DEAD_COLOR + "}"
                                                                             "QPushButton#" +  m_layout->itemAt(i)->widget()->objectName() + ":hover "
                                                                             "{background: #" + HOVER_COLOR + "}");
    else m_layout->itemAt(i)->widget()->setStyleSheet("QPushButton#" + m_layout->itemAt(i)->widget()->objectName() +
                                                      "{background: #" + ALIVE_COLOR + "}"
                                                      "QPushButton#" +  m_layout->itemAt(i)->widget()->objectName() + ":hover "
                                                      "{background: #" + HOVER_COLOR + "}");
}

void MainWindow::setTileColor(unsigned long long int row, unsigned long long int col){
    unsigned long long int i = row * m_cols + col;
    if (m_live_tiles[row][col]) m_layout->itemAt(i)->widget()->setStyleSheet("QPushButton#" + m_layout->itemAt(i)->widget()->objectName() +
                                                                             "{background: #" + ALIVE_COLOR + "}"
                                                                             "QPushButton#" +  m_layout->itemAt(i)->widget()->objectName() + ":hover "
                                                                             "{background: #" + HOVER_COLOR + "}");
    else m_layout->itemAt(i)->widget()->setStyleSheet("QPushButton#" + m_layout->itemAt(i)->widget()->objectName() +
                                                      "{background: #" + DEAD_COLOR + "}"
                                                      "QPushButton#" +  m_layout->itemAt(i)->widget()->objectName() + ":hover "
                                                      "{background: #" + HOVER_COLOR + "}");
}

void MainWindow::on_clear_clicked()
{
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Are you sure?", "This will delete your current init state, are you sure you want to continue?", QMessageBox::Yes|QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        m_generation = 0;
        m_ant_gen = 0;
        deleteTimer();
        deleteAnt();
        m_game_paused = true;
        ui->start->setText("Start");
        ui->generation->setText(QString::number(m_generation));
        ui->ant_gen->setText(QString::number(m_ant_gen));

        unsigned long long int i=0;
        for (unsigned long long int row=0; row<m_rows; row++){
            for (unsigned long long int col=0; col<m_cols; col++){
                m_init_state[row][col]=false;
                m_layout->itemAt(i)->widget()->setStyleSheet("QPushButton#" + m_layout->itemAt(i)->widget()->objectName() +
                                                                                         "{background: #" + DEAD_COLOR + "}"
                                                                                         "QPushButton#" +  m_layout->itemAt(i)->widget()->objectName() + ":hover "
                                                                                         "{background: #" + HOVER_COLOR + "}");
                i++;
            }
        }
        m_live_tiles = m_init_state;
        m_live_tiles_at_next_step = m_init_state;

        ui->initAntGeneration->setEnabled(true);
        ui->add_ant->setEnabled(true);
        ui->save_file->setEnabled(true);
    }



}

void MainWindow::deleteTimer(){
    if (m_timer != nullptr){
        delete m_timer;
        m_timer = nullptr;
    }
}

void MainWindow::on_save_init_state_clicked()
{
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Are you sure?", "This will rewrite your current init state, are you sure you want to continue?", QMessageBox::Yes|QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        m_init_state = m_live_tiles;
        if (m_ant == nullptr) ui->add_ant->setEnabled(true);
        ui->save_file->setEnabled(true);
    }
}

//-------- Ant -----------//

void MainWindow::setAntHover(unsigned long long int row, unsigned long long int col){
    unsigned long long int i = row * m_cols + col;
    if (m_live_tiles[row][col]) m_layout->itemAt(i)->widget()->setStyleSheet("QPushButton#" + m_layout->itemAt(i)->widget()->objectName() +
                                                                             "{background: #" + ALIVE_COLOR + "}"
                                                                             "QPushButton#" +  m_layout->itemAt(i)->widget()->objectName() + ":hover "
                                                                             "{background: #" + HOVER_ANT_COLOR + "}");
    else m_layout->itemAt(i)->widget()->setStyleSheet("QPushButton#" + m_layout->itemAt(i)->widget()->objectName() +
                                                      "{background: #" + DEAD_COLOR + "}"
                                                      "QPushButton#" +  m_layout->itemAt(i)->widget()->objectName() + ":hover "
                                                      "{background: #" + HOVER_ANT_COLOR + "}");
}

void MainWindow::deleteAnt(){
    if (m_ant != nullptr){
        unsigned long long int index = m_ant->getPositionIndex(m_cols);
        unsigned long long int row, col;
        row = index / m_cols;
        col = index % m_cols;
        setTileColor(row, col);
        delete m_ant;
        m_ant = nullptr;
        if (m_game_paused) {
            ui->initAntGeneration->setEnabled(false);
            ui->start_ant->setEnabled(false);
            ui->move_ant->setEnabled(false);
            ui->add_ant->setEnabled(false);
        }
    }
}

void MainWindow::addAnt(unsigned long long int row, unsigned long long int col){
    m_ant = new Ant(row, col);
    m_ant_gen = 0;
    onTileClicked(); // vyrusi druhe onTileClicked()
    ui->move_ant->setEnabled(true);
    ui->start_ant->setEnabled(true);
    ui->add_ant->setEnabled(false);
}

void MainWindow::on_add_ant_clicked()
{
    if (m_generation == 0 and m_ant == nullptr) {
        // posloupnost: on_add_ant_clicked -> chooseAntTile -> addAnt
        for (unsigned long long int row=0; row<m_rows; row++){
            for (unsigned long long int col=0; col<m_cols; col++){
                connect(m_tile_buttons[row][col], &QPushButton::clicked, this, &MainWindow::chooseAntTile); // propojeni tlacitka s funkci chooseAntTile()
                setAntHover(row, col);
            }
        }
    }
    else {
        QMessageBox::information(this, "Oops!", "You can not add an Ant right now, sorry");
    }

}

void MainWindow::chooseAntTile(){
    QObject *senderObj = sender();
    unsigned long long index = senderObj->objectName().toULongLong();
    unsigned long long int row, col;
    row = index / m_cols;
    col = index % m_cols;

    unsigned long long int i = 0;
    for (unsigned long long int row=0; row<m_rows; row++){
        for (unsigned long long int col=0; col<m_cols; col++){
            disconnect(m_tile_buttons[row][col], &QPushButton::clicked, this, &MainWindow::chooseAntTile); // ukonceni propojeni tlacitka s funkci chooseAntTile()
            setTileColor(row, col);
            i++;
        }
    }

    addAnt(row, col);
    m_layout->itemAt(index)->widget()->setStyleSheet("QPushButton#" + m_layout->itemAt(index)->widget()->objectName() +
                                                                             "{background: #" + ANT_COLOR + "}"
                                                                             "QPushButton#" +  m_layout->itemAt(index)->widget()->objectName() + ":hover "
                                                                             "{background: #" + HOVER_COLOR + "}");
}

void MainWindow::moveAnt(){
    m_ant_gen++;
    ui->ant_gen->setText(QString::number(m_ant_gen));
    // zjisteni pozice
    unsigned long long int tileIndex = m_ant->getPositionIndex(m_cols);
    unsigned long long int row, col, next_row, next_col;
    row = tileIndex / m_cols;
    col = tileIndex % m_cols;
    // otoceni - pokud je prazdne (false) tak doprava, pokud je nazivu (true) tak doleva
    if (m_live_tiles[row][col]) m_ant->changeDirection("left");
    else m_ant->changeDirection("right");
    // zmena hodnoty a barvy policka
    m_live_tiles[row][col]=!m_live_tiles[row][col];
    m_live_tiles_at_next_step[row][col]=!m_live_tiles_at_next_step[row][col];
    setTileColor(row, col);
    // vypocet novych souradnic pro presun na nove policko
    next_row = row;
    next_col = col;
    if (m_ant->getDirection() == "up") next_row = row-1;
    if (m_ant->getDirection() == "down") next_row = row+1;
    if (m_ant->getDirection() == "right") next_col = col+1;
    if (m_ant->getDirection() == "left") next_col = col-1;
    // kontrola hrancnich policek a pripadne prepocitani
    if (next_row >= m_rows){
        if (next_row == m_rows) next_row=0; // prekroceni spodni hrany
        else next_row = m_rows-1; // prekroceni horni hrany a preteceni
    }
    if (next_col >= m_cols){
        if (next_col == m_cols) next_col=0; // prekroceni prave hrany
        else next_col = m_cols-1; // prekroceni leve hrany a preteceni
    }
    // presun
    m_ant->setPosition(next_row, next_col);
    // prebarveni policka na kterem se nachazi Ant
    m_layout->itemAt(m_ant->getPositionIndex(m_cols))->widget()->setStyleSheet("QPushButton#" + m_layout->itemAt(m_ant->getPositionIndex(m_cols))->widget()->objectName() +
                                                                             "{background: #" + ANT_COLOR + "}"
                                                                             "QPushButton#" +  m_layout->itemAt(m_ant->getPositionIndex(m_cols))->widget()->objectName() + ":hover "
                                                                             "{background: #" + HOVER_COLOR + "}");
    //qDebug() << "policko " << row << " " << col << " nastaveno na " << m_live_tiles[row][col];
    //qDebug() << "prechod do policka " << next_row << " " << next_col ;
}

void MainWindow::on_start_ant_clicked()
{
//    if (m_generation == 0) {
//        m_live_tiles = m_init_state;
//    }
    if (m_game_paused){
        m_timer = new QTimer(this);
        connect(m_timer, SIGNAL(timeout()), this, SLOT(on_move_ant_clicked()));
        m_timer->start(m_speed);
        m_game_paused = false;
        ui->start_ant->setText("Stop Ant");
        ui->initAntGeneration->setEnabled(false);
        ui->start->setEnabled(false);
        ui->next_step->setEnabled(false);
        ui->reset->setEnabled(false);
        ui->clear->setEnabled(false);
        ui->save_init_state->setEnabled(false);
        ui->save_file->setEnabled(false);
        ui->load_file->setEnabled(false);
    } else {
        m_timer->stop();
        deleteTimer();
        m_game_paused = true;
        ui->start_ant->setText("Start Ant");
        ui->initAntGeneration->setEnabled(true);
        ui->start->setEnabled(true);
        ui->next_step->setEnabled(true);
        ui->reset->setEnabled(true);
        ui->clear->setEnabled(true);
        ui->save_init_state->setEnabled(true);
        ui->save_file->setEnabled(true);
        ui->load_file->setEnabled(true);
    }
}

void MainWindow::on_move_ant_clicked()
{
    if (m_ant != nullptr){
        if (ui->initAntGeneration->value() == 0 or ui->initAntGeneration->value() > m_ant_gen or m_game_paused) moveAnt();
        else {
            on_start_ant_clicked();
        }
    }
}

void MainWindow::initAntGeneration(){
    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(on_move_ant_clicked()));
    m_timer->start(m_speed);
    m_game_paused = false;
    ui->start_ant->setText("Stop Ant");
}

//-------- soubory -----------//

void MainWindow::on_save_file_clicked()
{
    if (m_init_state != m_live_tiles) {
        QMessageBox::warning(this, "I can not save this", "Only init states can be saved. Save this state as init, or reset init state and try it again!");
    }
    else {
        QString dir_path(QDir::currentPath()+"/initFiles/");
        QDir dir(dir_path);
        if (!dir.exists()){
          dir.mkdir(".");
        }

        QString file_path = dir_path + QDateTime::currentDateTime().toString("dd.MM.yyyy-hh.mm.ss") + ".txt";
        QFile file(file_path);
        if (!file.open(QFile::WriteOnly | QFile::Text)){
            QMessageBox::warning(this, "Sorry", "Can not write the file");
        } else {
            QTextStream out(&file);
            for (unsigned long long int row=0; row<m_rows; row++){
                for (unsigned long long int col=0; col<m_cols; col++){
                    if (m_init_state[row][col]) out << "1";
                    else out << "0";
                }
            }

            file.flush();
            file.close();
            QMessageBox::information(this, "Saved", "File successfully saved!");
        }
    }
}

void MainWindow::on_load_file_clicked()
{
    QString filter = "All Files (*.*) ;; Text Files (*.txt) ;; Porno (*.porn)";
    QString file_path = QFileDialog::getOpenFileName(this, "open a file", QDir::currentPath()+"/initFiles/", filter);
    QFile file(file_path);

    if (!file.open(QFile::ReadOnly | QFile::Text)){
        QMessageBox::warning(this, "Sorry", "Can not open this file");
    } else {
        QTextStream in(&file);
        for (unsigned long long int row=0; row<m_rows; row++){
            for (unsigned long long int col=0; col<m_cols; col++){
                QString tile = in.read(1);
                bool alive_tile = false;
                if (tile == "1") alive_tile = true;
                m_init_state[row][col] = alive_tile;
            }
        }
        file.close();
        on_reset_clicked();
    }
}


//-------- patterns: gliders -----------//

// posloupnost: on_glider_x_y_clicked -> on_add_pattern_clicked -> choosePatternPosition -> addGlider -> setTrueTile

void MainWindow::on_glider_d_r_clicked()
{
    m_add_pattern = "glider_down_right";
    on_add_pattern_clicked();
}

void MainWindow::on_glider_d_l_clicked()
{
    m_add_pattern = "glider_down_left";
    on_add_pattern_clicked();
}

void MainWindow::on_glider_u_l_clicked()
{
    m_add_pattern = "glider_up_left";
    on_add_pattern_clicked();
}

void MainWindow::on_glider_u_r_clicked()
{
    m_add_pattern = "glider_up_right";
    on_add_pattern_clicked();
}

void MainWindow::addGlider(unsigned long long int row, unsigned long long int col){
    if (m_add_pattern == "glider_down_right") {
        // setTrueTile(row, col); obstara onTileClicked();
        setTrueTile(row-1, col);
        setTrueTile(row-2, col);
        setTrueTile(row, col-1);
        setTrueTile(row-1, col-2);
    } else if (m_add_pattern == "glider_down_left") {
        setTrueTile(row-2, col-2);
        setTrueTile(row-1, col-2);
        setTrueTile(row, col-2);
        setTrueTile(row, col-1);
        setTrueTile(row-1, col);
        onTileClicked(); // counter na pozici row col
    } else if (m_add_pattern == "glider_up_left") {
        setTrueTile(row-2, col-2);
        setTrueTile(row-1, col-2);
        setTrueTile(row, col-2);
        setTrueTile(row-2, col-1);
        setTrueTile(row-1, col);
        onTileClicked(); // counter na pozici row col
    } else if (m_add_pattern == "glider_up_right") {
        // setTrueTile(row, col); obstara onTileClicked();
        setTrueTile(row-1, col);
        setTrueTile(row-2, col);
        setTrueTile(row-2, col-1);
        setTrueTile(row-1, col-2);
    }
    m_live_tiles_at_next_step=m_live_tiles;
}

//-------- patterns: spaceship -----------//

void MainWindow::on_spaceship_right_clicked()
{
    m_add_pattern = "spaceship_right";
    on_add_pattern_clicked();
}

void MainWindow::on_spaceship_left_clicked()
{
    m_add_pattern = "spaceship_left";
    on_add_pattern_clicked();
}

void MainWindow::on_spaceship_down_clicked()
{
    m_add_pattern = "spaceship_down";
    on_add_pattern_clicked();
}

void MainWindow::on_spaceship_up_clicked()
{
    m_add_pattern = "spaceship_up";
    on_add_pattern_clicked();
}

void MainWindow::addSpaceship(unsigned long long int row, unsigned long long int col){
    int i=1;
    for (int r=3; r>=0; r--){
        for (int c=4; c>=0; c--){
            if (m_add_pattern == "spaceship_right") {
                if (i==1 or i==4 or i==5 or i==10 or i==13 or i==16 or i==17 or i==20) ; // do nothing - prazdne policko
                else setTrueTile(row-r, col-c); // pole jsem prosel po radcich
            } else if (m_add_pattern == "spaceship_left"){
                if (i==1 or i==2 or i==5 or i==6 or i==13 or i==16 or i==19 or i==20) ;
                else setTrueTile(row-r, col-c);
            } else if (m_add_pattern == "spaceship_down") {
                if (i==1 or i==4 or i==5 or i==10 or i==13 or i==16 or i==17 or i==20) ; // stejna podminka jak spaceship_right
                else setTrueTile(row-c, col-r); // ale prochazim po sloupcich!
            } else if (m_add_pattern == "spaceship_up") {
                if (i==1 or i==2 or i==5 or i==6 or i==13 or i==16 or i==19 or i==20) ;
                else setTrueTile(row-c, col-r); // zase po sloupcich
            }
            i++;
        }
    }
    onTileClicked(); // counter na pozici row col
    m_live_tiles_at_next_step=m_live_tiles;
}

//-------- patterns: pulsar -----------//

void MainWindow::on_pulsar_pattern_clicked()
{
    m_add_pattern = "pulsar";
    on_add_pattern_clicked();
}

void MainWindow::addPulsar(unsigned long long int row, unsigned long long int col){
    for (int r=12; r>=0; r--){
        for (int c=12; c>=0; c--){
            if (((r==12 or r==7 or r==5 or r==0) and (c==10 or c==9 or c==8 or c==4 or c==3 or c==2)) or
                ((c==12 or c==7 or c==5 or c==0) and (r==10 or r==9 or r==8 or r==4 or r==3 or r==2))) setTrueTile(row-r, col-c);
        }
    }
    onTileClicked(); // counter na pozici row col
    m_live_tiles_at_next_step=m_live_tiles;
}

//-------- patterns -----------//

void MainWindow::on_add_pattern_clicked(){
    if (m_generation == 0) {
        for (unsigned long long int row=0; row<m_rows; row++){
            for (unsigned long long int col=0; col<m_cols; col++){
                connect(m_tile_buttons[row][col], &QPushButton::clicked, this, &MainWindow::choosePatternPosition); // propojeni tlacitka s funkci chooseAntTile()
            }
        }
        on_patterns_clicked();
    } else {
        QMessageBox::information(this, "Oops!", "You have to save or reset this state first");
    }
}

void MainWindow::choosePatternPosition(){
    QObject *senderObj = sender();
    unsigned long long index = senderObj->objectName().toULongLong();
    unsigned long long int row, col;
    row = index / m_cols;
    col = index % m_cols;

    for (unsigned long long int row=0; row<m_rows; row++){
        for (unsigned long long int col=0; col<m_cols; col++){
            disconnect(m_tile_buttons[row][col], &QPushButton::clicked, this, &MainWindow::choosePatternPosition); // ukonceni propojeni tlacitka s funkci chooseGliderPosition()
            setTileColor(row, col);
        }
    }

    if ((m_add_pattern == "glider_down_right") or (m_add_pattern == "glider_down_left") or (m_add_pattern == "glider_up_right") or (m_add_pattern == "glider_up_left")) addGlider(row, col);
    if ((m_add_pattern == "spaceship_right") or (m_add_pattern == "spaceship_left") or (m_add_pattern == "spaceship_down") or (m_add_pattern == "spaceship_up")) addSpaceship(row, col);
    if (m_add_pattern == "pulsar") addPulsar(row, col);
}

void MainWindow::setTrueTile(unsigned long long int row, unsigned long long int col){
    signed long long int sig_row, sig_col;
    sig_row = (signed long long int)row;
    sig_col = (signed long long int)col;

    qDebug() << "------------------";
    qDebug() << "row a col:" << row << " " << col;
    qDebug() << "sig row a col:" << sig_row << " " << sig_col;

    if (sig_row >= m_rows) row = 0 + sig_row - m_rows; // prekroceni spodni hrany
    if (sig_row < 0) row = m_rows + sig_row ; // prekroceni horni hrany
    if (sig_col >= m_cols) col = 0 + sig_col - m_cols; // prekroceni prave hrany
    if (sig_col < 0) col = m_cols + sig_col ; // prekroceni leve hrany

    qDebug() << "nove row a col:" << row << " " << col;

    m_init_state[row][col]=true;
    m_live_tiles[row][col]=true;
    setTileColor(row, col);
}

void MainWindow::on_patterns_clicked()
{
    QPropertyAnimation *animation;
    animation = new QPropertyAnimation(ui->patterns_group, "geometry");
    animation->setDuration(500);
    animation->setEasingCurve(QEasingCurve::InOutBack);
    if (ui->patterns_group->geometry().y() != PATTERNS_Y_UNDER_SCREEN){ //schovej
        animation->setStartValue(QRect(ui->patterns_group->geometry().x(), PATTERNS_Y_ON_SCREEN,  ui->patterns_group->geometry().width(),  ui->patterns_group->geometry().height()));
        animation->setEndValue(QRect(ui->patterns_group->geometry().x(), PATTERNS_Y_UNDER_SCREEN,  ui->patterns_group->geometry().width(),  ui->patterns_group->geometry().height()));
    } else { // ukaz
        animation->setStartValue(QRect(ui->patterns_group->geometry().x(), PATTERNS_Y_UNDER_SCREEN,  ui->patterns_group->geometry().width(),  ui->patterns_group->geometry().height()));
        animation->setEndValue(QRect(ui->patterns_group->geometry().x(), PATTERNS_Y_ON_SCREEN,  ui->patterns_group->geometry().width(),  ui->patterns_group->geometry().height()));
    }
    animation->start();
}


//void MainWindow::setGliderHover(){
//    qDebug() << "provolano";
//    QObject *senderObj = sender();
//    unsigned long long i = senderObj->objectName().toULongLong();
//    unsigned long long int row, col;
//    row = i / m_cols;
//    col = i % m_cols;

//    for (int r=-4; r<=0; r++){
//        for (int c=-4; c<=0; c++){
//            int index = (row+r) * m_cols + (col+c);
//            if ((r==-3 and c==-1) or (r==-2 and c==-1) or (r==-1 and c==-1) or (r==-1 and c==-2) or (r==-2 and c==-3))
//            m_layout->itemAt(index)->widget()->setStyleSheet("QPushButton#" + m_layout->itemAt(index)->widget()->objectName() + "{background: #" + GLIDER_HOVER_COLOR + "}");
//            else setTileColor(row+r, col+c);
//        }
//    }
//}




void MainWindow::on_about_button_clicked()
{
    QPropertyAnimation *animation;
    animation = new QPropertyAnimation(ui->about_article, "geometry");
    animation->setDuration(500);
    animation->setEasingCurve(QEasingCurve::InOutBack);
    if (ui->about_article->geometry().x() != ABOUT_X_OUT_OF_SCREEN){ //schovej
        animation->setStartValue(QRect(ABOUT_X_ON_SCREEN, ui->about_article->geometry().y(),  ui->about_article->geometry().width(),  ui->about_article->geometry().height()));
        animation->setEndValue(QRect(ABOUT_X_OUT_OF_SCREEN,  ui->about_article->geometry().y(),  ui->about_article->geometry().width(),  ui->about_article->geometry().height()));
    } else { // ukaz
        animation->setStartValue(QRect(ABOUT_X_OUT_OF_SCREEN, ui->about_article->geometry().y(),  ui->about_article->geometry().width(),  ui->about_article->geometry().height()));
        animation->setEndValue(QRect(ABOUT_X_ON_SCREEN,  ui->about_article->geometry().y(),  ui->about_article->geometry().width(),  ui->about_article->geometry().height()));
    }
    animation->start();
}


