#include <graphics.h>
#include <conio.h>
#include <iostream>
#include <cstdio>
#include <cstring>
using namespace std;

struct Room {
    int roomNo;
    char type[20];
    bool booked;
    char guestName[50];
    char age[10];
    char phone[20];
    int days;
};

Room rooms[5];
int CORAL = 6;
int TEAL = 3;
int SUIT_COLOR = 14;

void initRooms() {
    // Initialize each room individually for C++98 compatibility
    rooms[0].roomNo = 101; strcpy(rooms[0].type, "Single"); rooms[0].booked = false; strcpy(rooms[0].guestName, ""); strcpy(rooms[0].age, ""); strcpy(rooms[0].phone, ""); rooms[0].days = 0;
    rooms[1].roomNo = 102; strcpy(rooms[1].type, "Double"); rooms[1].booked = false; strcpy(rooms[1].guestName, ""); strcpy(rooms[1].age, ""); strcpy(rooms[1].phone, ""); rooms[1].days = 0;
    rooms[2].roomNo = 103; strcpy(rooms[2].type, "Suite"); rooms[2].booked = false; strcpy(rooms[2].guestName, ""); strcpy(rooms[2].age, ""); strcpy(rooms[2].phone, ""); rooms[2].days = 0;
    rooms[3].roomNo = 104; strcpy(rooms[3].type, "Single"); rooms[3].booked = false; strcpy(rooms[3].guestName, ""); strcpy(rooms[3].age, ""); strcpy(rooms[3].phone, ""); rooms[3].days = 0;
    rooms[4].roomNo = 105; strcpy(rooms[4].type, "Deluxe"); rooms[4].booked = false; strcpy(rooms[4].guestName, ""); strcpy(rooms[4].age, ""); strcpy(rooms[4].phone, ""); rooms[4].days = 0;
}

void drawHeader(const char* title) {
    setfillstyle(SOLID_FILL, TEAL);
    bar(0, 0, getmaxx(), 60);
    setcolor(CORAL);
    settextstyle(DEFAULT_FONT, HORIZ_DIR, 2);
    int x = (getmaxx() - textwidth((char*)title)) / 2;
    outtextxy(x, 15, (char*)title);
}

void drawLabel(int x, int y, const char* text) {
    setcolor(SUIT_COLOR);
    settextstyle(DEFAULT_FONT, HORIZ_DIR, 2);
    outtextxy(x, y, (char*)text);
}

void drawInputBox(int x, int y, int w, int h) {
    setfillstyle(SOLID_FILL, CORAL);
    bar(x+1, y+1, x+w-1, y+h-1);
    setcolor(TEAL);
    rectangle(x, y, x+w, y+h);
}

void inputText(int x, int y, int w, char* buffer, int maxLen) {
    drawInputBox(x, y, w, 28);
    int i = 0;
    char ch;
    buffer[0] = '\0';
    while (true) {
        ch = getch();
        if (ch == 13) break;
        else if ((ch == 8 || ch == 127) && i > 0) {
            i--;
            buffer[i] = '\0';
            setfillstyle(SOLID_FILL, CORAL);
            bar(x+2, y+2, x+w-2, y+26);
            outtextxy(x+6, y+6, buffer);
        } else if (ch >= 32 && ch <= 126 && i < maxLen-1) {
            buffer[i++] = ch;
            buffer[i] = '\0';
            outtextxy(x+6, y+6, buffer);
        }
    }
}

void viewRoomsGraphic() {
    cleardevice();
    drawHeader("Available Rooms");
    int y = 100;
    for (int i = 0; i < 5; i++) {
        if (!rooms[i].booked) {
            char line[100];
            sprintf(line, "%d - %s", rooms[i].roomNo, rooms[i].type);
            setcolor(SUIT_COLOR);
            outtextxy(100, y, line);
            y += 40;
        }
    }
    setcolor(TEAL);
    outtextxy(100, y + 40, "Press any key to return...");
    getch();
}

void bookedRoomDetailsGraphic() {
    cleardevice();
    drawHeader("Booked Rooms");
    int y = 100;
    bool anyBooked = false;
    for (int i = 0; i < 5; i++) {
        if (rooms[i].booked) {
            char line[200];
            sprintf(line, "%d - %s - Guest: %s, Age: %s, Phone: %s, Days: %d", rooms[i].roomNo, rooms[i].type, rooms[i].guestName, rooms[i].age, rooms[i].phone, rooms[i].days);
            setcolor(SUIT_COLOR);
            outtextxy(50, y, line);
            y += 40;
            anyBooked = true;
        }
    }
    if (!anyBooked) {
        setcolor(SUIT_COLOR);
        outtextxy(100, y, "No rooms booked yet.");
    }
    setcolor(TEAL);
    outtextxy(50, y + 40, "Press any key to return...");
    getch();
}

void checkOutGraphic() {
    cleardevice();
    drawHeader("Check Out");
    char buf[10];
    drawLabel(100, 150, "Enter Room Number:");
    inputText(300, 145, 100, buf, 10);
    int r = atoi(buf);
    bool found = false;
    for (int i = 0; i < 5; i++) {
        if (rooms[i].roomNo == r && rooms[i].booked) {
            rooms[i].booked = false;
            strcpy(rooms[i].guestName, "");
            strcpy(rooms[i].age, "");
            strcpy(rooms[i].phone, "");
            rooms[i].days = 0;
            found = true;
            break;
        }
    }
    setcolor(SUIT_COLOR);
    if (found) outtextxy(100, 200, "Checkout successful!");
    else outtextxy(100, 200, "Invalid room or not booked.");
    setcolor(TEAL);
    outtextxy(100, 250, "Press any key to return...");
    getch();
}

void showBookingSummary(int idx) {
    cleardevice();
    drawHeader("Your Booking Details");
    int y = 100;
    char line[200];
    sprintf(line, "Guest Name: %s", rooms[idx].guestName); outtextxy(80, y, line); y += 30;
    sprintf(line, "Age: %s   Phone: %s", rooms[idx].age, rooms[idx].phone); outtextxy(80, y, line); y += 30;
    sprintf(line, "Room Number: %d - %s", rooms[idx].roomNo, rooms[idx].type); outtextxy(80, y, line); y += 30;
    sprintf(line, "Total Days: %d", rooms[idx].days); outtextxy(80, y, line); y += 30;
    setcolor(TEAL); outtextxy(80, y + 40, "Press any key to return to menu...");
    getch();
}

void bookRoomGraphic() {
    cleardevice();
    drawHeader("Book Room - Guest Info");

    char name[50], age[10], phone[20], days[10];
    drawLabel(80, 120, "Full Name:");
    drawLabel(80, 160, "Age:");
    drawLabel(80, 200, "Phone No:");
    drawLabel(80, 240, "Days of Stay:");

    inputText(300, 115, 250, name, 50);
    inputText(300, 155, 100, age, 10);
    inputText(300, 195, 200, phone, 20);
    inputText(300, 235, 100, days, 10);

    cleardevice(); drawHeader("Select Room Type");
    setcolor(SUIT_COLOR); settextstyle(DEFAULT_FONT, HORIZ_DIR, 2);
    outtextxy(100, 120, "1. Single Room - Rs.1000/night");
    outtextxy(100, 160, "2. Double Room - Rs.1800/night");
    outtextxy(100, 200, "3. Suite Room - Rs.3000/night");
    outtextxy(100, 260, "Enter choice (1-3):");

    char buf[10]; inputText(450, 255, 50, buf, sizeof(buf));
    int choice = atoi(buf);
    char roomType[20];
    switch (choice) {
        case 1: strcpy(roomType, "Single"); break;
        case 2: strcpy(roomType, "Double"); break;
        case 3: strcpy(roomType, "Suite"); break;
        default: strcpy(roomType, "Unknown"); break;
    }

    int bookedIndex = -1;
    for (int i = 0; i < 5; i++) {
        if (!rooms[i].booked && strcmp(rooms[i].type, roomType) == 0) {
            rooms[i].booked = true;
            strcpy(rooms[i].guestName, name);
            strcpy(rooms[i].age, age);
            strcpy(rooms[i].phone, phone);
            rooms[i].days = atoi(days);
            bookedIndex = i;
            break;
        }
    }

    if (bookedIndex != -1) {
        showBookingSummary(bookedIndex);
    }
}

int main() {
    initRooms();
    int gd = DETECT, gm;
    initgraph(&gd, &gm, "");

    while (true) {
        cleardevice(); drawHeader("Hotel Management Menu");
        setcolor(SUIT_COLOR); settextstyle(DEFAULT_FONT, HORIZ_DIR, 2);
        outtextxy(100, 100, "1. View Available Rooms");
        outtextxy(100, 150, "2. Book Room");
        outtextxy(100, 200, "3. View All Booking Details");
        outtextxy(100, 250, "4. Check Out");
        outtextxy(100, 300, "5. Exit");
        setcolor(TEAL); outtextxy(100, 350, "Enter choice (1-5):");

        char buf[10]; inputText(350, 345, 50, buf, sizeof(buf));
        int choice = atoi(buf);
        switch (choice) {
            case 1: viewRoomsGraphic(); break;
            case 2: bookRoomGraphic(); break;
            case 3: bookedRoomDetailsGraphic(); break;
            case 4: checkOutGraphic(); break;
            case 5: closegraph(); return 0;
            default: setcolor(SUIT_COLOR); outtextxy(100, 400, "Invalid choice, press any key..."); getch();
        }
    }
    closegraph();
    return 0;
}

