#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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
    printf("table { width: 100%%; border-collapse: collapse; background-color: white; margin-top: 30px; box-shadow: 0 0 10px rgba(0,0,0,0.1); }");
    printf("th, td { border: 1px solid #8B5A2B; padding: 10px; text-align: center; }");
    printf("th { background-color: #8B4513; color: white; font-weight: bold; }");
    printf("tr:hover { background-color: #f2f2f2; }");
    printf(".logout-button { display: inline-block; margin-top: 40px; padding: 12px 25px; background-color: #8B4513; color: white; border: none; border-radius: 8px; font-size: 18px; text-decoration: none; transition: 0.3s ease; }");
    printf(".logout-button:hover { background-color: white; color: #8B4513; border: 2px solid #8B4513; }");
    printf("footer { background: #8B4513; color: white; padding: 10px; text-align: center; margin-top: auto; }");
    printf(".reservation-item { margin-top:10px; font-size:20px; padding: 8px 0; border-bottom: 1px solid #eee; color: #444; }");
    printf("</style></head><body><div class='wrapper'>");

    printf("<header><h1>Admin Dashboard - Aswin's Food Court</h1><nav><ul>");
    printf("<li><a href='/Home.html'>Home</a></li>");
    printf("<li><a href='#' class='active'>Admin Dashboard</a></li>");
    printf("<li><a href='/cgi-bin/logout.exe'>Logout</a></li>");
    printf("</ul></nav></header>");
    printf("<div class='container'>");
}

int removedCount = 0;

void removeOldReservations() {
    FILE *file = fopen("reservations.txt", "r");
    FILE *temp = fopen("temp.txt", "w");
    if (!file || !temp) return;

    time_t t = time(NULL);
    struct tm today = *localtime(&t);

    char line[512];
    while (fgets(line, sizeof(line), file)) {
        char name[100], phone[50], vip[10], date[50], time_field[50], seats[300];
        sscanf(line, "%[^|]|%[^|]|%[^|]|%[^|]|%[^|]|%[^]\n", name, phone, vip, date, time_field, seats);

        int year, month, day;
        if (sscanf(date, "%d-%d-%d", &year, &month, &day) == 3) {
            if (year > today.tm_year + 1900 ||
                (year == today.tm_year + 1900 && month > today.tm_mon + 1) ||
                (year == today.tm_year + 1900 && month == today.tm_mon + 1 && day >= today.tm_mday)) {
                fputs(line, temp);
            } else {
                removedCount++;
            }
        }
    }

    fclose(file);
    fclose(temp);

    remove("reservations.txt");
    rename("temp.txt", "reservations.txt");
}

void printFooter() {
    printf("<br><a href='/cgi-bin/logout.exe' class='logout-button'>Logout</a>");
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
        char username[100], email[100], phone[20], password[100], role[20], vip[10];
        if (sscanf(line, "%[^|]|%[^|]|%[^|]|%[^|]|%[^|]|%[^\n]", username, email, phone, password, role, vip) == 6) {
            printf("<tr>");
            if (strcmp(vip, "Yes") == 0) {
                printf("<td>%s &#11088;</td>", username);
            } else {
                printf("<td>%s</td>", username);
            }
            printf("<td>%s</td><td>%s</td></tr>", email, phone);
        }
    }

    printf("</table>");
    fclose(file);
}

void displayReservations() {
	int hasReserve = 0;
    FILE *file = fopen("reservations.txt", "r");
    if (!file) {
        printf("<p style='color:red;'>Error opening reservations.txt</p>");
        return;
    }

    printf("<h2>Reservations</h2>");
    printf("<table><tr><th>Name</th><th>Phone</th><th>VIP status</th><th>Date</th><th>Time</th><th>Seat Number</th><th>Action</th></tr>");

    char line[512];
    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '\n' || strlen(line) <= 1) {
            continue;
        }

        char name[100], phone[50], vip[10], date[50], time_field[50], seat[300];
        sscanf(line, "%[^|]|%[^|]|%[^|]|%[^|]|%[^|]|%[^\n]", name, phone, vip, date, time_field, seat);

        printf("<tr>");
        if (strcmp(vip, "Yes") == 0) {
            printf("<td>%s &#11088;</td>", name);
        } else {
            printf("<td>%s</td>", name);
        }
        printf("<td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td>", phone, vip, date, time_field, seat);

        printf("<td>");
        printf("<form method='POST' action='/cgi-bin/admin_cancel.exe' style='margin:0;'>");
        printf("<input type='hidden' name='name' value='%s'>", name);
        printf("<input type='hidden' name='phone' value='%s'>", phone);
        printf("<input type='hidden' name='vip' value='%s'>", vip);
        printf("<input type='hidden' name='date' value='%s'>", date);
        printf("<input type='hidden' name='time' value='%s'>", time_field);
        printf("<input type='hidden' name='seat' value='%s'>", seat);
        printf("<button type='submit' style='background-color:red;color:white;padding:5px 10px;border:none;border-radius:5px;'>Cancel</button>");
        printf("</form>");
        printf("</td>");

        printf("</tr>");
        hasReserve = 1;
    }

    printf("</table>");
    
    if (!hasReserve) {
	    printf("<div class='reservation-item'>No current Reservations.</div>");
	}
	
    fclose(file);
}

void displayWaitlist() {
	int hasWaitlist = 0;
    FILE *file = fopen("waitlist.txt", "r");
    if (!file) {
        printf("<p style='color:red;'>Error opening waitlist.txt</p>");
        return;
    }

    printf("<h2>Waitlist</h2>");
    printf("<table><tr><th>Name</th><th>Phone</th><th>VIP status</th><th>Date</th><th>Time</th><th>People</th><th>Action</th></tr>");

    char line[512];
    while (fgets(line, sizeof(line), file)) {
    	if (line[0] == '\n' || strlen(line) <= 1) {
            continue;
        }
        
        char name[100], phone[50], vip[10], date[50], time_field[50], seat[300];
        sscanf(line, "%[^|]|%[^|]|%[^|]|%[^|]|%[^|]|%[^\n]", name, phone, vip, date, time_field, seat);

        printf("<tr>");
        if (strcmp(vip, "Yes") == 0) {
            printf("<td>%s &#11088;</td>", name);
        } else {
            printf("<td>%s</td>", name);
        }
        printf("<td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td>", phone, vip, date, time_field, seat);

        printf("<td>");
        printf("<form method='POST' action='/cgi-bin/admin_waitlist_cancel.exe' style='margin:0;'>");
        printf("<input type='hidden' name='name' value='%s'>", name);
        printf("<input type='hidden' name='phone' value='%s'>", phone);
        printf("<input type='hidden' name='vip' value='%s'>", vip);
        printf("<input type='hidden' name='date' value='%s'>", date);
        printf("<input type='hidden' name='time' value='%s'>", time_field);
        printf("<input type='hidden' name='seat' value='%s'>", seat);
        printf("<button type='submit' style='background-color:red;color:white;padding:5px 10px;border:none;border-radius:5px;'>Cancel</button>");
        printf("</form>");
        printf("</td>");

        printf("</tr>");
        hasWaitlist = 1;
    }
    
	printf("</div>");
	
    printf("</table>");
    
    if (!hasWaitlist) {
	    printf("<div class='reservation-item'>No current Waitlist Reservations.</div>");
	}
    fclose(file);
}

int main() {
    printHeader();
    removeOldReservations();

    if (removedCount > 0) {
        printf("<script>alert('%d old reservations automatically cleaned!');</script>", removedCount);
    }

    displayUsers();
    displayReservations();
    displayWaitlist();
    printFooter();
    return 0;
}
