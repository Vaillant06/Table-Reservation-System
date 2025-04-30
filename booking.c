#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void urldecode(char *src) {
    char *dst = src;
    while (*src) {
        if (*src == '+') {
            *dst++ = ' ';
            src++;
        } else if (*src == '%' && src[1] && src[2]) {
            int ch;
            sscanf(src + 1, "%2x", &ch);
            *dst++ = ch;
            src += 3;
        } else {
            *dst++ = *src++;
        }
    }
    *dst = '\0';
}

void parse_form_data(char *data, char *name, char *phone, char *vip, char *date, char *time, char *people, char *seats) {
    sscanf(data, "name=%[^&]&phone=%[^&]&vip=%[^&]&date=%[^&]&time=%[^&]&people=%[^&]&seats=%s",
           name, phone, vip, date, time, people, seats);
    
    urldecode(name);
    urldecode(phone);
    urldecode(vip);
    urldecode(date);
    urldecode(time);
    urldecode(people);
    urldecode(seats);
}

int main() {
    printf("Content-Type: text/html\r\n\r\n");

    char *content_length = getenv("CONTENT_LENGTH");
    int len = content_length ? atoi(content_length) : 0;
    if (len <= 0) {
        printf("<h2>Error: No data received.</h2>");
        return 1;
    }

    char *form_data = malloc(len + 1);
    fread(form_data, 1, len, stdin);
    form_data[len] = '\0';

    char name[100], phone[50], vip[10], date[20], time[20], people[5], seats[500];
    parse_form_data(form_data, name, phone, vip, date, time, people, seats);

    // Save to file
    FILE *file = fopen("reservations.txt", "a");
    if (file) {
        fprintf(file, "%s|%s|%s|%s|%s|%s\n", name, phone, vip, date, time, seats);  // Seats are now comma-separated
        fclose(file);
    }
	
	// URL-encode seats (replace , with %2C)
	char encoded_seats[300] = "";
	int i, j = 0;
	for (i = 0; seats[i] != '\0'; i++) {
	    if (seats[i] == ',') {
	        encoded_seats[j++] = '%';
	        encoded_seats[j++] = '2';
	        encoded_seats[j++] = 'C';
	    } else {
	        encoded_seats[j++] = seats[i];
	    }
	}
	encoded_seats[j] = '\0';

    // Show confirmation page
    printf("<!DOCTYPE html><html lang='en'><head><meta charset='UTF-8'>");
    printf("<meta name='viewport' content='width=device-width, initial-scale=1.0'>");
    printf("<title>Booking Confirmation</title>");
    printf("<link href='https://fonts.googleapis.com/css2?family=Poppins:wght@300;500&display=swap' rel='stylesheet'>");
    printf("<style>");
    printf("body { font-family: 'Poppins', sans-serif; background-color: #fff8f0; margin: 0; padding: 0; text-align: center; }");
    printf(".card { background: white; max-width: 600px; margin: 50px auto; padding: 30px; border-radius: 12px; box-shadow: 0 0 12px rgba(0,0,0,0.2); }");
    printf("h2 { color: #8B4513; margin-bottom: 20px; }");
    printf("p { margin: 10px 0; font-size: 18px; color: #444; }");
    printf(".btn-container { margin-top: 30px; display: flex; justify-content: center; gap: 20px; flex-wrap: wrap; }");
    printf(".btn { background: #8B4513; color: white; padding: 12px 25px; border: none; border-radius: 6px; font-size: 16px; cursor: pointer; text-decoration: none; transition: 0.3s ease; }");
    printf(".btn:hover { background: lightsalmon; color: #222; }");
    printf("</style></head><body>");

    printf("<div class='card'>");
    printf("<h2>Booking Successful!</h2>");
    printf("<p><strong>Name:</strong> %s</p>", name);
    printf("<p><strong>Phone:</strong> %s</p>", phone);
    printf("<p><strong>VIP Status:</strong> %s</p>", vip);
    printf("<p><strong>Date:</strong> %s</p>", date);
    printf("<p><strong>Time:</strong> %s</p>", time);
    printf("<p><strong>Selected Seats:</strong> %s</p>", seats);
    printf("<div class='btn-container'>");
	printf("<a href='/cgi-bin/profile.exe' class='btn'>Confirm Booking</a>");
	printf("<a href='/cgi-bin/reservation.exe?seats=%s&date=%s&time=%s&people=%s' class='btn'>Update Again</a>", encoded_seats, date, time, people);
	printf("</div></div>");

    printf("</div></div>");

    printf("</body></html>");

    free(form_data);
    return 0;
}



