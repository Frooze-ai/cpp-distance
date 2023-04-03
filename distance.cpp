#include <iostream>
#include <string>
#include <curl/curl.h>
#include "json/json.h"

// OpenRouteService API anahtarını buraya yerleştirin
const std::string API_KEY = "YOUR_API_KEY";

// İki şehir arasındaki mesafeyi hesaplar
float calculateDistance(std::string origin, std::string destination)
{
    // HTTP isteği için URL oluşturma
    std::string url = "https://api.openrouteservice.org/v2/directions/driving-car?api_key=" + API_KEY + "&start=" + origin + "&end=" + destination;

    // libcurl kullanarak HTTP GET isteği gönderme
    CURL* curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    // Yanıtı almak için bir bellek alanı oluşturma
    std::string response_string;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);

    // HTTP isteği gönderme
    CURLcode res = curl_easy_perform(curl);

    // HTTP isteği hatası durumunu kontrol etme
    if (res != CURLE_OK)
    {
        std::cerr << "HTTP isteği hatasi: " << curl_easy_strerror(res) << std::endl;
        return -1;
    }

    // JSON yanıtını parse etme
    Json::Value root;
    Json::CharReaderBuilder builder;
    Json::CharReader* reader = builder.newCharReader();
    std::string errors;
    bool parsingSuccessful = reader->parse(response_string.c_str(), response_string.c_str() + response_string.size(), &root, &errors);

    // JSON parse hatası durumunu kontrol etme
    if (!parsingSuccessful)
    {
        std::cerr << "JSON parse hatasi: " << errors << std::endl;
        return -1;
    }

    // Hata durumunu kontrol etme
    if (root.isMember("error"))
    {
        std::cerr << "OpenRouteService hatasi: " << root["error"]["message"].asString() << std::endl;
        return -1;
    }

    // Mesafe bilgisini alma
    float distance = root["features"][0]["properties"]["segments"][0]["distance"] / 1000.0;

    // Temizleme
    curl_easy_cleanup(curl);

    // Sonucu döndürme
    return distance;
}

// libcurl tarafından kullanılan bir callback fonksiyonu
static size_t writeCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

int main()
{
    // Kullanıcıdan şehir adlarını alma
    std::string origin, destination;
    std::cout << "Baslangic sehri: ";
    std::getline(std::cin, origin);
    std::cout << "Hedef sehri: ";
    std::getline(std::cin, destination);

    // Mesafe hesaplama
    float distance = calculateDistance(origin, destination);

    // Sonucu ekrana yazdırma
    if (distance >= 0)
    {
    std::cout << "Mesafe: " << distance << " km" << std::endl;
}
else
{
    std::cout << "Mesafe hesaplanamadi" << std::endl;
}

return 0;