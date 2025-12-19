#include <iostream>  
#include <winsock2.h> 
#include <windows.h> 
#include <string> 
#pragma comment (lib, "Ws2_32.lib")  
using namespace std;

#define SRV_PORT 1234  
#define BUF_SIZE 64
#define SRV_HOST "localhost"

struct Med {
    char name[32]; 
    int weight;
    int height;
};

int main() {
    char buff[1024];
    if (WSAStartup(0x0202, (WSADATA*)&buff[0])) {
        cout << "Error WSAStartup \n" << WSAGetLastError();
        return -1;
    }
    
    SOCKET s, s_new;
    int from_len;
    char buf[BUF_SIZE] = { 0 };
    sockaddr_in sin, from_sin;
    
    s = socket(AF_INET, SOCK_STREAM, 0);
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = 0;
    sin.sin_port = htons(SRV_PORT);
    bind(s, (sockaddr*)&sin, sizeof(sin));
    
    string msg, msg1;
    listen(s, 3);
    
    cout << "Server started. Waiting for connections..." << endl;
    
    while (1) {
        from_len = sizeof(from_sin);
        s_new = accept(s, (sockaddr*)&from_sin, &from_len);
        cout << "New connected client!" << endl;
        
        msg = "Enter command (med_req or Disconnect): ";
        
        while (1) {
            send(s_new, (char*)&msg[0], msg.size(), 0);
            
            from_len = recv(s_new, (char*)buf, BUF_SIZE, 0);
            if (from_len <= 0) break;
            
            buf[from_len] = 0;
            msg1 = (string)buf;
            cout << "Received command: " << msg1 << endl;
            
            if (msg1 == "med_req") {
                Med patient;
                int received = recv(s_new, (char*)&patient, sizeof(Med), 0);
                
                if (received == sizeof(Med)) {
                    cout << "\nPatient data received:" << endl;
                    cout << "Name: " << patient.name << endl;
                    cout << "Height: " << patient.height << " cm" << endl;
                    cout << "Weight: " << patient.weight << " kg" << endl;
                    
                    // Рассчитываем индекс массы тела
                    if (patient.height > 0) {
                        float height_m = patient.height / 100.0f;
                        int k = (int)(patient.weight / (height_m * height_m));
                        
                        cout << "Body mass index: " << k << endl;
                        
                        if (k < 19) {
                            cout << "Weight deficiency" << endl;
                        }
                        else if (k > 25) {
                            cout << "Excess weight" << endl;
                        }
                        else {
                            cout << "Normal weight" << endl;
                        }
                        
                        send(s_new, (char*)&k, sizeof(int), 0);
                    }
                    else {
                        int k = -1;
                        send(s_new, (char*)&k, sizeof(int), 0);
                    }
                }
                else {
                    cout << "Error receiving patient data" << endl;
                }
            }
            
            if (msg1 == "Disconnect") {
                cout << "Client disconnected" << endl;
                break;
            }
            
            msg = "Enter next command (med_req or Disconnect): ";
        }
        
        closesocket(s_new);
    }
    
    closesocket(s);
    WSACleanup();
    return 0;
}
