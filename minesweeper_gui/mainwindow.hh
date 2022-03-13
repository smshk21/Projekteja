// ---------------------------------------------------------------------
// Elias Keso / Norssi
// elias.keso@tuni.fi
// TUNI-tunnus: mrelke
// Opiskelijanumero: 150580881
// ---------------------------------------------------------------------
// Lisäinformaatio .cpp- ja instructions.txt -tiedostoissa.
#ifndef MAINWINDOW_HH
#define MAINWINDOW_HH

#include "gameboard.hh"
#include <QMainWindow>
#include <QGridLayout>
#include <QPushButton>
#include <QStyle>
#include <QSpinBox>
#include <algorithm>
#include <QLabel>
#include <iostream>
#include <QLCDNumber>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:

    // Tietue, johon tallennetaan jokaisen graafisen peliruudun tiedot.
    // Muut aika yksiselitteisiä, sisalto = mikä kuva napin päällä on,
    // eli "lippu", "kysymysmerkki" tai "tyhja".
    // tila = "auki" tai k"iinni".
    struct Kentta{
        int x;
        int y;
        QPushButton* nappi;
        QString sisalto;
        QString tila;
    };

    Ui::MainWindow *ui;

    // Alustaa laudan graafisen sekä ei-graafisen käyttöliittymän.
    void teeLaudanGui();

    // Käsittelee pelikenttien painallukset.
    void peliKenttaaPainettu();

    // Muuttaa lipun, avaamisen ja kysymysmerkin välillä.
    void muutaValinta();

    // Avaa käyttöliittymässä parametrina annetun kentän/ruudun.
    void avaaKentta(Square* klikattu_ruutu, Kentta* ruudun_tietue);

    // Avaa kaikki miinaruudut ja disabloi kaikki kentät. Pysäyttää ajastimen.
    void lopetaPelaaminen();

    // Saa sekunnin välein ajastimelta kutsun, jolloin se nostaa
    // aikaa sekunnin verran lcd-tauluilla.
    void naytaAika();

    // tarkistaa, ovatko kaikki tähän mennessä asetetut liput oikeilla
    // paikoilla.
    void onkoKaikkiOk();


    // Muuttujat pushButtoneihin laitettavia kuvia varten.
    QPixmap miinakuva_pix_;
    QIcon miinakuva_;

    QPixmap lippukuva_pix_;
    QIcon lippukuva_;

    QPixmap avauskuva_pix_;
    QIcon avauskuva_;

    QPixmap kysymyskuva_pix_;
    QIcon kysymyskuva_;


    // Käyttäjän syöttämä siemenluku
    QSpinBox* siemen_;

    // Kehote siemenluvun syöttämiseen.
    QLabel* syota_siemen_;

    // Kulunutta aikaa varten näyttämiseen
    QTimer* ajastin_;
    QLCDNumber* lcdmin_;
    QLCDNumber* lcdsek_;
    int min_;
    int sek_;
    QLabel* aika_ohje_;
    QLabel* min_ohje_;
    QLabel* sek_ohje_;

    // Nappi, josta käyttäjä voi tsekata, onko kaikki tähän mennessä
    // asetetut liput oikeissa paikoissa.
    QPushButton* kaikki_ok_;
    // Label, johon tulostetaan yllä olevaan vastaus
    QLabel* ok_tilanne_;

    // Sisältää käyttäjän valinnan että mitä tapahtuu, kun hän painaa
    // (miina)kenttää.
    QString valittu_;
    QLabel* tulosilmoitus_;

    // Nappi, josta käyttäjä voi muuttaa valintaansa välillä
    // avaaminen/lipun asettaminen/kysymysmerkin asettaminen.
    QPushButton* valintanappi_;

    // Pelattavan miinaharavapelin ei-graafinen versio. Kaikki operaatiot
    // suoritetaan tälle, ja kopioidaan graafiseen käyttöliittymään.
    GameBoard pelilauta_;

    // Mainwindow:n centralWidget
    QGridLayout* asetelma_;


    // Sisältää kaikki pelikentän QPushButton:it.
    std::vector<Kentta> painonapit_;
};
#endif // MAINWINDOW_HH
