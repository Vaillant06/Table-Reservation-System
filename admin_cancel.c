#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// URL decode function
void url_decode(char *dst, const char *src) {
    char a, b;
    while (*src) {
        if ((*src == '%') &&
            ((a = src[1]) && (b = src[2])) &&
            (isxdigit(a) && isxdigit(b))) {
            if (a >= 'a') a -= 'a' - 'A';
            if (a >= 'A') a -= ('A' - 10);
            else a -= '0';
            if (b >= 'a') b -= 'a' - 'A';
            if (b >= 'A') b -= ('A' - 10);
            else b -= '0';
            *dst++ = 16 * a + b;
            src += 3;
        } else if (*src == '+') {
            *dst++ = ' ';
            src++;
        } else {
            *dst++ = *src++;
        }
    }
    *dst = '\0';
}

int main() {
    printf("Content-Type: text/html\n\n");

    char *len_str = getenv("CONTENT_LENGTH");
    int len = len_str ? atoi(len_str) : 0;
    if (len <= 0) {
        printf("<h2>No data received</h2>");
        return 1;
    }

    char *data = malloc(len + 1);
    if (!data) {
        printf("<h2>Memory allocation error</h2>");
        return 1;
    }

    fread(data, 1, len, stdin);
    data[len] = '\0';

    // Extract form fields
    char name[100], phone[50], vip[10], date[50], time[50], people[300];
    sscanf(data, "name=%99[^&]&phone=%49[^&]&vip=%9[^&]&date=%49[^&]&time=%49[^&]&seat=%299[^&]", name, phone, vip, date, time, people);

    // Decode all fields
    char d_name[100], d_phone[50], d_vip[10], d_date[50], d_time[50], d_people[300];
    url_decode(d_name, name);
    url_decode(d_phone, phone);
    url_decode(d_vip, vip);
    url_decode(d_date, date);
    url_decode(d_time, time);
    url_decode(d_people, people);

    FILE *file = fopen("reservations.txt", "r");
    FILE *temp = fopen("temp.txt", "w");
    if (!file || !temp) {
        printf("<h2>Error opening reservation files</h2>");
        return 1;
    }

    char line[512];
    int found = 0;
    while (fgets(line, sizeof(line), file)) {
        char f_name[100], f_phone[50], f_vip[10], f_date[50], f_time[50], f_people[300];

        sscanf(line, "%[^|]|%[^|]|%[^|]|%[^|]|%[^|]|%[^\n]", f_name, f_phone, f_vip, f_date, f_time, f_people);

        if (strcmp(f_name, d_name) == 0 &&
            strcmp(f_phone, d_phone) == 0 &&
            strcmp(f_vip, d_vip) == 0 &&
            strcmp(f_date, d_date) == 0 &&
            strcmp(f_time, d_time) == 0 &&
            strcmp(f_people, d_people) == 0) {
            found = 1;
            continue;  // Skip this line (cancel it)
        }

        fputs(line, temp);
    }

    fclose(file);
    fclose(temp);
    remove("reservations.txt");
    rename("temp.txt", "reservations.txt");

    // Output
    printf("<html><head><meta charset='UTF-8'>");
    printf("<link href='https://fonts.googleapis.com/css2?family=Poppins&display=swap' rel='stylesheet'>");
    printf("<style>");
    printf("body { font-family: 'Poppins', sans-serif; background-color: #fff8f0; display: flex; justify-content: center; align-items: center; height: 100vh; margin: 0; }");
    printf(".message-box { background: white; padding: 40px; border-radius: 10px; box-shadow: 0 0 15px rgba(0,0,0,0.2); text-align: center; }");
    printf(".message-box h2 { margin-bottom: 20px; color: %s; }", found ? "#28a745" : "#dc3545");
    printf(".message-box a { display: inline-block; margin-top: 20px; padding: 10px 20px; background: #8B4513; color: white; text-decoration: none; border-radius: 5px; transition: 0.3s ease; }");
    printf(".message-box a:hover { background: white; color: #8B4513; border: 2px solid #8B4513; }");
    printf("</style></head><body>");

    printf("<div class='message-box'>");
    if (found) {
        printf("<h2>Reservation cancelled successfully!</h2>");
    } else {
        printf("<h2>Reservation not found!</h2>");
    }
    printf("<a href='/cgi-bin/admin.exe'>Back to Admin Dashboard</a>");
    printf("</div>");

    printf("</body></html>");

    free(data);
    return 0;
}

