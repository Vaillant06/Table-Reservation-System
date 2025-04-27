#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void printHeader() {
    printf("Content-Type: text/html\n\n");
    printf("<!DOCTYPE html>");
    printf("<html lang='en'><head><meta charset='UTF-8'>");
    printf("<title>Admin Dashboard</title>");
    printf("<link href='https://fonts.googleapis.com/css2?family=Poppins&display=swap' rel='stylesheet'>");
    printf("<style>");
    printf("body, html { font-family: 'Poppins', sans-serif; background-color: white; color: #8B5A2B; text-align: center; margin: 0; padding: 0; }");
    printf(".wrapper { display: flex; flex-direction: column; min-height: 100vh; }");
    printf("header { background: #8B4513; color: white; padding: 15px 0; }");
    printf("header h1 { margin-bottom: 10px; }");
    printf("nav ul { list-style: none; padding: 0; }");
    printf("nav ul li { display: inline-block; margin: 0 10px; }");
    printf("nav ul li a { color: white; text-decoration: none; font-size: 18px; padding: 10px 15px; border-radius: 5px; transition: 0.3s ease; }");
    printf("nav ul li a:hover { background: white; color: #8B5A2B; box-shadow: 0 0 5px rgba(0,0,0,0.2); }");
    printf("nav ul li a.active { background: white; color: #8B5A2B; font-weight: bold; box-shadow: 0 0 5px rgba(0,0,0,0.2); }");
    printf(".container { flex: 1; padding: 30px; margin: 20px auto; width: 90%%; max-width: 1200px; }");
    printf("h2 { margin: 20px 0 10px; color: #8B5A2B; }");
    printf("table { width: 100%%; border-collapse: collapse; background-color: white; margin-top: 20px; box-shadow: 0 0 10px rgba(0,0,0,0.1); }");
    printf("th, td { border: 1px solid #8B5A2B; padding: 10px; text-align: center; }");
    printf("th { background-color: #8B4513; color: white; font-weight: bold; }");
    printf("tr:hover { background-color: #f2f2f2; }");
    printf(".logout-button { display: inline-block; margin-top: 40px; padding: 12px 25px; background-color: #8B4513; color: white; border: none; border-radius: 8px; font-size: 18px; text-decoration: none; transition: 0.3s ease; }");
    printf(".logout-button:hover { background-color: white; color: #8B4513; border: 2px solid #8B4513; }");
    printf("footer { background: #8B4513; color: white; padding: 10px; text-align: center; margin-top: auto; }");
    printf("</style></head><body><div class='wrapper'>");

    // Header and Nav
    printf("<header><h1>Admin Dashboard - Aswin's Food Court</h1><nav><ul>");
    printf("<li><a href='/Home.html'>Home</a></li>");
    printf("<li><a href='#' class='active'>Admin Dashboard</a></li>");
    printf("<li><a href='/Home.html'>Logout</a></li>");
    printf("</ul></nav></header>");
    printf("<div class='container'>");
}

void printFooter() {
    // Logout button at bottom
    printf("<br><a href='/Home.html' class='logout-button'>Logout</a>");
    printf("</div>");
    printf("<footer><p>&copy; 2025 Aswin's Food Court. All rights reserved.</p></footer>");
    printf("</div></body></html>");
}

void displayUsers() {
    FILE *file = fopen("users.txt", "r");
    if (!file) {
        printf("<p style='color:red;'>Error opening users.txt</p>");
        return;
    }

    printf("<h2>User Signups</h2>");
    printf("<table><tr><th>Username</th><th>Email</th><th>Phone</th></tr>");

    char line[512];
    while (fgets(line, sizeof(line), file)) {
        char username[100], email[100], phone[20], password[100];
        sscanf(line, "%[^|]|%[^|]|%[^|]|%[^|\n]", username, email, phone, password);
        printf("<tr><td>%s</td><td>%s</td><td>%s</td></tr>", username, email, phone);
    }

    printf("</table>");
    fclose(file);
}

void displayReservations() {
    FILE *file = fopen("reservations.txt", "r");
    if (!file) {
        printf("<p style='color:red;'>Error opening reservations.txt</p>");
        return;
    }

    printf("<h2>Reservations</h2>");
    printf("<table><tr><th>Name</th><th>Phone</th><th>VIP status</th><th>Date</th><th>Time</th><th>Seat Number</th></tr>");

    char line[512];
    while (fgets(line, sizeof(line), file)) {
        // Check if the line is not empty
        if (line[0] == '\n' || strlen(line) <= 1) {
            continue; // skip blank lines
        }

        char name[100], phone[50], seat[50], date[50], time[50], vip[10];
        sscanf(line, "%[^|]|%[^|]|%[^|]|%[^|]|%[^|]|%[^|\n]", name, phone, vip, date, time, seat);
        printf("<tr><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>", name, phone, vip, date, time, seat);
    }

    printf("</table>");
    fclose(file);
}


int main() {
    printHeader();
    displayUsers();
    displayReservations();
    printFooter();
    return 0;
}

