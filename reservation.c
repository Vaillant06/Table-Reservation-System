// reservation.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_RESERVATIONS 100
#define MAX_SEATS 32
#define MAX_NAME 50
#define MAX_PHONE 20
#define MAX_DATE 20
#define MAX_TIME 20

typedef struct {
    int seat_no;
    char date[MAX_DATE];
    char time[MAX_TIME];
} BookedSeat;

BookedSeat bookedSeats[MAX_RESERVATIONS * 4];
int bookedCount = 0;

void trim_newline(char *str) {
    size_t len = strlen(str);
    if (len > 0 && str[len - 1] == '\n') str[len - 1] = '\0';
}

void load_booked_seats() {
    FILE *file = fopen("reservations.txt", "r");
    if (!file) return;

    char line[512];
    while (fgets(line, sizeof(line), file)) {
        char username[MAX_NAME], phone[MAX_PHONE], vip[10], date[MAX_DATE], time[MAX_TIME], seats[300];
        if (sscanf(line, "%49[^|]|%19[^|]|%9[^|]|%19[^|]|%19[^|]|%299[^\n]",
                   username, phone, vip, date, time, seats) == 6) {

            // Now parse seats one by one
            char *token = strtok(seats, ",");
            while (token != NULL) {
                int seat_no = atoi(token);
                bookedSeats[bookedCount].seat_no = seat_no;
                strcpy(bookedSeats[bookedCount].date, date);
                strcpy(bookedSeats[bookedCount].time, time);
                bookedCount++;
                token = strtok(NULL, ",");
            }
        }
    }

    fclose(file);
}


void get_today_date(char *date_str) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(date_str, "%04d-%02d-%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
}

int is_seat_booked(int seat_no, const char* date, const char* time) {
	int i;
    for (i = 0; i < bookedCount; i++) {
        if (bookedSeats[i].seat_no == seat_no) {
            if (strcmp(bookedSeats[i].date, date) == 0 && strcmp(bookedSeats[i].time, time) == 0) {
                return 1;
            }
        }
    }
    return 0;
}

int main() {
    printf("Content-Type: text/html\r\n\r\n");

    char username[MAX_NAME] = "", phone[MAX_PHONE] = "";
    FILE *session = fopen("session.txt", "r");
    if (!session || !fgets(username, sizeof(username), session)) {
        printf("<html><body><h2>Session not found. Please <a href='/Login.html'>login</a>.</h2></body></html>");
        return 1;
    }
    fclose(session);
    trim_newline(username);

    FILE *users = fopen("users.txt", "r");
    if (users) {
        char line[256], file_username[MAX_NAME], file_email[50], file_phone[MAX_PHONE], password[50];
        while (fgets(line, sizeof(line), users)) {
            sscanf(line, "%49[^|]|%49[^|]|%19[^|]|%49[^]", file_username, file_email, file_phone, password);
            if (strcmp(file_username, username) == 0) {
                strcpy(phone, file_phone);
                break;
            }
        }
        fclose(users);
    }

    load_booked_seats();

    char today[11];
    get_today_date(today);

    // HTML output
    printf("<!DOCTYPE html><html lang='en'><head><meta charset='UTF-8'>");
    printf("<meta name='viewport' content='width=device-width, initial-scale=1.0'>");
    printf("<title>Reserve Table</title>");
    printf("<link href='https://fonts.googleapis.com/css2?family=Poppins:wght@300;400;600&display=swap' rel='stylesheet'>");
    printf("<style>");
    printf("body { font-family: 'Poppins', sans-serif; background: #fff8f0; margin: 0; }");
    printf("header { background-color: #8B4513; padding: 20px; display: flex; justify-content: space-between; align-items: center; }");
    printf("header h1 { color: white; margin: 0 20px; font-size: 24px; }");
    printf("header nav ul { list-style: none; display: flex; margin: 0; padding: 0; gap: 20px; }");
    printf("header nav ul li a { color: white; text-decoration: none; font-weight: bold; padding: 6px 12px; border-radius: 5px; }");
    printf("header nav ul li a:hover { background: white; color: #8B4513; }");
    printf(".container { display: flex; gap: 40px; padding: 40px; flex-wrap: wrap; }");
    printf(".tables { display: grid; grid-template-columns: repeat(2, 1fr); gap: 30px; }");
    printf(".table { display: flex; flex-wrap: wrap; background: #f0e5d8; border: 2px solid #ccc; border-radius: 10px; padding: 15px; justify-content: center; align-items: center; position: relative; }");
    printf(".table-label { position: absolute; top: -12px; background: #8B4513; color: white; font-size: 14px; padding: 3px 7px; border-radius: 6px; }");
    printf(".seat { width: 30px; height: 30px; background-color: #28a745; margin: 8px; border-radius: 50%%; cursor: pointer; }");
    printf(".seat.booked { background-color: #dc3545; cursor: not-allowed; }");
    printf(".seat.selected { outline: 3px solid #333; background-color: #dc3545; }");
    printf(".form-section { flex: 1; background: white; padding: 30px; border-radius: 12px; box-shadow: 0 0 10px rgba(0,0,0,0.2); }");
    printf(".form-section h2 { color: #8B4513; margin-bottom: 20px; text-align: center; }");
    printf(".form-group { margin-bottom: 18px; }");
    printf(".form-group label { display: block; font-weight: bold; margin-bottom: 5px; color: #444; }");
    printf(".form-group input, .form-group select { width: 100%%; padding: 10px; font-size: 16px; border: 1px solid #ccc; border-radius: 6px; }");
    printf(".submit-btn { background-color: #8B4513; color: white; border: none; padding: 12px 20px; font-size: 16px; border-radius: 6px; width: 100%%; cursor: pointer; }");
    printf(".submit-btn:hover { background-color: lightsalmon; }");
    printf("</style></head><body>");

    printf("<header><h1>Aswin's Food Court</h1><nav><ul>");
    printf("<li><a href='/Home.html'>Home</a></li>");
    printf("<li><a href='/cgi-bin/reservation.exe'>Reserve</a></li>");
    printf("<li><a href='/cgi-bin/profile.exe'>Profile</a></li>");
    printf("<li><a href='/Contact Us.html'>Contact</a></li>");
    printf("<li><a href='/About Us.html'>About</a></li>");
    printf("<li><a href='/Login.html'>Login</a></li>");
    printf("<li><a href='/Signup.html'>Signup</a></li>");
    printf("</ul></nav></header>");

    printf("<div class='container'>");

    int i, j, hour;
    printf("<div class='tables'>");
    for (i = 0; i < 8; i++) {
        printf("<div class='table'><div class='table-label'>Table %d</div>", i + 1);
        for (j = 1; j <= 4; j++) {
            int seat_id = i * 4 + j;
            int booked = is_seat_booked(seat_id, "", "");
            printf("<div class='seat%s' id='seat%d' data-id='%d'></div>",
                   booked ? " booked" : "", seat_id, seat_id);
        }
        printf("</div>");
    }
    printf("</div>");

    printf("<div class='form-section'><h2>Book Your Table</h2>");
    printf("<form method='POST' action='/cgi-bin/book.exe' onsubmit='return validateForm()'>");
    printf("<div class='form-group'><label>Full Name</label><input type='text' name='name' value='%s' readonly></div>", username);
    printf("<div class='form-group'><label>Phone Number</label><input type='text' name='phone' value='%s' readonly></div>", phone);
    printf("<div class='form-group'><label>VIP Member</label><input type='text' name='vip' value='No' readonly></div>");
    printf("<div class='form-group'><label>Date</label><input type='date' name='date' required min='%s'></div>", today);
    printf("<div class='form-group'><label>Time</label><select name='time' required>");
    for (hour = 10; hour <= 21; hour++) {
        printf("<option value='%02d:00'>%02d:00</option>", hour, hour);
    }
    printf("</select></div>");
    printf("<div class='form-group'><label>Number of People</label><input type='number' name='people' min='1' max='15' required></div>");
    printf("<input type='hidden' name='seats' id='selectedSeats'>");
    printf("<button type='submit' class='submit-btn'>Book Now</button>");
    printf("</form></div>");

    printf("</div>");

    printf("<script>");
	printf("let bookedSeats = [");
	for (i = 0; i < bookedCount; i++) {
	    printf("{ seat: %d, date: '%s', time: '%s' },", bookedSeats[i].seat_no, bookedSeats[i].date, bookedSeats[i].time);
	}
	printf("];");
	
	printf("function updateSeatAvailability() {");
	printf("let selectedDate = document.querySelector('input[name=\"date\"]').value;");
	printf("let selectedTime = document.querySelector('select[name=\"time\"]').value;");
	printf("document.querySelectorAll('.seat').forEach(seat => {");
	printf("seat.classList.remove('booked');");
	printf("let sid = parseInt(seat.dataset.id);");
	printf("if (bookedSeats.some(bs => bs.seat === sid && bs.date === selectedDate && bs.time === selectedTime)) {");
	printf("seat.classList.add('booked');");
	printf("}");
	printf("});");
	printf("document.getElementById('selectedSeats').value = '';");
	printf("document.querySelectorAll('.seat.selected').forEach(seat => seat.classList.remove('selected'));");
	printf("}");
	
	printf("document.querySelector('input[name=\"date\"]').addEventListener('change', updateSeatAvailability);");
	printf("document.querySelector('select[name=\"time\"]').addEventListener('change', updateSeatAvailability);");
	
	printf("document.querySelectorAll('.seat').forEach(seat => {");
	printf("seat.addEventListener('click', () => {");
	printf("if (seat.classList.contains('booked')) return;");
	printf("let selected = document.querySelectorAll('.seat.selected');");
	printf("let limit = parseInt(document.querySelector('input[name=\"people\"]').value);");
	printf("if (!limit || isNaN(limit)) { alert('Please select number of people first.'); return; }");
	printf("if (!seat.classList.contains('selected') && selected.length >= limit) {");
	printf("alert('You can only select ' + limit + ' seat(s).'); return;");
	printf("}");
	printf("seat.classList.toggle('selected');");
	printf("let selectedIDs = Array.from(document.querySelectorAll('.seat.selected')).map(s => s.dataset.id);");
	printf("document.getElementById('selectedSeats').value = selectedIDs.join(',');");
	printf("}); });");
	
	printf("function validateForm() {");
	printf("let selected = document.getElementById('selectedSeats').value;");
	printf("let limit = parseInt(document.querySelector('input[name=\"people\"]').value);");
	printf("let selectedCount = selected.split(',').filter(s => s).length;");
	printf("if (selectedCount !== limit) { alert('Please select exactly ' + limit + ' seat(s).'); return false; }");
	printf("return true;");
	printf("}");
	
	printf("window.addEventListener('DOMContentLoaded', () => {\n");
	printf("  updateSeatAvailability();\n");
	printf("  getURLParams();\n");
	printf("});\n");
	printf("function getURLParams() {\n");
	printf("  let params = new URLSearchParams(window.location.search);\n");
	printf("  let seats = params.get('seats');\n");
	printf("  let date = params.get('date');\n");
	printf("  let time = params.get('time');\n");
	printf("  let people = params.get('people');\n");
	printf("  if (date) document.querySelector('input[name=\"date\"]').value = date;\n");
	printf("  if (time) document.querySelector('select[name=\"time\"]').value = time;\n");
	printf("  if (people) document.querySelector('input[name=\"people\"]').value = people;\n");
	printf("  if (seats) {\n");
	printf("    let ids = seats.split(',');\n");
	printf("    ids.forEach(id => {\n");
	printf("      let seat = document.querySelector(`.seat[data-id='${id}']`);\n");
	printf("      if (seat && !seat.classList.contains('booked')) seat.classList.add('selected');\n");
	printf("    });\n");
	printf("    document.getElementById('selectedSeats').value = seats;\n");
	printf("  }\n");
	printf("}\n");

	printf("</script>");

    printf("</body></html>");
    return 0;
}

