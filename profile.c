#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


/* For Profile display,
 File -> Struct -> Webpage */

typedef struct Reservation {
    char username[50];
    char phone[20];
    char vip[10];
    char date[20];
    char time[20];
    char seats[300]; 
    struct Reservation* prev;
    struct Reservation* next;
} Reservation;

Reservation* head = NULL;
Reservation* tail = NULL;

void trim_newline(char *str) {
    size_t len = strlen(str);
    if (len > 0 && str[len - 1] == '\n') str[len - 1] = '\0';
}

void parse_datetime(const char *date_str, const char *time_str, struct tm *dt_tm) {
    sscanf(date_str, "%d-%d-%d", &dt_tm->tm_year, &dt_tm->tm_mon, &dt_tm->tm_mday);
    sscanf(time_str, "%d:%d", &dt_tm->tm_hour, &dt_tm->tm_min);
    dt_tm->tm_year -= 1900;
    dt_tm->tm_mon -= 1;
    dt_tm->tm_sec = 0;
    dt_tm->tm_isdst = -1;
}


int is_future_date(const char *date_str, const char *time_str) {
    time_t now = time(NULL);

    struct tm *now_tm_ptr = localtime(&now);
    struct tm now_tm = *now_tm_ptr;

    struct tm res_tm;
    parse_datetime(date_str, time_str, &res_tm);

    time_t res_time = mktime(&res_tm);
    time_t now_time = mktime(&now_tm);

    if (difftime(res_time, now_time) > 0) {
        return 1; // Future
    } else {
        return 0; // Past
    }
}


void url_decode(char *dst, const char *src) {
    char a, b;
    while (*src) {
        if ((*src == '%') &&
            ((a = src[1]) && (b = src[2])) &&
            (isxdigit(a) && isxdigit(b))) {
            if (a >= 'a') a -= 'a'-'A';
            if (a >= 'A') a -= ('A' - 10);
            else a -= '0';
            if (b >= 'a') b -= 'a'-'A';
            if (b >= 'A') b -= ('A' - 10);
            else b -= '0';
            *dst++ = 16*a+b;
            src+=3;
        } else if (*src == '+') {
            *dst++ = ' ';
            src++;
        } else {
            *dst++ = *src++;
        }
    }
    *dst++ = '\0';
}

void load_reservations(const char* username) {
    FILE* file = fopen("reservations.txt", "r");
    if (!file) return;

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        Reservation* new_node = (Reservation*)malloc(sizeof(Reservation));
        sscanf(line, "%49[^|]|%49[^|]|%19[^|]|%49[^|]|%49[^|]|%299[^\n]",
	    new_node->username, new_node->phone, new_node->vip,
	    new_node->date, new_node->time, new_node->seats);

	    if (strcmp(username, new_node->username) != 0) {
	        free(new_node);
	        continue;
	    }
	
	    new_node->prev = tail;
	    new_node->next = NULL;
	    if (tail) tail->next = new_node;
	    else head = new_node;
	    tail = new_node;
    }
    fclose(file);
}

void cancel_reservation(const char *username, const char *date, const char *time) {
    FILE *file = fopen("reservations.txt", "r");
    if (!file) {
        perror("Failed to open reservations.txt");
        return;
    }

    FILE *temp = fopen("temp.txt", "w");
    if (!temp) {
        perror("Failed to create temp.txt");
        fclose(file);
        return;
    }

    char line[512];
    while (fgets(line, sizeof(line), file)) {
        // Declare local variables inside the loop for better memory locality
        char res_username[50], phone[20], vip[10], res_date[20], res_time[20], seats[300];

        // Parse line carefully
        sscanf(line, "%49[^|]|%19[^|]|%9[^|]|%19[^|]|%19[^|]|%299[^\n]",
            res_username, phone, vip, res_date, res_time, seats);

        trim_newline(res_username);
        trim_newline(res_date);
        trim_newline(res_time);

        // Check if current line matches the reservation to cancel
        if (strcmp(username, res_username) == 0 &&
            strcmp(date, res_date) == 0 &&
            strcmp(time, res_time) == 0) {
            // Match found: skip writing this line (cancel it)
            continue;
        }

        // Write all other reservations back into temp.txt
        fputs(line, temp);
    }

    fclose(file);
    fclose(temp);

    // Remove original file
    if (remove("reservations.txt") != 0) {
        perror("Error deleting original reservations.txt");
    }

    // Rename temp.txt to reservations.txt
    if (rename("temp.txt", "reservations.txt") != 0) {
        perror("Error renaming temp.txt to reservations.txt");
    }
}


int main() {
    printf("Content-Type: text/html\r\n\r\n");
    fflush(stdout);

    char username[50] = "", email[50] = "", phone[20] = "", membership[20] = "Normal Member";
    
   char *method = getenv("REQUEST_METHOD");
if (method && strcmp(method, "POST") == 0) {
    int len = 0;
    char *content_len = getenv("CONTENT_LENGTH");
    if (content_len) len = atoi(content_len);

    char post_data[1024] = "";
    fread(post_data, 1, len, stdin);

    char date_encoded[50] = "", time_encoded[50] = "";
    sscanf(post_data, "date=%49[^&]&time=%49[^&]", date_encoded, time_encoded);

    char date[50], time[50];
    url_decode(date, date_encoded);
    url_decode(time, time_encoded);

    FILE *session = fopen("session.txt", "r");
    if (session && fgets(username, sizeof(username), session)) {
        trim_newline(username);
        fclose(session);

        cancel_reservation(username, date, time);

        printf("<script>alert('Reservation canceled successfully!'); window.location.href='/cgi-bin/Profile.exe';</script>");
        return 0;
    }
}

    FILE *session = fopen("session.txt", "r");
    if (!session || !fgets(username, sizeof(username), session)) {
        printf("<!DOCTYPE html><html lang='en'><head>");
        printf("<meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'>");
        printf("<title>Session Not Found</title>");
        printf("<link href='https://fonts.googleapis.com/css2?family=Poppins:wght@300;400;600&display=swap' rel='stylesheet'>");
        printf("<style>");
        printf("body { font-family: 'Poppins', sans-serif; background: #f9f5f0; display: flex; justify-content: center; align-items: center; height: 100vh; margin: 0; }");
        printf(".box { background-color: white; border: 2px solid #dc3545; padding: 30px 40px; border-radius: 12px; text-align: center; box-shadow: 0 0 15px rgba(0,0,0,0.1); max-width: 500px; }");
        printf(".box h2 { color: #dc3545; margin-bottom: 20px; }");
        printf(".box p { color: #555; font-size: 16px; }");
        printf(".btn { margin-top: 20px; display: inline-block; background-color: #8B5A2B; color: white; padding: 10px 20px; border-radius: 6px; text-decoration: none; transition: all 0.3s ease; }");
        printf(".btn:hover { background-color: white; color: #8B5A2B; border: 1px solid #8B5A2B; }");
        printf("</style></head><body>");
        printf("<div class='box'>");
        printf("<h2>No Active Session Found</h2>");
        printf("<p>You must be logged in to view your profile.</p>");
        printf("<a href='/Login.html' class='btn'>Back to Login</a>");
        printf("</div></body></html>");
        return 1;
    }
    fclose(session);
    trim_newline(username);

    FILE *users = fopen("users.txt", "r");
    if (!users) {
        printf("<html><body><h2>Could not open users.txt</h2></body></html>");
        return 1;
    }

    char line[256], file_username[50], file_email[50], file_phone[20], file_password[50];
	int found = 0;
	while (fgets(line, sizeof(line), users)) {
	    sscanf(line, "%49[^|]|%49[^|]|%19[^|]|%49[^|]|%*s", file_username, file_email, file_phone, file_password);
	    trim_newline(file_username);
	    trim_newline(file_password);
	
	    if (strcmp(username, file_username) == 0) {
	        strcpy(email, file_email);
	        strcpy(phone, file_phone);
	        found = 1;
	        break;
	    }
	}
    fclose(users);

    if (!found) {
    printf("<!DOCTYPE html><html lang='en'><head>");
    printf("<meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'>");
    printf("<title>User Not Found</title>");
    printf("<link href='https://fonts.googleapis.com/css2?family=Poppins:wght@300;400;600&display=swap' rel='stylesheet'>");
    printf("<style>");
    printf("body { font-family: 'Poppins', sans-serif; background: #f9f5f0; display: flex; justify-content: center; align-items: center; height: 100vh; margin: 0; }");
    printf(".box { background-color: white; border: 2px solid #dc3545; padding: 30px 40px; border-radius: 12px; text-align: center; box-shadow: 0 0 15px rgba(0,0,0,0.1); max-width: 500px; }");
    printf(".box h2 { color: #dc3545; margin-bottom: 20px; }");
    printf(".box p { color: #555; font-size: 16px; }");
    printf(".btn { margin-top: 20px; display: inline-block; background-color: #8B5A2B; color: white; padding: 10px 20px; border-radius: 6px; text-decoration: none; transition: all 0.3s ease; }");
    printf(".btn:hover { background-color: white; color: #8B5A2B; border: 1px solid #8B5A2B; }");
    printf("</style></head><body>");
    printf("<div class='box'>");
    printf("<h2>Please login to view your Profile</h2>");
    printf("<a href='/Login.html' class='btn'>Go to Login</a>");
    printf("</div></body></html>");
    return 1;
	}


    load_reservations(username);

    // HTML and Styles
    printf("<!DOCTYPE html><html lang='en'><head><meta charset='UTF-8'>");
    printf("<meta name='viewport' content='width=device-width, initial-scale=1.0'>");
    printf("<title>User Profile</title>");
    printf("<link href='https://fonts.googleapis.com/css2?family=Poppins:wght@300;400;600&display=swap' rel='stylesheet'>");
    printf("<style>");
    printf("body { font-family: 'Poppins', sans-serif; background-color: #f8f8f8; margin: 0; padding: 0; }");
    printf("header { background-color: #8B4513; padding: 20px; display: flex; justify-content: space-between; align-items: center; }");
    printf("header h1 { color: white; margin: 0 20px; font-size: 24px; }");
    printf("header nav ul { list-style: none; margin: 0; padding: 0; display: flex; gap: 20px; }");
    printf("header nav ul li a { color: white; text-decoration: none; font-weight: bold; padding: 5px 10px; border-radius: 5px; transition: background-color 0.3s ease; }");
    printf("header nav ul li a:hover { background-color: white; color: #8B4513; }");

    printf(".profile-container { display: flex; flex-wrap: wrap; justify-content: center; align-items: flex-start; padding: 40px; gap: 50px; }");
    printf(".left-column { flex: 1; max-width: 300px; text-align: center; }");
    printf(".left-column img { width: 100%%; max-width: 250px; border-radius: 50%%; box-shadow: 0 0 10px rgba(0, 0, 0, 0.2); }");
    printf(".btn-common { background-color: #8B5A2B; color: white; border: none; padding: 12px 20px; border-radius: 6px; font-size: 16px; cursor: pointer; margin: 10px 0; width: 95%%; text-decoration: none; display: inline-block; text-align: center; }");
    printf(".btn-common:hover { background-color: lightsalmon; }");
    printf(".right-column { flex: 2; max-width: 600px; background: white; padding: 30px; border-radius: 12px; box-shadow: 0 0 15px rgba(0, 0, 0, 0.1); }");
    printf(".right-column h2 { color: #8B5A2B; margin-bottom: 25px; }");
    printf(".info-group { margin-bottom: 20px; }");
    printf(".info-group label { display: block; font-weight: bold; margin-bottom: 5px; color: #555; }");
    printf(".info-group p { font-size: 16px; margin: 0; color: #333; }");
    printf(".section-title { font-size: 20px; margin-top: 30px; margin-bottom: 10px; color: #8B5A2B; font-weight: bold; }");
    printf(".reservation-history { background-color: #fff9f0; padding: 15px; border-radius: 6px; }");
    printf(".reservation-item { padding: 8px 0; border-bottom: 1px solid #eee; color: #444; }");
    printf("</style></head><body>");

    // Navigation Header
    printf("<header>");
    printf("<h1>Aswin's Food Court</h1>");
    printf("<nav><ul>");
    printf("<li><a href='/Home.html'>Home</a></li>");
    printf("<li><a href='/cgi-bin/reservation.exe'>Reserve</a></li>");
    printf("<li><a href='/cgi-bin/Profile.exe'>Profile</a></li>");
    printf("<li><a href='/Contact Us.html'>Contact</a></li>");
    printf("<li><a href='/About Us.html'>About</a></li>");
    printf("<li><a href='/Login.html'>Login</a></li>");
    printf("<li><a href='/Signup.html'>Signup</a></li>");
    printf("</ul></nav>");
    printf("</header>");

    // Profile Content
    printf("<div class='profile-container'>");
    printf("<div class='left-column'>");
    printf("<img src='/Images/login.png' alt='User Picture'>");
    printf("<a href='/edit.html' class='btn-common'>Edit Profile</a>");
    printf("<a href='/cgi-bin/logout.exe' class='btn-common'>Logout</a>");
    printf("</div>");

    printf("<div class='right-column'><h2>User Profile</h2>");
    printf("<div class='info-group'><label>Full Name</label><p>%s</p></div>", username);
    printf("<div class='info-group'><label>Email</label><p>%s</p></div>", email);
    printf("<div class='info-group'><label>Phone</label><p>%s</p></div>", phone);
    printf("<div class='info-group'><label>Membership</label><p>%s</p></div>", membership);

    printf("<div class='section-title'>Recent Reservations</div>");
    printf("<div class='reservation-history'>");
	if (!head) {
	    printf("<div class='reservation-item'>No reservations found.</div>");
	} else {
	    Reservation *curr;
	    for (curr = head; curr != NULL; curr = curr->next) {
		    int future = is_future_date(curr->date, curr->time);
		
		    printf("<div class='reservation-item' style='display: flex; justify-content: space-between; align-items: center;'>");
		
		    if (future) {
		        printf("<div>%s - %s | Seats: %s</div>", curr->date, curr->time, curr->seats);
		        printf("<form method='POST' action='/cgi-bin/Profile.exe' style='margin:0;' onsubmit=\"return confirm('Are you sure you want to cancel this reservation?');\">");
		        printf("<input type='hidden' name='date' value='%s'>", curr->date);
		        printf("<input type='hidden' name='time' value='%s'>", curr->time);
		        printf("<input type='submit' value='Cancel' style='background-color:red; color:white; border:none; padding:5px 10px; border-radius:5px; cursor:pointer;'>");
		        printf("</form>");
		    } 
			else {
		        printf("<div style='color:grey;'>%s - %s | Seats: %s (Past Reservation)</div>", curr->date, curr->time, curr->seats);
		    }
			printf("</div>");
		}
		
	}
	printf("</div>");
    printf("<a href='/cgi-bin/reservation.exe' class='btn-common'>Reserve Table</a>");
    printf("</div></div></body></html>");

    return 0;
}

