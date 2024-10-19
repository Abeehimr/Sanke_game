#include <bits/stdc++.h>
#include <cstdlib>
#include <ctime>
using namespace std;
#define R 10
#define C 10

#if __linux__ // Or #if __linux__
#define CLEAR system("clear");
#include <unistd.h>   // For read(), usleep()
#include <termios.h>  // For tcgetattr(), tcsetattr()
#include <fcntl.h>    // For F_GETFL, F_SETFL, O_NONBLOCK
#elif _WIN32
#define CLEAR system("cls");
#include <conio.h>    // For _kbhit(), _getch()
#include <windows.h>  // For Sleep() function (in milliseconds)
#endif


#ifdef __linux__
// Terminal settings for Linux
termios originalSettings;

// Function to enable non-blocking input (Linux)
void enableNonBlockingInput() {
    termios settings;
    tcgetattr(STDIN_FILENO, &originalSettings); // Save original terminal settings

    settings = originalSettings;
    settings.c_lflag &= ~ICANON; // Disable canonical mode
    settings.c_lflag &= ~ECHO;   // Disable echo
    tcsetattr(STDIN_FILENO, TCSANOW, &settings); // Apply new terminal settings

    // Set the file descriptor to non-blocking mode
    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
}

// Function to restore terminal settings (Linux)
void disableNonBlockingInput() {
    // Restore original terminal settings
    tcsetattr(STDIN_FILENO, TCSANOW, &originalSettings);
}

// Function to check if a key is pressed (Linux)
bool kbhit() {
    char ch;
    if (read(STDIN_FILENO, &ch, 1) > 0) {
        ungetc(ch, stdin);  // Put the character back into input buffer
        return true;
    }
    return false;
}
#endif

char getto() {
    #ifdef _WIN32
        return _getch();  // Windows-specific _getch

    #elif __linux__
        char ch;
        read(STDIN_FILENO, &ch, 1);  // Linux-specific read to get single character
        return ch;
    #endif
}

// Platform-independent sleep function
void sleep_ms(int milliseconds) {
    #ifdef _WIN32
        Sleep(milliseconds);  // Windows Sleep() takes milliseconds
    #elif __linux__
        usleep(milliseconds * 1000);  // Linux usleep() takes microseconds
    #endif
}

int getkey() {
    if (kbhit()) {
        return getto();  // Get the pressed key
    }
    else{
        return -1;
    }
}



int directions[] = {0,1,0,-1,0};

void display(int filled[][C],int length){
    CLEAR
    for (int r = -1; r <= R; r++){
        for (int c = -1; c <= C; c++){
            if (r == -1 || r == R || c == -1 || c == C){
                cout << "#";
            }
            else if (filled[r][c] == 1){
                cout << "@";
            }
            else if (filled[r][c] == 2){
                cout << "e";
            }
            else if (filled[r][c] == 3){
                cout << "%";
            }
            else{
                cout << " ";
            }
        }
        cout << "\n";
    }
    cout << "SCORE: " << length -1;
    cout.flush();
}

void mark_fill(int pos,int val,int filled[][C]){
    filled[pos/C][pos%C] = val;
}

int next_pos(int cur,int face){
    return ((cur/C + directions[face]+R)%R)*C + (cur%C + directions[face+1]+C)%C;
}

void reset(int&tail,int& head,bool& cherry,int& face,int& length, int filled[][C], int snake[]){
    for (int i = 0; i < R; i++){
        for (int j = 0; j < C; j++){
            filled[i][j] = 0;
            snake[i*C + j] = -1;
        }
    }
    snake[R*C] = -1;
    face = 0;
    tail=0;
    head = 0;
    length = 1;
    snake[head] = (R/2)*C + (C/2);
    filled[R/2][C/2] = 2;
    cherry = false;
}

bool mov(int&tail,int& head,bool& cherry,int& length,int& face,int filled[][C], int snake[]){
    int next = next_pos(snake[head],face);
    if (filled[next/C][next%C] == 1){
        return false;
    } 
    bool ate = filled[next/C][next%C] == 3;
    // change filled
    mark_fill(snake[head],1,filled);
    mark_fill(next,2,filled);
    mark_fill(snake[tail],0,filled);


    // change snake
    if (!ate){
        snake[tail] = -1;
        tail = (tail + 1)%(R*C+1);
    }
    else{
        cherry = false;
        length++;
    }
    head = (head + 1)%(R*C+1);
    snake[head] = next;
    return true;
}


int change_face(int face,int ch){
    if (ch == 97 || ch == 65){
        return (face +3)%4;
    }
    else if (ch == 100 || ch == 68){
        return (face+1)%4;
    }

    return face;
}


void cherry_pos(int filled[][C]){
    int r,c;
    while (true){
        r = rand()%R;
        c = rand()%C;
        if (filled[r][c] == 0){
            filled[r][c] =3;
            break;
        }
    }
}

int main(){
    #ifdef __linux__
        enableNonBlockingInput();
    #endif
    srand(static_cast<unsigned int>(time(0)));


    int filled[R][C] = {0};
    int snake[R*C+1] = {-1};
    int face,tail,head,length,ch;
    bool cherry;
    reset(tail,head,cherry,face,length,filled,snake);

    while (true){
        ch = getkey();
        if (ch == 27){break;}
        else if (ch >= 0){
            face = change_face(face,ch);
        }
        // cherry
        // find pos
        if (!cherry && length < R*C){
            cherry_pos(filled);
            cherry = true;
        }

        if (!mov(tail,head,cherry,length,face,filled,snake)){
            reset(tail,head,cherry,face,length,filled,snake);
            sleep_ms(1000);
        }
        display(filled,length);
        sleep_ms(200);
    }

    #ifdef __linux__
        disableNonBlockingInput();
    #endif
    return 0;
}