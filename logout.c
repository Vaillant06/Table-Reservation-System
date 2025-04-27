#include <stdio.h>
#include <stdlib.h>

int main() {
    // Clear session file
    remove("session.txt");

    // Set content-type
    printf("Content-Type: text/html\r\n\r\n");

    // Begin HTML response
    printf("<!DOCTYPE html><html lang='en'><head>");
    printf("<meta charset='UTF-8'>");
    printf("<meta name='viewport' content='width=device-width, initial-scale=1.0'>");
    printf("<title>Log Out</title>");
    printf("<meta http-equiv='refresh' content='3;url=/home.html'>"); // Redirect after 3 seconds
    printf("<link href='https://fonts.googleapis.com/css2?family=Poppins:wght@300;400;600&display=swap' rel='stylesheet'>");
    printf("<style>");
    printf("body { font-family: 'Poppins', sans-serif; background-color: #f9f9f9; margin: 0; display: flex; align-items: center; justify-content: center; height: 100vh; }");
    printf(".box { text-align: center; padding: 40px; background-color: white; border-radius: 10px; box-shadow: 0 4px 15px rgba(0,0,0,0.1); }");
    printf(".box h2 { color: #8B5A2B; margin-bottom: 10px; }");
    printf(".box p { color: #444; font-size: 16px; }");
    printf("</style></head><body>");

    printf("<div class='box'>");
    printf("<h2>Logged Out Successfully</h2>");
    printf("<p>You will be redirected to the Home page shortly...</p>");
    printf("</div>");

    printf("</body></html>");
    return 0;
}

