#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    FILE *session = fopen("session.txt", "r");
    if (session) {
        char username[50];
        if (fgets(username, sizeof(username), session)) {
		    // Remove trailing newline
		    username[strcspn(username, "\r\n")] = 0;
		
		    if (strlen(username) > 0) {
		        printf("Status: 302 Found\r\n");
		        printf("Location: /cgi-bin/reservation.exe\r\n\r\n");
		        fclose(session);
		        return 0;
		    } 	 	
		}

        fclose(session);
    }

    // Not logged in
    printf("Content-Type: text/html\r\n\r\n");
    printf("<html><head><title>Login Required</title>");
    printf("<link href='https://fonts.googleapis.com/css2?family=Poppins:wght@300;400;600&display=swap' rel='stylesheet'>");
    printf("<style>");
    printf("body { font-family: 'Poppins', sans-serif; background-color: #f9f5f0; display: flex; justify-content: center; align-items: center; height: 100vh; }");
    printf(".message-box { background: white; padding: 30px; border-radius: 10px; box-shadow: 0 0 10px rgba(0,0,0,0.1); text-align: center; }");
    printf("h2 { color: #dc3545; }");
    printf("a { display: inline-block; margin-top: 15px; padding: 10px 20px; background-color: #8B5A2B; color: white; border-radius: 5px; text-decoration: none; }");
    printf("a:hover { background-color: white; color: #8B5A2B; border: 1px solid #8B5A2B; }");
    printf("</style></head><body>");
    printf("<div class='message-box'>");
    printf("<h2>Please login to access the reservation page</h2>");
    printf("<a href='/Login.html'>Go to Login</a>");
    printf("</div></body></html>");
    return 0;
}

