// Toteuttaa ristinollan graafisen käyttöliittymän ja
// pitää apuna tallessa tietoa ei-graafisen miinaharavapelin sisällöstä.
// ---------------------------------------------------------------------
// Elias Keso / Norssi
// elias.keso@tuni.fi
// TUNI-tunnus: mrelke
// Opiskelijanumero: 150580881
// ---------------------------------------------------------------------¨
#include "mainwindow.hh"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    // Lippu- ja miinakuvien (ikonien) sijoitus muuttujiin.    
    , miinakuva_pix_(":/kuvat/miina.png")
    , miinakuva_(miinakuva_pix_)
    , lippukuva_pix_(":/kuvat/lippu.png")
    , lippukuva_(lippukuva_pix_)
    , avauskuva_pix_(":/kuvat/open.png")
    , avauskuva_(avauskuva_pix_)
    , kysymyskuva_pix_(":/kuvat/kysymys.png")
    , kysymyskuva_(kysymyskuva_pix_)

{

    ui->setupUi(this);

    tulosilmoitus_ = new QLabel(this);

    // Keskelle asetettava grid-layout.
    auto keski = new QWidget;
    asetelma_ = new QGridLayout(keski);
    setCentralWidget(keski);

    syota_siemen_ = new QLabel(this);
    syota_siemen_->setText("Syötä siemenluku:");
    asetelma_->addWidget(syota_siemen_, 1, BOARD_SIDE + 2);

    siemen_= new QSpinBox(this);
    siemen_->setMinimum(1);
    asetelma_->addWidget(siemen_, 2, BOARD_SIDE + 2);

    QPushButton* aloita_peli = new QPushButton("Uusi peli", this);
    asetelma_->addWidget(aloita_peli, 3, BOARD_SIDE + 2);
    connect(aloita_peli, &QPushButton::clicked, this,
            &MainWindow::teeLaudanGui);

    lcdmin_ = new QLCDNumber(this);
    lcdmin_->setAutoFillBackground(true);
    lcdmin_->setPalette(Qt::red);
    asetelma_->addWidget(lcdmin_, 0, BOARD_SIDE/2-1);

    lcdsek_ = new QLCDNumber(this);
    lcdsek_->setAutoFillBackground(true);
    lcdsek_->setPalette(Qt::red);
    asetelma_->addWidget(lcdsek_, 0, BOARD_SIDE/2 +1);

    aika_ohje_ = new QLabel(this);
    aika_ohje_->setText("Kulunut aika:");
    asetelma_->addWidget(aika_ohje_, 0, BOARD_SIDE/2 -2);

    min_ohje_ = new QLabel(this);
    min_ohje_->setText("min.");
    asetelma_->addWidget(min_ohje_, 0, BOARD_SIDE/2);

    sek_ohje_ = new QLabel(this);
    sek_ohje_->setText("sek.");
    asetelma_->addWidget(sek_ohje_, 0, BOARD_SIDE/2 +2);

    ajastin_ = new QTimer(this);
    connect(ajastin_, &QTimer::timeout, this, &MainWindow::naytaAika);

    kaikki_ok_ = new QPushButton(this);
    kaikki_ok_->setText("Kaikki OK?");
    asetelma_->addWidget(kaikki_ok_, 4, BOARD_SIDE + 2);
    connect(kaikki_ok_, &QPushButton::clicked, this,
            &MainWindow::onkoKaikkiOk);

    ok_tilanne_ = new QLabel(this);
    asetelma_->addWidget(ok_tilanne_, 4, BOARD_SIDE + 3);



}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::teeLaudanGui()
{

    // Alkaa täten nollasta; jos pistäisi sek_=0, niin alkaisi ykkösestä.
    sek_ = -1;
    min_ = 0;
    ajastin_->start(1000);
    // Muilla kuin ensimmäisillä tämän funktion kutsukerroilla seuraavat
    // muuttujat sisältävät tietoa edellisestä pelistä, joten poistetaan se.
    painonapit_.clear();
    pelilauta_.tyhjenna();
    tulosilmoitus_->clear();

    // Alustetaan pelin toimintoja varten rinnakkainen GameBoard -muuttuja.
    pelilauta_.init(siemen_->value());

    // Luodaan QGridLayout, joka tulee sisältämään kaikki minesweeperin napit.
    for (int y = 0; y < BOARD_SIDE; ++y)
    {

        for (int x = 0; x < BOARD_SIDE; ++x)
        {
            // Luodaan uusi nappi kyseisillä paikoilla gridLayoutissa.
            QPushButton* nappi = new QPushButton(this);
            connect(nappi, &QPushButton::clicked, this,
                    &MainWindow::peliKenttaaPainettu);
            nappi->setPalette(QPalette(Qt::blue));
            asetelma_->addWidget(nappi, x+1, y);

            // Lisätään napin tiedot napit sisältävään vektoriin.
            Kentta yksi = {y, x, nappi, "tyhja", "kiinni"};
            painonapit_.push_back(yksi);

        }
    }
    // Lisätään painike, josta käyttäjä voi valita mitä hän haluaa tapahtuvan
    // miinakenttää painaessa.
    valintanappi_ = new QPushButton(this);
    // Aluksi oletustoiminto on kentän avaus.
    valintanappi_->setIcon(avauskuva_);
    valittu_ = "avaa";
    connect(valintanappi_, &QPushButton::clicked, this,
            &MainWindow::muutaValinta);
    asetelma_->addWidget(valintanappi_, 0, BOARD_SIDE +2);



    // Yhdistetään napit toisiinsa kiinni
    // ja asetetaan ne layoutissa pääikkunaan.
    asetelma_->setSizeConstraint(QLayout::SetFixedSize);
    asetelma_->setSpacing(0);

}

void MainWindow::peliKenttaaPainettu()
{
    // Käydään läpi tietueita, kunnes löytyy painetun napin tietue.
    for (auto& tietue : painonapit_)
    {
        if (tietue.nappi == sender())
            {
                Square* klikattu_ruutu = pelilauta_.getSquare(tietue.x,
                                                             tietue.y);
                if (valittu_ == "avaa")
                {
                    // Lipullista tai kysymysmerkkistä ruutua ei pysty
                    // avaamaan.
                    if (tietue.sisalto != "tyhja")
                    {
                        return;
                    }

                    avaaKentta(klikattu_ruutu, &tietue);
                    if (pelilauta_.isGameOver()){
                        lopetaPelaaminen();
                        tulosilmoitus_->setText("Voitit pelin!");
                    }
                    return;
                }
                else if (valittu_ == "lippu")
                {
                    // Jos ruudussa on jo valmiiksi lippu, niin poistetaan se.
                    if (tietue.sisalto == "lippu")
                    {
                        tietue.sisalto = "tyhja";
                        tietue.nappi->setIcon(QIcon());
                        klikattu_ruutu->removeFlag();
                        show();
                        return;
                    }
                    tietue.sisalto = "lippu";
                    tietue.nappi->setIcon(lippukuva_);
                    klikattu_ruutu->addFlag();
                    show();
                    return;
                }
                else
                {
                    // Jos ruudussa on jo valmiiksi kysymysmerkki,
                    // niin poistetaan se.
                    if (tietue.sisalto == "kysymysmerkki")
                    {
                        tietue.sisalto = "tyhja";
                        tietue.nappi->setIcon(QIcon());
                        klikattu_ruutu->poistaKysymysmerkki();
                        show();
                        return;
                    }
                    tietue.sisalto = "kysymysmerkki";
                    tietue.nappi->setIcon(kysymyskuva_);
                    klikattu_ruutu->lisaaKysymysmerkki();
                    show();
                    return;
                }
            }


    }
}

void MainWindow::muutaValinta()
{
    if (valittu_ == "avaa")
    {
        valittu_ = "lippu";
        valintanappi_->setIcon(lippukuva_);
        show();
    }
    else if (valittu_ == "lippu")
    {
        valittu_ = "kysymys";
        valintanappi_->setIcon(kysymyskuva_);
        show();
    }
    else
    {
        valittu_ = "avaa";
        valintanappi_->setIcon(avauskuva_);
        show();
    }
}

void MainWindow::avaaKentta(Square* klikattu_ruutu, Kentta* ruudun_tietue)
{

    // Avatussa ruudussa ei ole miinaa, jos alla oleva palauttaa true.
    if (klikattu_ruutu->open()){

        // Avataan ruutu ei-graafisesti
        pelilauta_.openSquare(ruudun_tietue->x, ruudun_tietue->y);

        // Käydään läpi kaikki kentän ruudut, ja jos edellinen ei-graafinen
        // ruudun avaus avasi ruutuja, jotka ovat vielä graafisesti kiinni,
        // niin avataan nekin graafisesti.
        for (auto& ruutu : painonapit_){
            if (pelilauta_.getSquare(ruutu.x, ruutu.y)->
                    isOpen() and ruutu.tila == "kiinni")
            {
                // Tarvitsee vain asettaa miinaan tieto siitä, kuinka
                // monta miinaa sen vieressä on.
                // Disabloidaan myös nappi, koska sillä
                // ei voi enää käyttäjä tehdä mitään.
                ruutu.nappi->setText(QString::number(
            pelilauta_.getSquare(ruutu.x, ruutu.y)->countAdjacent()));
                ruutu.nappi->setDisabled(true);
                ruutu.tila = "auki";
            }
        }
    }
    else
    {

        // Käyttäjän avaamassa ruudussa oli miina, joten tulostetaan
        // ilmoitus häviöstä ja muutetaan painonapin kuvaksi miina.
        tulosilmoitus_->setText("Hävisit pelin!");
        asetelma_->addWidget(tulosilmoitus_, BOARD_SIDE + 3, BOARD_SIDE);
        ruudun_tietue->nappi->setIcon(miinakuva_);
        ruudun_tietue->tila = "auki";

        // Jos jossain ruudussa on kysymysmerkki ja painetaan miinaa,
        // niin kysymysmerkit eivät poistu tyhjistä ruuduista, joten
        // poistetaan ne.
        for (auto& ruutu : painonapit_)
        {
            if (ruutu.sisalto == "kysymysmerkki")
            {
                ruutu.nappi->setIcon(QIcon());
            }
        }
        lopetaPelaaminen();
        return;
    }

}

void MainWindow::lopetaPelaaminen()
{
    // Käy kaikki pelikentän painonapit läpi ja disabloi ne.
    // Jos ruutu sisältää miinan, niin avataan se.
    for (auto& nappitietue : painonapit_)
    {
        if (pelilauta_.getSquare(nappitietue.x, nappitietue.y)->hasMine())
        {
            nappitietue.nappi->setIcon(miinakuva_);
        }
        nappitietue.nappi->setDisabled(true);
        show();
    }
    ajastin_->stop();
}

void MainWindow::naytaAika()
{
    if (sek_ == 59)
    {
        sek_ = 0;
        ++min_;
    }
    else
    {
        ++sek_;
    }

    lcdsek_->display(sek_);
    lcdmin_->display(min_);
    show();

}

void MainWindow::onkoKaikkiOk()
{
    if (pelilauta_.liputOikein())
    {
        ok_tilanne_->setText("Kyllä!");
    }
    else
    {
        ok_tilanne_->setText("Ei.");
    }
    show();
}


