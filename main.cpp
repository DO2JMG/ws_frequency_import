#include <iostream>
#include <string>
#include <curl/curl.h>
#include <thread> 
#include <unistd.h>
#include <vector>
#include <cmath>
#include <sstream> 
#include <cstring>
#include <fstream>

#define ws_import_url "http://api.wettersonde.net/csv_live.php"
#define ws_import_time 15
#define ws_frequency_time 20

using std::vector;
using namespace std;

unsigned int frequencylistentry;
bool isfrequencyonlist = false;

struct struct_config {
  int ws_range;
  std::string ws_frequenyfile;
  std::string ws_squelch = "85";
  double ws_latitude;
  double ws_longitude;
};

struct_config config;

struct struct_frequencylist {
  std::string f_frequency;
  std::string f_bandwidth = "6300"; // default bandwidth
  std::string f_agc = "5"; // default agc 
};

vector<struct_frequencylist> frequencylist;

void debugmsg(std::string msg) {
  time_t     now = time(0);
  struct tm  tstruct;
  char       buf[80];
  tstruct = *localtime(&now);

  strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

  cout << buf << " - "<< msg << "\n";
}

double deg2rad(double dCoos) {
  double radians;
  radians = (dCoos * 3.14) / 180 ;
  return radians;
}

double rad2deg(double dCoos) {
  double degrees;
  degrees = (dCoos * 180) / 3.14 ;
  return degrees;
}

double getDistance(double dLatFrom, double dLonFrom, double dLatTo, double dLonTo) {
  double dtheta, dist, dmiles;
  dtheta = dLonFrom - dLonTo;
  dist = sin(deg2rad(dLatFrom)) * sin(deg2rad(dLatTo)) +  cos(deg2rad(dLatFrom)) * cos(deg2rad(dLatTo)) * cos(deg2rad(dtheta));
  dist = acos(dist);
  dist = rad2deg(dist);
  dmiles = dist * 60 * 1.1515;
  dmiles = dmiles * 1.609344;
  return dmiles;
}

vector<string> split(string str, char delimiter) { 
  vector<string> internal; 
  stringstream ss(str); 
  string tok; 
 
  while(getline(ss, tok, delimiter)) { 
    internal.push_back(tok); 
  } 
 
  return internal; 
} 

static size_t getCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

bool isonfrequenyimport(std::string strbuffer, std::string strFreq) {
  std::string strLine ("");
  vector<string> sep = split(strbuffer, '\n'); 

  for(unsigned int i = 0; i < sep.size(); ++i) {
    strLine = sep[i];
    vector<string> elements = split(strLine, ';'); 
    std::string eleFreq = elements[7];
    if (strFreq.length() > 5) {
      if (eleFreq == strFreq) {
        return true;
      }
    }
  }
  return false;
}

bool isonfrequencylist(std::string strFreq) {
  for(unsigned int i=0; i < frequencylist.size(); i++) {
    if (frequencylist[i].f_frequency == strFreq) {
      return true;
    }
  } 
  return false;
}

void setFrequencyFile() {
  while (true) { 
    if (isfrequencyonlist == true) {
      debugmsg("Write to file");

      unsigned int frequencylist_count = frequencylist.size();

      if (frequencylist_count == frequencylistentry) { frequencylistentry = 0; } // reset to 0

      debugmsg("Entrys " + std::to_string(frequencylist_count));
      debugmsg("Frequency change to " + frequencylist[frequencylistentry].f_frequency + "MHz"); // set frequency

      std::string strOutputFile = "f " + frequencylist[frequencylistentry].f_frequency;
      strOutputFile += " " + frequencylist[frequencylistentry].f_agc;
      strOutputFile += " " + config.ws_squelch;
      strOutputFile += " 0 " + frequencylist[frequencylistentry].f_bandwidth;

      frequencylistentry++;

      fstream fFileOutput;
      fFileOutput.open(config.ws_frequenyfile, ios::out);
      fFileOutput << strOutputFile;
      fFileOutput.close();
      
    }
    sleep(ws_frequency_time);
  }
}

int getFrequencyImport() {
  std::string strLine ("");

  while (true) { 
    CURL *curl;
    //CURLcode res;
    std::string readBuffer;

    curl = curl_easy_init();
    if(curl) {
      curl_easy_setopt(curl, CURLOPT_URL, ws_import_url);
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, getCallback);
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
      //res = curl_easy_perform(curl);
      curl_easy_perform(curl);
      curl_easy_cleanup(curl);

      if (readBuffer.length() > 10) {
        isfrequencyonlist = true;
      } else {
        isfrequencyonlist = false;
      }


      for(unsigned int i=0; i < frequencylist.size(); i++) {  
        if (isonfrequenyimport(readBuffer, frequencylist[i].f_frequency) == false) {
          debugmsg("Remove " + frequencylist[i].f_frequency + "MHz");
          frequencylist.erase(frequencylist.begin()+i,frequencylist.begin()+1+i); //Remove from frequencylist
        }
      } 

      vector<string> sep = split(readBuffer, '\n'); 
 
      for(unsigned int i = 0; i < sep.size(); ++i) {
        strLine = sep[i];
        vector<string> elements = split(strLine, ';'); 

        std::string strFreq = elements[7];
        std::string strSerial = elements[0];
  
        if (strFreq.length() > 5) {
          double dDistance = getDistance(std::stod(elements[1]), std::stod(elements[2]), config.ws_latitude, config.ws_longitude); // calculate distance
      
          if (dDistance < config.ws_range) { // check distance is < range
            struct_frequencylist frequencylist_temp;

            frequencylist_temp.f_frequency = strFreq;

            if (strSerial.substr(0,1) == "D") { frequencylist_temp.f_bandwidth = "12500"; } // bandwidth for DFM 
            if (strSerial.substr(0,4) == "IMET") { frequencylist_temp.f_bandwidth = "8000"; } // bandwidth for iMet 
            if (strSerial.substr(0,3) == "IMS") { frequencylist_temp.f_bandwidth = "15000"; } // bandwidth for Meisei
            if (strSerial.substr(0,2) == "ME") { frequencylist_temp.f_bandwidth = "24000"; frequencylist_temp.f_bandwidth = "8"; } // bandwidth and agc for M10/M20  

            if (isonfrequencylist(strFreq) == false) { // check frequecy is on list
              frequencylist.push_back(frequencylist_temp); //add frequency to the list
              debugmsg("Add frequency " + strFreq + "MhZ");
            }
          }
        }
      }
    }
    
    debugmsg("-- Frequencylist");
    for(unsigned int i=0; i < frequencylist.size(); i++) {  
      debugmsg(frequencylist[i].f_frequency + "MHz");
    } 
    debugmsg("--");

    sleep(ws_import_time);
  }
}

int main(int argc, char** argv) {
  std::cout << "\nFrequency import for dxlAPRS" << std::endl;
  std::cout << "Copyright (C) Jean-Michael Grobel (DO2JMG) <info@wettersonde.net>\n" << std::endl;

  for (int i = 0; i<argc; i++) {
    if (strcmp(argv[i],"-h") == 0 || strcmp(argv[i],"-help") == 0) {
        cout << "\nHelp!\n\n"; 
        cout << "-h\tThis help\n\n";
        cout << "-r\tRange in km (300)\n";
        cout << "-f\tFrequency file\n";
        cout << "-lat\tLatitude (52.1234)\n";
        cout << "-lon\tLongitude (8.1234)\n";
        cout << "-sql\tSquelch Level (Default 85)\n";
        cout << "\n";
        return 0;
    }
    if (strcmp(argv[i],"-r") == 0) {
      if(i+1 < argc) {
        config.ws_range = std::stoi(argv[i+1]);
      } else {
         debugmsg("Error : No Range");
         return 0;
      }
    } 
    if (strcmp(argv[i],"-f") == 0) {
      if(i+1 < argc) {
        config.ws_frequenyfile = argv[i+1];
      } else {
         debugmsg("Error : No frequency file!");
         return 0;
      }
    } 
    if (strcmp(argv[i],"-lat") == 0) {
      if(i+1 < argc) {
        config.ws_latitude = std::stod((argv[i+1]));
      } else {
         debugmsg("Error : No Latitude file!");
         return 0;
      }
    }
    if (strcmp(argv[i],"-lon") == 0) {
      if(i+1 < argc) {
        config.ws_longitude = std::stod((argv[i+1]));
      } else {
         debugmsg("Error : No Longitude file!");
         return 0;
      }
    }
    if (strcmp(argv[i],"-sql") == 0) {
      if(i+1 < argc) {
        config.ws_squelch = argv[i+1];
      }
    }
  }
  std::thread thread_setFrequencyFile(setFrequencyFile);  // start thread frequency file write
  std::thread thread_getFrequencyImport (getFrequencyImport);  // start thread frequency import
  thread_getFrequencyImport.join();   
  thread_setFrequencyFile.join();   
   
   return 0;
}