#include <graphics.h>
#include <conio.h>
#include <string>
#include <vector>
#include <sstream>
#include <ctype.h> 
#include <stdlib.h> 
#include <time.h> 
#include <iomanip> // For formatting cost output

using namespace std;

// --- Data Structures and Constants ---

// Structure to hold a single booking record
struct Booking {
    string name;
    string address;
    string phone;
    string pin; // 4-digit security PIN
    int days;
    string room_type_str;
    string meal_str;
    double total_cost;
};

// Global container to store all bookings
vector<Booking> g_bookings; 

const double SINGLE_PRICE = 50.0;
const double DOUBLE_PRICE = 80.0;
const double SUITE_PRICE = 150.0;
const double MEAL_COST_PER_DAY = 20.0; // Additional flat charge for any meal plan

// --- Utility Functions ---

// Simple rectangle box drawing
void drawBox(int x, int y, int w, int h) {
    setcolor(WHITE);
    rectangle(x, y, x + w, y + h);
}

// Convert string to char* for outtextxy (standard BGI workaround)
char* toBGIChar(const string& s) {
    return const_cast<char*>(s.c_str());
}

// Function to clear a specific rectangular area by filling it with black
void clearArea(int x1, int y1, int x2, int y2) {
    setfillstyle(SOLID_FILL, BLACK);
    bar(x1, y1, x2, y2);
}

/**
 * Calculates the total cost of a booking.
 */
double calculateCost(const Booking& b) {
    double room_rate = 0.0;
    if (b.room_type_str == "Single Room") room_rate = SINGLE_PRICE;
    else if (b.room_type_str == "Double Room") room_rate = DOUBLE_PRICE;
    else if (b.room_type_str == "Suite Room") room_rate = SUITE_PRICE;

    double meal_cost = 0.0;
    // Meal plans (1 through 5) incur a cost, only 'No Meal Plan' (6) is free
    if (b.meal_str != "No Meal Plan") {
        meal_cost = MEAL_COST_PER_DAY * b.days;
    }

    // Ensure days is at least 1 for calculation
    int actual_days = (b.days > 0) ? b.days : 1;
    
    return (room_rate * actual_days) + meal_cost;
}

/**
 * A flicker-free input box with a blinking cursor.
 */
string inputBox(int x, int y, int w, int h) {
    string text = "";
    bool redraw_required = true;
    bool cursor_visible = true;
    long last_cursor_toggle = clock();
    const int CURSOR_BLINK_RATE_MS = 300; 

    clearArea(x, y, x + w, y + h);
    drawBox(x, y, w, h);
    setcolor(WHITE);

    while (1) {
        long current_time = clock();
        bool blink_time = false;

        if (current_time - last_cursor_toggle > CURSOR_BLINK_RATE_MS) {
            cursor_visible = !cursor_visible;
            last_cursor_toggle = current_time;
            blink_time = true;
        }

        if (redraw_required || blink_time) {
            clearArea(x + 1, y + 1, x + w - 1, y + h - 1); 
            setcolor(WHITE);
            outtextxy(x + 5, y + 5, toBGIChar(text));

            if (cursor_visible) {
                int cursor_x = x + 5 + textwidth(toBGIChar(text));
                int cursor_y1 = y + 5;
                int cursor_y2 = y + h - 5;
                
                setcolor(WHITE);
                line(cursor_x, cursor_y1, cursor_x, cursor_y2);
            }
            redraw_required = false; 
        }

        if (kbhit()) {
            int ch = getch();

            if (ch == 0 || ch == 224) { 
                (void)getch();
                continue;
            }

            if (ch == 13) { // ENTER
                return text;
            } else if (ch == 8) { // BACKSPACE
                if (!text.empty()) {
                    text.erase(text.length() - 1, 1);
                    redraw_required = true;
                }
            } else if (ch >= 32 && ch <= 126) { // printable characters
                // Check if text exceeds input box width 
                if (textwidth(toBGIChar(text + (char)ch)) < w - 10) { 
                    text.push_back((char)ch);
                    redraw_required = true;
                }
            }
            
            cursor_visible = true; 
            last_cursor_toggle = current_time;
        }
        delay(10); 
    }
}

// --- Screen Drawing Functions ---

void drawMainMenu() {
    cleardevice();
    
    int max_x = getmaxx();
    int max_y = getmaxy();
    int margin_x = max_x * 0.05; // 5% margin from left
    int spacing_y = max_y * 0.06; // 6% vertical spacing

    settextstyle(DEFAULT_FONT, HORIZ_DIR, 4);
    setcolor(YELLOW);
    outtextxy(margin_x, max_y * 0.08, toBGIChar("--- Hotel Management System ---"));

    settextstyle(DEFAULT_FONT, HORIZ_DIR, 2);
    setcolor(WHITE);
    outtextxy(margin_x, max_y * 0.20, toBGIChar("Press a number to select an option:"));

    setcolor(CYAN);
    int menu_start_y = max_y * 0.30;
    outtextxy(margin_x, menu_start_y + 0 * spacing_y, toBGIChar("1. View Available Rooms & Rates"));
    outtextxy(margin_x, menu_start_y + 1 * spacing_y, toBGIChar("2. Create New Booking"));
    outtextxy(margin_x, menu_start_y + 2 * spacing_y, toBGIChar("3. View Last Booking Details"));
    outtextxy(margin_x, menu_start_y + 3 * spacing_y, toBGIChar("4. Search Booking by PIN"));
    outtextxy(margin_x, menu_start_y + 4 * spacing_y, toBGIChar("5. View All Bookings")); 
    outtextxy(margin_x, menu_start_y + 5 * spacing_y, toBGIChar("6. Exit Application")); 
    
    setcolor(LIGHTGRAY);
    stringstream ss_count;
    ss_count << "Total Bookings in System: " << g_bookings.size();
    outtextxy(margin_x, max_y * 0.85, toBGIChar(ss_count.str()));
}

void drawAvailableRooms() {
    cleardevice();
    int max_x = getmaxx();
    int max_y = getmaxy();
    int margin_x = max_x * 0.05;
    int y_start = max_y * 0.20;
    int spacing_y = max_y * 0.06;

    settextstyle(DEFAULT_FONT, HORIZ_DIR, 3);
    setcolor(LIGHTGREEN);
    outtextxy(margin_x, max_y * 0.08, toBGIChar("--- Available Rooms & Rates ---"));

    settextstyle(DEFAULT_FONT, HORIZ_DIR, 2);
    setcolor(WHITE);

    outtextxy(margin_x, y_start, toBGIChar("Single Room: $50/night + $20/day for meals"));
    outtextxy(margin_x, y_start + 1 * spacing_y, toBGIChar("Double Room: $80/night + $20/day for meals"));
    outtextxy(margin_x, y_start + 2 * spacing_y, toBGIChar("Suite Room: $150/night + $20/day for meals"));
    
    setcolor(YELLOW);
    outtextxy(margin_x, max_y * 0.85, toBGIChar("Press any key to return to Main Menu..."));
    getch();
}

void displayBookingDetails(const Booking& b, const string& title) {
    cleardevice();
    int max_x = getmaxx();
    int max_y = getmaxy();
    int margin_x = max_x * 0.05;
    int spacing_y = max_y * 0.05;

    settextstyle(DEFAULT_FONT, HORIZ_DIR, 3);
    setcolor(YELLOW);
    outtextxy(margin_x, max_y * 0.08, toBGIChar(title));

    settextstyle(DEFAULT_FONT, HORIZ_DIR, 2);
    setcolor(WHITE);

    int y_pos = max_y * 0.20;
    
    outtextxy(margin_x, y_pos, toBGIChar(("Name: " + b.name).c_str())); y_pos += spacing_y;
    outtextxy(margin_x, y_pos, toBGIChar(("Address: " + b.address).c_str())); y_pos += spacing_y;
    outtextxy(margin_x, y_pos, toBGIChar(("Phone Number: " + b.phone).c_str())); y_pos += spacing_y;
    outtextxy(margin_x, y_pos, toBGIChar(("Room Type: " + b.room_type_str).c_str())); y_pos += spacing_y;
    outtextxy(margin_x, y_pos, toBGIChar(("Meal Plan: " + b.meal_str).c_str())); y_pos += spacing_y;
    outtextxy(margin_x, y_pos, toBGIChar(("Security PIN: " + b.pin).c_str())); y_pos += spacing_y;
    
    stringstream ss_days;
    ss_days << "Days Staying: " << b.days;
    outtextxy(margin_x, y_pos, toBGIChar(ss_days.str().c_str())); y_pos += spacing_y;

    // Output Total Cost with fixed precision
    stringstream ss_cost;
    ss_cost << "Total Cost: $" << fixed << setprecision(2) << b.total_cost;
    setcolor(LIGHTMAGENTA);
    outtextxy(margin_x, y_pos + spacing_y, toBGIChar(ss_cost.str().c_str()));
    
    setcolor(YELLOW);
    outtextxy(margin_x, max_y * 0.85, toBGIChar("Press any key to return to Main Menu..."));
    getch();
}

void drawRecordView() {
    if (g_bookings.empty()) {
        cleardevice();
        int max_x = getmaxx();
        int max_y = getmaxy();
        int margin_x = max_x * 0.05;

        settextstyle(DEFAULT_FONT, HORIZ_DIR, 3);
        setcolor(RED);
        outtextxy(margin_x, max_y * 0.15, toBGIChar("No booking records found in the system."));
        setcolor(YELLOW);
        outtextxy(margin_x, max_y * 0.85, toBGIChar("Press any key to return to Main Menu..."));
        getch();
        return;
    }
    
    // Display the last booking added
    const Booking& last_booking = g_bookings.back();
    displayBookingDetails(last_booking, "--- Last Booking Record ---");
}

void searchBookingByPin() {
    cleardevice();
    int max_x = getmaxx();
    int max_y = getmaxy();
    int margin_x = max_x * 0.05;

    settextstyle(DEFAULT_FONT, HORIZ_DIR, 3);
    setcolor(LIGHTBLUE);
    outtextxy(margin_x, max_y * 0.08, toBGIChar("--- Search Booking by PIN ---"));
    
    settextstyle(DEFAULT_FONT, HORIZ_DIR, 2);
    setcolor(WHITE);
    outtextxy(margin_x, max_y * 0.20, toBGIChar("Enter the 4-digit PIN to search:"));
    
    // Input box resized and positioned
    string search_pin = inputBox(margin_x, max_y * 0.25, max_x * 0.25, max_y * 0.05);
    
    bool found = false;
    for (int i = 0; i < (int)g_bookings.size(); ++i) {
        if (g_bookings[i].pin == search_pin) {
            // Found the booking!
            displayBookingDetails(g_bookings[i], "--- Found Booking Record ---");
            found = true;
            break;
        }
    }
    
    if (!found) {
        setcolor(RED);
        outtextxy(margin_x, max_y * 0.40, toBGIChar("Booking with that PIN not found."));
        setcolor(YELLOW);
        outtextxy(margin_x, max_y * 0.85, toBGIChar("Press any key to return to Main Menu..."));
        getch();
    }
}

/**
 * Iterates through all bookings and displays them in a summary list.
 */
void viewAllBookings() {
    cleardevice();
    int max_x = getmaxx();
    int max_y = getmaxy();
    int margin_x = max_x * 0.05;
    int header_y = max_y * 0.15;
    int entry_height = max_y * 0.04; // Smaller spacing for list view

    settextstyle(DEFAULT_FONT, HORIZ_DIR, 3);
    setcolor(CYAN);
    outtextxy(margin_x, max_y * 0.08, toBGIChar("--- All Booking Records ---"));

    if (g_bookings.empty()) {
        settextstyle(DEFAULT_FONT, HORIZ_DIR, 2);
        setcolor(RED);
        outtextxy(margin_x, header_y + entry_height, toBGIChar("No booking records found in the system."));
    } else {
        settextstyle(DEFAULT_FONT, HORIZ_DIR, 1);
        setcolor(LIGHTGRAY);
        
        // Header line adjusted to relative positions
        int col1_x = margin_x;
        int col2_x = max_x * 0.15; // Name starts at 15%
        int col3_x = max_x * 0.50; // PIN starts at 50%
        int col4_x = max_x * 0.65; // Cost starts at 65%

        outtextxy(col1_x, header_y, toBGIChar("No."));
        outtextxy(col2_x, header_y, toBGIChar("Name (Partial)"));
        outtextxy(col3_x, header_y, toBGIChar("PIN"));
        outtextxy(col4_x, header_y, toBGIChar("Total Cost"));
        line(margin_x, header_y + 15, max_x * 0.80, header_y + 15); // Adjusted line length

        int y_pos = header_y + 30;
        int max_entries = (max_y - y_pos - (int)(max_y * 0.15)) / entry_height; // Calculate max entries based on screen height

        for (int i = 0; i < (int)g_bookings.size(); ++i) {
            if (i >= max_entries) {
                setcolor(YELLOW);
                outtextxy(margin_x, max_y * 0.80, toBGIChar("(Displaying first few bookings. Press any key to continue...)"));
                break;
            }

            const Booking& b = g_bookings[i];
            setcolor(WHITE);

            // Entry Number
            stringstream ss_num;
            ss_num << i + 1 << ".";
            outtextxy(col1_x, y_pos, toBGIChar(ss_num.str().c_str()));

            // Name (Truncate for list view)
            string display_name = b.name;
            if (b.name.length() > 30) {
                display_name = b.name.substr(0, 27) + "...";
            }
            outtextxy(col2_x, y_pos, toBGIChar(display_name.c_str()));
            
            // PIN
            outtextxy(col3_x, y_pos, toBGIChar(b.pin.c_str()));

            // Total Cost
            stringstream ss_cost;
            ss_cost << "$" << fixed << setprecision(2) << b.total_cost;
            outtextxy(col4_x, y_pos, toBGIChar(ss_cost.str().c_str()));

            y_pos += entry_height; 
        }
    }
    
    // Final prompt
    setcolor(YELLOW);
    outtextxy(margin_x, max_y * 0.85, toBGIChar("Press any key to return to Main Menu..."));
    getch();
}

void drawBookingForm() {
    Booking new_booking;
    int max_x = getmaxx();
    int max_y = getmaxy();
    int margin_x = max_x * 0.05;
    const int INPUT_BOX_W = max_x * 0.5; // Half screen width for input
    const int INPUT_BOX_H = max_y * 0.05; // 5% screen height for input box
    const int SPACING = max_y * 0.10; // 10% vertical spacing between prompts

    
    // --- Step 1: Guest Details ---
    cleardevice();
    settextstyle(DEFAULT_FONT, HORIZ_DIR, 3);
    setcolor(LIGHTRED);
    outtextxy(margin_x, max_y * 0.05, toBGIChar("--- Room Booking Form (Step 1 of 3) ---"));

    settextstyle(DEFAULT_FONT, HORIZ_DIR, 2);
    setcolor(WHITE);
    
    // 1. Name Input 
    int current_y = max_y * 0.15;
    const int NAME_PROMPT_Y = current_y;
    const int NAME_INPUT_Y = current_y + (int)(max_y * 0.05);
    const int NAME_ERROR_Y = current_y + (int)(max_y * 0.10); 
    
    while (true) {
        // Clear previous input box and error message area
        clearArea(margin_x - 10, NAME_PROMPT_Y - 5, max_x, NAME_ERROR_Y + (int)(max_y * 0.03));
        setcolor(WHITE);
        outtextxy(margin_x, NAME_PROMPT_Y, toBGIChar("1. Enter Your Name (Alphabets only):"));
        
        new_booking.name = inputBox(margin_x, NAME_INPUT_Y, INPUT_BOX_W, INPUT_BOX_H);
        
        if (new_booking.name.empty()) {
            setcolor(RED);
            outtextxy(margin_x, NAME_ERROR_Y, toBGIChar("Name cannot be empty! Please fill the blank."));
            delay(1000); 
            continue;
        }

        bool has_non_alpha = false;
        for (int i = 0; i < (int)new_booking.name.length(); i++) {
            char c = new_booking.name[i];
            if (!isalpha(c) && c != ' ') { 
                has_non_alpha = true;
                break;
            }
        }
        
        if (has_non_alpha) {
            setcolor(RED);
            outtextxy(margin_x, NAME_ERROR_Y, toBGIChar("ERROR: Name must contain only alphabets and spaces! Please re-enter."));
            delay(1500); 
            continue; 
        }
        
        break; // Exit loop on success
    }
    
    // 2. Address Input 
    current_y += SPACING; 
    const int ADDRESS_PROMPT_Y = current_y;
    const int ADDRESS_INPUT_Y = current_y + (int)(max_y * 0.05);
    const int ADDRESS_ERROR_Y = current_y + (int)(max_y * 0.10);
    
    while (true) {
        clearArea(margin_x - 10, ADDRESS_PROMPT_Y - 5, max_x, ADDRESS_ERROR_Y + (int)(max_y * 0.03));
        setcolor(WHITE);
        outtextxy(margin_x, ADDRESS_PROMPT_Y, toBGIChar("2. Enter Your Address:"));
        
        new_booking.address = inputBox(margin_x, ADDRESS_INPUT_Y, INPUT_BOX_W, INPUT_BOX_H);
        
        if (new_booking.address.empty()) {
            setcolor(RED);
            outtextxy(margin_x, ADDRESS_ERROR_Y, toBGIChar("Address cannot be empty! Please fill the blank."));
            delay(1000); 
        } else {
            break; 
        }
    }

    // 3. Phone Input (Numeric Validation)
    current_y += SPACING;
    const int PHONE_PROMPT_Y = current_y;
    const int PHONE_INPUT_Y = current_y + (int)(max_y * 0.05);
    const int PHONE_ERROR_Y = current_y + (int)(max_y * 0.10);
    const int PHONE_BOX_W = max_x * 0.4; // Slightly smaller box for phone number
    
    while (true) {
        clearArea(margin_x - 10, PHONE_PROMPT_Y - 5, max_x, PHONE_ERROR_Y + (int)(max_y * 0.03));
        setcolor(WHITE);
        outtextxy(margin_x, PHONE_PROMPT_Y, toBGIChar("3. Enter Your Phone Number (Numbers only):"));
        
        new_booking.phone = inputBox(margin_x, PHONE_INPUT_Y, PHONE_BOX_W, INPUT_BOX_H);
        
        if (new_booking.phone.empty()) {
            setcolor(RED);
            outtextxy(margin_x, PHONE_ERROR_Y, toBGIChar("Phone number cannot be empty! Please fill the blank."));
            delay(1000); 
            continue;
        }

        bool has_non_digit = false;
        for (int i = 0; i < (int)new_booking.phone.length(); i++) {
            if (!isdigit(new_booking.phone[i])) { 
                has_non_digit = true;
                break;
            }
        }

        if (has_non_digit) {
            setcolor(RED);
            outtextxy(margin_x, PHONE_ERROR_Y, toBGIChar("ERROR: Phone number must contain only digits (0-9)! Please re-enter."));
            delay(1500); 
            continue; 
        }
        
        break; 
    }

    // 4. PIN Input (4-digit numeric validation)
    current_y += SPACING;
    const int PIN_PROMPT_Y = current_y;
    const int PIN_INPUT_Y = current_y + (int)(max_y * 0.05);
    const int PIN_ERROR_Y = current_y + (int)(max_y * 0.10); 
    const int PIN_BOX_W = max_x * 0.25; // Smallest box for PIN
    
    while (true) {
        clearArea(margin_x - 10, PIN_PROMPT_Y - 5, max_x, PIN_ERROR_Y + (int)(max_y * 0.03));
        
        settextstyle(DEFAULT_FONT, HORIZ_DIR, 2);
        setcolor(WHITE);
        outtextxy(margin_x, PIN_PROMPT_Y, toBGIChar("4. Enter a four-digit PIN code (Numbers only):"));
        
        new_booking.pin = inputBox(margin_x, PIN_INPUT_Y, PIN_BOX_W, INPUT_BOX_H);
        
        bool is_valid = false;
        if (new_booking.pin.length() == 4) {
            bool is_numeric = true;
            for (int i = 0; i < (int)new_booking.pin.length(); i++) {
                if (!isdigit(new_booking.pin[i])) {
                    is_numeric = false;
                    break;
                }
            }
            if (is_numeric) {
                is_valid = true;
            }
        }
        
        if (is_valid) {
            break; 
        }

        setcolor(RED);
        outtextxy(margin_x, PIN_ERROR_Y, toBGIChar("ERROR: PIN must be exactly four digits (0-9). Please re-enter."));
        delay(1000); 
    }
    
    // 5. Days Staying Input 
    current_y += SPACING;
    const int DAYS_PROMPT_Y = current_y;
    const int DAYS_INPUT_Y = current_y + (int)(max_y * 0.05);
    const int DAYS_ERROR_Y = current_y + (int)(max_y * 0.10);
    
    while (true) {
        clearArea(margin_x - 10, DAYS_PROMPT_Y - 5, max_x, DAYS_ERROR_Y + (int)(max_y * 0.03));
        setcolor(WHITE);
        outtextxy(margin_x, DAYS_PROMPT_Y, toBGIChar("5. Enter Number of Days staying (Numbers only, Max 365):"));
        string days_str = inputBox(margin_x, DAYS_INPUT_Y, PIN_BOX_W, INPUT_BOX_H);
        
        bool is_numeric = true;
        for (int i = 0; i < (int)days_str.length(); i++) {
            if (!isdigit(days_str[i])) {
                is_numeric = false;
                break;
            }
        }
        
        int days_int = 0;
        if (is_numeric && !days_str.empty()) {
            stringstream ss(days_str);
            ss >> days_int;
            if (days_int > 0 && days_int <= 365) { 
                new_booking.days = days_int;
                break;
            }
        }
        
        setcolor(RED);
        outtextxy(margin_x, DAYS_ERROR_Y, toBGIChar("ERROR: Days must be a number between 1 and 365. Please re-enter."));
        delay(1000); 
    }

    // Prompt for next step
    int next_step_y = max_y * 0.90;
    clearArea(margin_x, next_step_y - 10, max_x, max_y);
    setcolor(GREEN);
    outtextxy(margin_x, next_step_y, toBGIChar("Guest Details Entered. Press Enter to go to Room Selection..."));
    getch();
    
    // --- Step 2: Room Selection ---
    cleardevice();
    settextstyle(DEFAULT_FONT, HORIZ_DIR, 3);
    setcolor(LIGHTRED);
    outtextxy(margin_x, max_y * 0.05, toBGIChar("--- Room Booking Form (Step 2 of 3) ---"));

    settextstyle(DEFAULT_FONT, HORIZ_DIR, 2);
    setcolor(WHITE);
    outtextxy(margin_x, max_y * 0.15, toBGIChar("Select Room Type (Press 1, 2, or 3):"));
    
    int menu_y_start = max_y * 0.25;
    int menu_spacing = max_y * 0.06;

    setcolor(CYAN);
    outtextxy(margin_x, menu_y_start, toBGIChar("1. Single Room ($50/night)"));
    outtextxy(margin_x, menu_y_start + 1 * menu_spacing, toBGIChar("2. Double Room ($80/night)"));
    outtextxy(margin_x, menu_y_start + 2 * menu_spacing, toBGIChar("3. Suite Room ($150/night)"));

    int room_ch = 0;
    new_booking.room_type_str = "N/A";
    
    while (room_ch != '1' && room_ch != '2' && room_ch != '3') {
        if (kbhit()) {
            room_ch = getch();
        }
        delay(20);
    }
    
    if (room_ch == '1') new_booking.room_type_str = "Single Room";
    else if (room_ch == '2') new_booking.room_type_str = "Double Room";
    else if (room_ch == '3') new_booking.room_type_str = "Suite Room";

    setcolor(LIGHTGREEN);
    outtextxy(margin_x, max_y * 0.50, toBGIChar(("Selected Room Type: " + new_booking.room_type_str).c_str()));
    outtextxy(margin_x, max_y * 0.85, toBGIChar("Press Enter to go to Meal Selection..."));
    getch();

    // --- Step 3: Meal Selection ---
    cleardevice();
    settextstyle(DEFAULT_FONT, HORIZ_DIR, 3);
    setcolor(LIGHTRED);
    outtextxy(margin_x, max_y * 0.05, toBGIChar("--- Room Booking Form (Step 3 of 3) ---"));

    settextstyle(DEFAULT_FONT, HORIZ_DIR, 2);
    outtextxy(margin_x, max_y * 0.15, toBGIChar("Select Meal Plan (Press 1-6):"));

    setcolor(CYAN);
    menu_y_start = max_y * 0.25;
    outtextxy(margin_x, menu_y_start + 0 * menu_spacing, toBGIChar("1. Breakfast"));
    outtextxy(margin_x, menu_y_start + 1 * menu_spacing, toBGIChar("2. Lunch & Dinner"));
    outtextxy(margin_x, menu_y_start + 2 * menu_spacing, toBGIChar("3. Snacks & Light Meals"));
    outtextxy(margin_x, menu_y_start + 3 * menu_spacing, toBGIChar("4. Special Packages"));
    outtextxy(margin_x, menu_y_start + 4 * menu_spacing, toBGIChar("5. Room Service Access"));
    outtextxy(margin_x, menu_y_start + 5 * menu_spacing, toBGIChar("6. No Meal Plan"));

    int meal_ch = 0;
    new_booking.meal_str = "No Meal Plan";
    
    while (meal_ch < '1' || meal_ch > '6') {
        if (kbhit()) {
            meal_ch = getch();
        }
        delay(20);
    }
    
    // Assign meal string
    if (meal_ch == '1') new_booking.meal_str = "Breakfast Only";
    else if (meal_ch == '2') new_booking.meal_str = "Lunch & Dinner";
    else if (meal_ch == '3') new_booking.meal_str = "Snacks & Light Meals";
    else if (meal_ch == '4') new_booking.meal_str = "Special Packages (Full Board)";
    else if (meal_ch == '5') new_booking.meal_str = "Room Service Access";
    else if (meal_ch == '6') new_booking.meal_str = "No Meal Plan";

    setcolor(LIGHTGREEN);
    outtextxy(margin_x, max_y * 0.65, toBGIChar(("Selected Meal Plan: " + new_booking.meal_str).c_str()));
    
    // --- Finalize Booking and Calculate Cost ---
    new_booking.total_cost = calculateCost(new_booking);
    g_bookings.push_back(new_booking);

    // --- Step 4: Confirmation and Display Details ---
    cleardevice();
    settextstyle(DEFAULT_FONT, HORIZ_DIR, 4);
    setcolor(GREEN);
    outtextxy(margin_x, max_y * 0.10, toBGIChar("ROOM BOOKED SUCCESSFULLY!"));
    delay(1000); // Wait 1 second before showing details

    // Display the details of the newly created booking
    const Booking& created_booking = g_bookings.back();
    displayBookingDetails(created_booking, "--- Your Confirmed Booking Details ---");
}

int main() {
    // Initialize graphics window. We explicitly use VGA driver and VGAHI mode 
    // (640x480) for the largest *standard* BGI window size, ensuring it's 
    // initialized to the highest safe resolution.
    int gd = VGA, gm = VGAHI; 
    
    // The empty string "" tells BGI to look for the driver files (e.g., EGAVGA.BGI) 
    // in the current directory.
    initgraph(&gd, &gm, "");

    // Note: If the environment supports a larger resolution (like 800x600 VESA), 
    // you would need to use initgraph(&gd, &gm, "path") where a specific 
    // VESA driver is located, but VGAHI is the safest, highest standard mode.
    
    while (1) {
        drawMainMenu();
        
        if (kbhit()) {
            int ch = getch();
            
            if (ch == '1') {
                drawAvailableRooms();
            } else if (ch == '2') {
                drawBookingForm();
            } else if (ch == '3') {
                drawRecordView();
            } else if (ch == '4') {
                searchBookingByPin();
            } else if (ch == '5') { 
                viewAllBookings();
            } else if (ch == '6' || ch == 27) { // 6 or ESC key to exit
                break;
            }
        }
        delay(50); 
    }

    closegraph();
    return 0;
}
