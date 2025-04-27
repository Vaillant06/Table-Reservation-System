#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    printf("Content-Type: text/html\r\n\r\n");

    // Check session for logged in user
    char username[50] = "";
    FILE *session = fopen("session.txt", "r");
    if (session) {
        fgets(username, sizeof(username), session);
        fclose(session);
        // Remove newline
        size_t len = strlen(username);
        if (len > 0 && username[len - 1] == '\n') username[len - 1] = '\0';
    }

    // Begin HTML output
    printf("<!DOCTYPE html><html lang='en'><head>");
    printf("<meta charset='UTF-8'>");
    printf("<meta name='viewport' content='width=device-width, initial-scale=1.0'>");
    printf("<title>Home | Aswin's Food Court</title>");
    printf("<link href='https://fonts.googleapis.com/css2?family=Poppins:wght@300;400;600&display=swap' rel='stylesheet'>");
    printf("<style>");
    printf("body { font-family: 'Poppins', sans-serif; margin: 0; background-color: #f5f5f5; }");
    printf("header { background-color: white; display: flex; justify-content: space-between; align-items: center; padding: 15px 30px; box-shadow: 0 2px 5px rgba(0,0,0,0.1); }");
    printf(".logo { font-size: 24px; color: brown; font-weight: bold; }");
    printf("nav a { margin-left: 20px; text-decoration: none; color: #333; font-weight: 500; transition: 0.3s ease; }");
    printf("nav a:hover { color: #8B5A2B; }");
    printf(".hero { padding: 80px 20px; text-align: center; background-image: url('Images/restaurant.jpg'); background-size: cover; color: white; animation: fadeIn 1.2s ease-out; }");
    printf(".hero h1 { font-size: 48px; margin-bottom: 20px; color:#8B5A2B}");
    printf(".hero p { font-size: 20px; max-width: 600px; margin: 0 auto; color:#8B5A2B}");
    printf(".cta-buttons { margin-top: 30px; }");
    printf(".cta-buttons a { margin: 0 10px; padding: 12px 24px; background-color: #8B5A2B; color: white; text-decoration: none; border-radius: 5px; font-weight: bold; transition: 0.3s ease; }");
    printf(".cta-buttons a:hover { background-color: white; color: #8B5A2B; border: 1px solid #8B5A2B; }");
    printf("@keyframes fadeIn { from { opacity: 0; transform: translateY(20px); } to { opacity: 1; transform: translateY(0); } }");
    printf("</style></head><body>");

    // Navigation bar
    printf("<header><div class='logo'>ASWIN'S FOOD COURT</div><nav>");
    if (strlen(username) > 0) {
        printf("<a href='/cgi-bin/home.exe'>Home</a>");
    } else {
        printf("<a href='/Home.html'>Home</a>");
    }
    printf("<a href='/Reservation.html'>Reserve a Table</a>");
    printf("<a href='/Contact Us.html'>Contact Us</a>");
    printf("<a href='/About Us.html'>About Us</a>");

    if (strlen(username) > 0) {
        printf("<a href='/cgi-bin/profile.exe'>Profile</a>");
        printf("<a href='/Home.html' onclick=\"fetch('/cgi-bin/logout.exe')\">Logout</a>");
    } else {
        printf("<a href='/Login.html'>Login</a>");
        printf("<a href='/Signup.html'>Sign Up</a>");
    }
    printf("</nav></header>");

    // Hero section with animations and buttons
    printf("<section class='hero'>");
    printf("<h1>Welcome to Aswin's Food Court</h1>");
    printf("<p>Experience the best food in town with easy table reservations, VIP services, and a user-friendly platform.</p>");
    if (strlen(username) == 0) {
        printf("<div class='cta-buttons'><a href='/Signup.html'>Sign Up</a><a href='/Login.html'>Login</a></div>");
    } else {
        printf("<div class='cta-buttons'><a href='/Reservation.html'>Reserve Now</a><a href='/cgi-bin/profile.exe'>View Profile</a></div>");
    }
    printf("</section>");

    printf("</body></html>");
    return 0;
}

