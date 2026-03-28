# Lidar Veri İşleme ve Görselleştirme (C++)

Lidar sensör verilerini işleyerek nokta bulutu oluşturan, bu veriler üzerinden doğruları tespit eden ve kesişim noktalarını analiz eden bir uygulama. Sonuçlar grafiksel olarak görselleştirilir.

---

## Özellikler

- Lidar verisini internet üzerinden indirme  
- Dosyadan veri okuma ve parsing  
- Polar koordinatları kartezyen koordinatlara dönüştürme  
- Nokta bulutu oluşturma  
- RANSAC benzeri yöntem ile doğru tespiti  
- Doğrular arası kesişim noktası hesaplama  
- Kesişim açısı ve mesafe analizi  
- Allegro ile grafiksel görselleştirme  

---

## Kullanılan Teknolojiler

- C++  
- STL (vector, string, vs.)  
- Allegro 5 (grafik çizimi)  
- Windows API (dosya indirme)  

---

## Mimari

- Veri akışı:
  - Dosya indirme → veri okuma → nokta üretimi → doğru tespiti → kesişim analizi → görselleştirme  

- Modüler yapı:
  - Veri işleme fonksiyonları  
  - Geometrik hesaplamalar  
  - Grafik çizim fonksiyonları  

---

## Öne Çıkan Kısımlar

- Lidar verisi açı ve mesafe bilgilerine göre işlenir  
- Noktalar trigonometrik hesaplarla koordinata çevrilir  
- RANSAC benzeri yaklaşım ile en uygun doğrular bulunur  
- Doğruların kesişimi açı ve mesafe kriterlerine göre hesaplanır  
- Sonuçlar Allegro ile görselleştirilir  

---

## Kazanımlar

- Geometrik algoritmalar (doğru, mesafe, kesişim)  
- Nokta bulutu işleme  
- Rastgele örnekleme (RANSAC mantığı)  
- Grafik programlama (Allegro)  
- Dosya ve veri işleme  
