#include <iostream>
#include <string>
#include <curl/curl.h>
#include "json/json.h"

// OpenRouteService API anahtarı buraya yerleştirilir.
const std::string API_KEY = "BURAYA_YERLESTIR";

// Mesafeyi hesap
float calculateDistance(std::string origin, std::string destination)
{
    // HTTP isteği için URL
    std::string url = "https://api.openrouteservice.org/v2/directions/driving-car?api_key=" + API_KEY + "&start=" + origin + "&end=" + destination;

    // libcurl istek
    CURL* curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    std::string response_string;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);

    // HTTP istek
    CURLcode res = curl_easy_perform(curl);

    // Hata kontrol 
    if (res != CURLE_OK)
    {
        std::cerr << "HTTP isteği hatasi: " << curl_easy_strerror(res) << std::endl;
        return -1;
    }

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

    curl_easy_cleanup(curl);

    return distance;
}

static size_t writeCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// cppWx

int main()
{
    std::string origin, destination;
    std::cout << "Baslangic sehri: ";
    std::getline(std::cin, origin);
    std::cout << "Hedef sehri: ";
    std::getline(std::cin, destination);

    float distance = calculateDistance(origin, destination);

    if (distance >= 0)
    {
    std::cout << "Mesafe: " << distance << " km" << std::endl;
}
else
{
    std::cout << "Mesafe hesaplanamadi" << std::endl;
}

return 0;
