//Mehmet Ali GÜllüce 240201098
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <windows.h>
#include <urlmon.h>
using namespace std;

// Structlar
struct Struct_okuma {
    float angle_min = 0.0f;
    float angle_max = 0.0f;
    float angle_increment = 0.0f;
    float range_min = 0.0f;
    float range_max = 0.0f;
    float time_increment = 0.0f;
    vector<float> ranges;
};

struct nokta{
    float x;
    float y;
};

struct denklem {
    float A;
    float B;
    float C;
};

struct dogru{
    denklem d;
    vector<nokta> destekciler;
};

struct kesisim{
    bool kesisti=false;
    nokta kesnoktasi;
    float aci;
    float mesafe;
    int i=-1,j=-1;
};




// Fonksiyonlar
void Dosya_indirme(const string& ,const string&);
void Dosya_okuma(const string& ,Struct_okuma&);
vector<nokta> Nokta_uret(const Struct_okuma&);
vector<dogru> En_iyi_dogrular( vector<nokta>&);
nokta Kordinat_Donusturucu(const nokta& ,int ,int ,float);
kesisim Kesisim_Noktasi(const vector <dogru>&);
void Allegro_cizim(const vector<nokta>& ,const vector<dogru>& ,const kesisim&);
void Ciz_grafik(int ,int);
void Ciz_bilgiler(const vector<dogru>&, int ,int , int );
void Ciz_noktalar(const vector<nokta>&, float , int, int , int , int );
void Ciz_dogrular(const vector<dogru>&, float, int , int , int , int);
void Ciz_kesisim_ve_robot(const kesisim& ,float ,int ,int ,int ,int );


//Main
int main()
{
srand(time(NULL));

Struct_okuma okuma;
int sayi;

cout<<"Dosya numarasi giriniz [1-5] : "<<endl;
cin>>sayi;

if(sayi>5 || sayi<1) {
    cerr << "Hata: Gecersiz dosya numarasi. 1-5 arasi olmali." << endl;
    return 1;
}
string url="http://abilgisayar.kocaeli.edu.tr/lidar"+to_string(sayi)+".toml";
string dosya_adi="lidar"+to_string(sayi)+".toml";


Dosya_indirme(url,dosya_adi);
Dosya_okuma(dosya_adi,okuma);
vector<nokta> nokta_bulutu = Nokta_uret(okuma);
vector<dogru> dogru_bulutu = En_iyi_dogrular(nokta_bulutu);
kesisim sonuc=Kesisim_Noktasi(dogru_bulutu);
Allegro_cizim(nokta_bulutu, dogru_bulutu, sonuc);

    return 0;
}



//Fonksiyon içerikleri

void Dosya_indirme(const string& url_adi,const string& dosya_adi)
{
    const char* url = url_adi.c_str();

    char fullPath[MAX_PATH];
    GetFullPathNameA(dosya_adi.c_str(), MAX_PATH, fullPath, NULL); //Dosya Yolu

    const char* dst = fullPath;

    HRESULT hr = URLDownloadToFileA(NULL, url, dst, 0, NULL);

    if (hr == S_OK) {
        cout << dosya_adi << " basariyla indirildi: " << fullPath << endl;
    } else {
        cerr << "Hata: Dosya indirilemedi! URL: " << url_adi << " Hata Kodu: " << hr << endl;
        exit(1);
    }

}

void Dosya_okuma(const string& dosya_adi, Struct_okuma &okuma)
{
ifstream MyFile(dosya_adi);
if(MyFile.is_open())
{

bool vb=0;
string temp;
vector<float> deger_vektor;
while(getline(MyFile,temp))
{
    if(!vb)
    {
        if(temp.find("ranges =")!=string::npos)
        {
         vb=true;
        }

        else if(temp.find("=")!=string::npos)
        {
        string parametre;
        float deger;
        string esittir;
        stringstream st(temp);
            if(st>>parametre>>esittir>>deger)
            {
            if(parametre=="angle_min") okuma.angle_min=deger;
            if(parametre=="angle_max") okuma.angle_max=deger;
            if(parametre=="angle_increment") okuma.angle_increment=deger;
            if(parametre=="range_min") okuma.range_min=deger;
            if(parametre=="range_max") okuma.range_max=deger;
            if(parametre=="time_increment") okuma.time_increment=deger;
            }

        }

    }


     else{

        if(temp.find("]")!=string::npos)
            {
            vb=false;
            }



        float sayi;
        stringstream sstream(temp);
            while(sstream>>sayi)
            {
            char gereksiz;
            sstream>>gereksiz;
            deger_vektor.push_back(sayi);
            }
    }

}
MyFile.close();
okuma.ranges = deger_vektor;
}
    else
    {
        cerr<<"Dosya Acilamadi. ";
        exit(1);
    }

}

vector<nokta> Nokta_uret(const Struct_okuma& okuma)
{

        vector<nokta> nokta_bulutu;

            for(int i=0;i<okuma.ranges.size();i++)
            {
                if(okuma.ranges[i]<=okuma.range_max && okuma.ranges[i]>=okuma.range_min && !isnan(okuma.ranges[i]))
                {
                float temp_angle=okuma.angle_min+(okuma.angle_increment*i);
                float x=cos(temp_angle)*okuma.ranges[i];
                float y=sin(temp_angle)*okuma.ranges[i];
                nokta temp_nokta={x,y};
                nokta_bulutu.push_back(temp_nokta);
                }
            }
return nokta_bulutu;

}


denklem dogru_denklemi(nokta n1, nokta n2) {
    denklem denklem;    // Ax+By+C
    denklem.A=n2.y-n1.y;
    denklem.B=n1.x-n2.x;
    denklem.C=-(denklem.A*n1.x)-(denklem.B*n1.y);
    return denklem;
}

float dogru_mesafe(nokta n, denklem denklem) {    //  |A*x + B*y + C| / √(A² + B²)
    float pay=(denklem.A*n.x)+(denklem.B*n.y)+denklem.C;
    if(pay<0) pay*=-1;
    float payda=sqrt(pow(denklem.A,2)+pow(denklem.B,2));
    if (payda == 0) return 0;
    return pay/payda;
}

vector<dogru> En_iyi_dogrular(vector<nokta> &nokta_bulutu)
{
float destekci_uzaklik=0.5; int uzunluk=1000;
vector<dogru> dogru_bulutu;  vector<nokta> kalan_nokta_bulutu = nokta_bulutu;

 while(kalan_nokta_bulutu.size()>=8)
 {
  dogru asil_dogru;
  asil_dogru.destekciler.clear();
  int max_destekci = 0;
  vector<nokta> max_destekciler;
  for (int i = 0; i < uzunluk; i++)
  {
   int e1 = rand() % kalan_nokta_bulutu.size();
   int e2 = rand() % kalan_nokta_bulutu.size();
   while (e1 == e2) e2 = rand() % kalan_nokta_bulutu.size();
   nokta n1 = kalan_nokta_bulutu[e1];
   nokta n2 = kalan_nokta_bulutu[e2];
   denklem suanki_dogru = dogru_denklemi(n1,n2);
   int suanki_destekci = 0;
   vector<nokta> temp_destekciler;
   for (int j=0; j<kalan_nokta_bulutu.size();j++) {
     nokta ntemp=kalan_nokta_bulutu[j];
     float uzaklik= dogru_mesafe(ntemp,suanki_dogru);
     if (uzaklik < destekci_uzaklik) {
         suanki_destekci++;
         temp_destekciler.push_back(ntemp);
     }
   }
   if (suanki_destekci>max_destekci) {
     max_destekci=suanki_destekci;
     asil_dogru.d=suanki_dogru;
     max_destekciler= temp_destekciler;
     if(asil_dogru.d.A<0|| (asil_dogru.d.A==0 && asil_dogru.d.B<0))
     {
         asil_dogru.d.A*=-1;
         asil_dogru.d.B*=-1;
         asil_dogru.d.C*=-1;
     }
   }
  }
  if(max_destekci>=8)
  {
   asil_dogru.destekciler=max_destekciler;
   dogru_bulutu.push_back(asil_dogru);
   vector <nokta> kalan_noktalar;
   for(int i=0;i<kalan_nokta_bulutu.size();i++)
   {
     nokta ntemp=kalan_nokta_bulutu[i];
     if(dogru_mesafe(ntemp,asil_dogru.d)>destekci_uzaklik)
         kalan_noktalar.push_back(ntemp);
   }
   kalan_nokta_bulutu=kalan_noktalar;
  }
  else break;
 }
 return dogru_bulutu;
}


nokta Kordinat_Donusturucu(const nokta& p, int ekran_boyutu_x,int ekran_boyutu_y, float harita_olcegi) {
    nokta ekran_noktasi;
    ekran_noktasi.x = (p.x * harita_olcegi) + (ekran_boyutu_x / 2);
    ekran_noktasi.y = (-p.y * harita_olcegi) + (ekran_boyutu_y / 2);
    return ekran_noktasi;
}


kesisim Kesisim_Noktasi(const vector<dogru>& dogru_bulutu)
{
    auto parca_ici = [](nokta p, nokta a, nokta b){
        float vx=b.x-a.x, vy=b.y-a.y;
        float wx=p.x-a.x, wy=p.y-a.y;
        float len2 = vx*vx + vy*vy;
        if(len2 < 1e-6f) return false;
        float t = (vx*wx + vy*wy) / len2;
        return t >= -0.05f && t <= 1.05f;
    };

    kesisim out; out.kesisti=false; out.mesafe=1e9f;
    int n = (int)dogru_bulutu.size();

    for(int i=0;i<n;++i){
        const auto& Di = dogru_bulutu[i];
        nokta ai = Di.destekciler[0], bi = Di.destekciler[0];
        for(const auto& p : Di.destekciler){
            if(p.x < ai.x) ai = p;
            if(p.x > bi.x) bi = p;
        }

        for(int j=i+1;j<n;++j){
            const auto& Dj = dogru_bulutu[j];

            nokta aj = Dj.destekciler[0], bj = Dj.destekciler[0];
            for(const auto& p : Dj.destekciler){
                if(p.x < aj.x) aj = p;
                if(p.x > bj.x) bj = p;
            }

            const auto& d1 = Di.d;
            const auto& d2 = Dj.d;

            float det = d1.A*d2.B - d2.A*d1.B;
            if(fabs(det) < 1e-6f) continue;

            float x = (d1.B*d2.C - d2.B*d1.C) / det;
            float y = (d2.A*d1.C - d1.A*d2.C) / det;

            nokta P{ x, y };
            if(!parca_ici(P, ai, bi)) continue;
            if(!parca_ici(P, aj, bj)) continue;

            float dot = d1.A*d2.A + d1.B*d2.B;
            float teta = fabs(atan2(det, dot)) * 180.0f / M_PI;

            if(teta >= 60 && teta <= 120){
                float mes = sqrt(x*x + y*y);
                if(mes < out.mesafe){
                    out.kesisti   = true;
                    out.kesnoktasi= P;
                    out.aci       = teta;
                    out.mesafe    = mes;
                    out.i         = i+1;
                    out.j         = j+1;
                }
            }
        }
    }
    return out;
}


/// Allegro////

void Allegro_cizim(const vector<nokta>& nokta_bulutu,const vector<dogru>& dogru_bulutu,const kesisim& sonuc)
{
    al_init(); al_init_font_addon(); al_init_ttf_addon(); al_init_primitives_addon();   al_install_keyboard();

    int EKRAN_X = 1280;    int EKRAN_Y = 1000;    float HARITA_OLCEGI = 120.0;
    int GRAF_X=100, GRAF_Y=40, GRAF_BOY=900;
    int gx = GRAF_X + GRAF_BOY/2, gy = GRAF_Y + GRAF_BOY/2;
    int DX = gx - EKRAN_X/2, DY = gy - EKRAN_Y/2;

    ALLEGRO_DISPLAY* ekran = al_create_display(EKRAN_X,EKRAN_Y);
    ALLEGRO_EVENT_QUEUE* olay_kuyrugu = al_create_event_queue();
    al_register_event_source(olay_kuyrugu, al_get_keyboard_event_source());
    al_register_event_source(olay_kuyrugu, al_get_display_event_source(ekran));

    al_clear_to_color(al_map_rgb(255, 255, 255));
    Ciz_bilgiler( dogru_bulutu,1000,60,1240);
    Ciz_grafik( EKRAN_X,  EKRAN_Y);
    Ciz_noktalar(nokta_bulutu, HARITA_OLCEGI, EKRAN_X, EKRAN_Y, DX, DY);
    Ciz_dogrular(dogru_bulutu, HARITA_OLCEGI, EKRAN_X, EKRAN_Y, DX, DY);
    Ciz_kesisim_ve_robot(sonuc, HARITA_OLCEGI, EKRAN_X, EKRAN_Y, DX, DY);

    bool devam = true;  //esc
    while (devam)
    {
        ALLEGRO_EVENT event;
        while (al_get_next_event(olay_kuyrugu, &event))
        {
            if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
                devam = false;

            if (event.type == ALLEGRO_EVENT_KEY_DOWN &&
                event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
                devam = false;
        }
        al_flip_display();
        al_rest(0.016);
    }








  al_destroy_event_queue(olay_kuyrugu);
  al_destroy_display(ekran);


}

void Ciz_noktalar(const vector<nokta>& nokta_bulutu, float HARITA_OLCEGI, int EKRAN_X, int EKRAN_Y, int DX, int DY)
{
    for (int i=0;i<nokta_bulutu.size();i++)
    {
        nokta ekran_noktasi = Kordinat_Donusturucu(nokta_bulutu[i], EKRAN_X,EKRAN_Y, HARITA_OLCEGI);
        ekran_noktasi.x += DX;
        ekran_noktasi.y += DY;

        al_draw_filled_circle(ekran_noktasi.x, ekran_noktasi.y, 5, al_map_rgb(224,224,224));
    }
}

void Ciz_dogrular(const vector<dogru>& dogru_bulutu, float HARITA_OLCEGI, int EKRAN_X, int EKRAN_Y, int DX, int DY)
{
    ALLEGRO_FONT* font = al_load_ttf_font("fonts/DejaVuSans.ttf", 16, 0);
    for (int i = 0; i < dogru_bulutu.size(); i++) {
        if (dogru_bulutu[i].destekciler.size() < 8) continue;

        nokta n1 = dogru_bulutu[i].destekciler[0];
        nokta n2 = dogru_bulutu[i].destekciler[0];

        float minX = n1.x;
        float maxX = n2.x;

        for (int j = 0; j < dogru_bulutu[i].destekciler.size(); j++) {
            if (dogru_bulutu[i].destekciler[j].x < minX) {
                minX = dogru_bulutu[i].destekciler[j].x;
                n1 = dogru_bulutu[i].destekciler[j];
            }
            if (dogru_bulutu[i].destekciler[j].x > maxX) {
                maxX = dogru_bulutu[i].destekciler[j].x;
                n2 = dogru_bulutu[i].destekciler[j];
            }
        }

        nokta e1 = Kordinat_Donusturucu(n1, EKRAN_X,EKRAN_Y, HARITA_OLCEGI);
        nokta e2 = Kordinat_Donusturucu(n2, EKRAN_X,EKRAN_Y, HARITA_OLCEGI);

        e1.x += DX;
        e1.y += DY;
        e2.x += DX;
        e2.y += DY;

        al_draw_line(e1.x, e1.y, e2.x, e2.y, al_map_rgb(45,206, 60), 3);

        float ortX = (e1.x + e2.x) / 2;
        float ortY = (e1.y + e2.y) / 2;
        string isim = "d" + to_string(i + 1);
        al_draw_filled_rounded_rectangle(ortX - 20, ortY - 11, ortX + 20, ortY + 11,4, 4, al_map_rgb(27,94,32));
        al_draw_text(font, al_map_rgb(255,255,255),ortX, ortY - 8,ALLEGRO_ALIGN_CENTER, isim.c_str());
    }
    al_destroy_font(font);
}

void Ciz_kesisim_ve_robot(const kesisim& sonuc, float HARITA_OLCEGI, int EKRAN_X, int EKRAN_Y, int DX, int DY)
{
    ALLEGRO_FONT* font = al_load_ttf_font("fonts/DejaVuSans.ttf", 16, 0);

    nokta robot{0,0};
    nokta ekran_robot = Kordinat_Donusturucu(robot, EKRAN_X, EKRAN_Y, HARITA_OLCEGI);
    ekran_robot.x += DX;
    ekran_robot.y += DY;
    if (!sonuc.kesisti) {
        al_draw_filled_circle(ekran_robot.x, ekran_robot.y, 7, al_map_rgb(229, 0, 53));
        al_destroy_font(font);
        return;
    }

    nokta ekran_kesisim = Kordinat_Donusturucu(sonuc.kesnoktasi, EKRAN_X, EKRAN_Y, HARITA_OLCEGI);
    ekran_kesisim.x += DX;
    ekran_kesisim.y += DY;

    al_draw_line(ekran_robot.x, ekran_robot.y, ekran_kesisim.x, ekran_kesisim.y, al_map_rgb(229, 57, 53), 4);

    float ortX = (ekran_robot.x + ekran_kesisim.x) / 2;
    float ortY = (ekran_robot.y + ekran_kesisim.y) / 2;
    stringstream ss; ss.precision(2); ss << fixed << sonuc.mesafe << " m";
    string mesafe_yazi = ss.str();
    float tw = al_get_text_width(font, mesafe_yazi.c_str()) + 10;
    float th = al_get_font_line_height(font) + 4;
    al_draw_filled_rounded_rectangle(ortX - tw/2, ortY - th/2, ortX + tw/2, ortY + th/2, 4, 4, al_map_rgb(255,213,79));
    al_draw_text(font, al_map_rgb(0,0,0), ortX, ortY - th/2 + 3, ALLEGRO_ALIGN_CENTER, mesafe_yazi.c_str());

    int aci = (int)sonuc.aci;
    string aci_yazi = to_string(aci) + "° (d" + to_string(sonuc.i) + "∩d" + to_string(sonuc.j) + ")";
    al_draw_filled_rounded_rectangle(ekran_kesisim.x + 10, ekran_kesisim.y - 25, ekran_kesisim.x + 120, ekran_kesisim.y, 4, 4, al_map_rgb(255,255,255));
    al_draw_rounded_rectangle(ekran_kesisim.x + 10, ekran_kesisim.y - 25, ekran_kesisim.x + 120, ekran_kesisim.y, 4, 4, al_map_rgb(27,94,32), 2);
    al_draw_text(font, al_map_rgb(27,94,32), ekran_kesisim.x + 65, ekran_kesisim.y - 20, ALLEGRO_ALIGN_CENTER, aci_yazi.c_str());

    al_draw_filled_circle(ekran_robot.x, ekran_robot.y, 7, al_map_rgb(229, 0, 53));

    al_destroy_font(font);
}
void Ciz_grafik(int EKRAN_X,int EKRAN_Y)
{
ALLEGRO_FONT* axis_font = al_load_ttf_font("fonts/DejaVuSans.ttf", 12, ALLEGRO_TTF_NO_KERNING);

int GRAF_X = 100;   int GRAF_Y = 40;    int GRAF_SIZE = 900;    int PANEL_X1 = 1000;    int PANEL_Y1 = 60;  int PANEL_X2 = 1240;
int cx = EKRAN_X/2; int cy = EKRAN_Y/2; int gx = GRAF_X + GRAF_SIZE/2;  int gy = GRAF_Y + GRAF_SIZE/2;
int DX = (GRAF_X + GRAF_SIZE/2) - (EKRAN_X/2);  int DY = (GRAF_Y + GRAF_SIZE/2) - (EKRAN_Y/2);
int CX = GRAF_X + GRAF_SIZE/2;  int CY = GRAF_Y + GRAF_SIZE/2;  int STEP_GRID = (int)(GRAF_SIZE / 10.0f + 0.5f);

al_draw_rectangle(GRAF_X, GRAF_Y, GRAF_X+GRAF_SIZE, GRAF_Y+GRAF_SIZE, al_map_rgb(0, 0, 0), 5);

for (int i = -4; i <= 4; i++) {
    int x = CX + i * STEP_GRID;
    string sx = to_string(i);
    al_draw_text(axis_font, al_map_rgb(0,0,0), x - 6, GRAF_Y + GRAF_SIZE + 22, 0, sx.c_str());
}

for (int i = 4; i >= -4; i--) {
    int y = CY - i * STEP_GRID;
    string sy = to_string(i);
    al_draw_text(axis_font, al_map_rgb(0,0,0), GRAF_X - 28, y - 6, 0, sy.c_str());
}
int ekran_genislik = EKRAN_X;
int ekran_uzunluk = EKRAN_Y;
int bosluk = GRAF_X;

for (int i = 1; i < 10; i++) {
    float step = (float)GRAF_SIZE / 10.0f;
    float x = GRAF_X + step * i;
    float y = GRAF_Y + step * i;
    al_draw_line((int)x, GRAF_Y, (int)x, GRAF_Y+GRAF_SIZE, al_map_rgb(224,224,224), 1);
    al_draw_line(GRAF_X, (int)y, GRAF_X+GRAF_SIZE, (int)y, al_map_rgb(224,224,224), 1);
}
al_draw_line(GRAF_X, GRAF_Y + GRAF_SIZE/2, GRAF_X+GRAF_SIZE, GRAF_Y + GRAF_SIZE/2, al_map_rgb(224,224,224), 1);
al_destroy_font(axis_font);

}

void Ciz_bilgiler(const vector<dogru>& dogru_bulutu,int PANEL_X1,int PANEL_Y1,int PANEL_X2)
{
    ALLEGRO_FONT* font = al_load_ttf_font("fonts/DejaVuSans.ttf", 15, ALLEGRO_TTF_NO_KERNING);
    al_draw_rectangle(PANEL_X1+20,PANEL_Y1-20,PANEL_X2+20,(int)(PANEL_Y1 + 200 + dogru_bulutu.size()*50),al_map_rgb(0,0,0),5);
    stringstream ss;
ss.precision(2);
ss<<"Geçerli Noktalar";
string stemp=ss.str();
     al_draw_filled_circle(1040,(int)90, 7, al_map_rgb(224,224,224));
    al_draw_text(font,al_map_rgb(0,0,0),(int)1060,(int)80,0,stemp.c_str());  ss.clear(); ss.str("");
ss<<"Robot";
stemp=ss.str();
     al_draw_filled_circle(1040,(int)120, 7, al_map_rgb(229,57,53));
    al_draw_text(font,al_map_rgb(0,0,0),(int)1060,(int)110,0,stemp.c_str());  ss.clear(); ss.str("");

for(int i = 0; i < dogru_bulutu.size(); i++)
{
    nokta temp; temp.x = 0; temp.y = 0;
    float x = dogru_mesafe(temp, dogru_bulutu[i].d);

    ss << "d" << i+1 << " noktaları (" << dogru_bulutu[i].destekciler.size() << ") nokta";
    stemp = ss.str();
    al_draw_filled_circle(1038,(int)(150 + i * 50),5,al_map_rgb(45,206, 60));
    al_draw_text(font, al_map_rgb(0,0,0), (int)1060, (int)(140 + i * 50), 0, stemp.c_str());
    ss.clear(); ss.str("");

    ss << "d" << i+1 << " (" << x << "m)";
    stemp = ss.str();
    al_draw_line(1032,(int)(172 + i * 50),1047,(int)(172 + i * 50),al_map_rgb(45,206,60),4);
    al_draw_text(font, al_map_rgb(0,0,0), (int)1060, (int)(140 + i * 50 + 22), 0, stemp.c_str());
    ss.clear(); ss.str("");
}

ss << "60°+ Kesişim";
stemp = ss.str();
al_draw_line(1031, (int)(143 + dogru_bulutu.size() * 50), 1045, (int)(157 + dogru_bulutu.size() * 50), al_map_rgb(255, 213, 79), 4);
al_draw_line(1045, (int)(143 + dogru_bulutu.size() * 50), 1031, (int)(157 + dogru_bulutu.size() * 50), al_map_rgb(255, 213, 79), 4);
al_draw_text(font, al_map_rgb(0,0,0), (int)1060, (int)(140 + dogru_bulutu.size() * 50), 0, stemp.c_str());
ss.clear(); ss.str("");

ss << "Mesafe Çizgisi";
stemp = ss.str();
al_draw_line(1032, (int)(172+ dogru_bulutu.size() * 50),1047, (int)(172+ dogru_bulutu.size() * 50),al_map_rgb(229, 57, 53),4);
al_draw_text(font, al_map_rgb(0,0,0), (int)1060, (int)(140 + 22 + dogru_bulutu.size() * 50), 0, stemp.c_str());
ss.clear(); ss.str("");

}
