#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Remove newline characters
void trim_newline(char *str) {
    size_t len = strlen(str);
    if (len > 0 && str[len - 1] == '\n') str[len - 1] = '\0';
}

// URL decoding
char from_hex(char ch) {
    if (ch >= '0' && ch <= '9') return ch - '0';
    if (ch >= 'A' && ch <= 'F') return ch - 'A' + 10;
    if (ch >= 'a' && ch <= 'f') return ch - 'a' + 10;
    return 0;
}

void url_decode(char *dst, const char *src) {
    while (*src) {
        if (*src == '%' && *(src + 1) && *(src + 2)) {
            *dst++ = from_hex(*(src + 1)) * 16 + from_hex(*(src + 2));
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

// Print styled response page
void print_result(const char *message, bool success, const char *redirect) {
    printf("<html><head><title>Login Result</title>"
           "<meta http-equiv='refresh' content='3;url=%s'>"
           "<style>"
           "body { font-family: 'Poppins', sans-serif; background: #f0f0f0; display: flex; justify-content: center; align-items: center; height: 100vh; }"
           ".box { background: white; padding: 30px 40px; border-radius: 10px; border: 2px solid %s; box-shadow: 0 0 10px rgba(0,0,0,0.1); text-align: center; }"
           ".box h2 { color: %s; }"
           "a { text-decoration: none; background: #8B5A2B; color: white; padding: 10px 20px; border-radius: 5px; display: inline-block; margin-top: 20px; }"
           "a:hover { background: white; color: #8B5A2B; border: 1px solid #8B5A2B; }"
           "</style></head><body>", 
           redirect,
           success ? "#28a745" : "#dc3545",
           success ? "#28a745" : "#dc3545");

    printf("<div class='box'>");
    printf("<h2>%s</h2>", message);
    printf("</div></body></html>");
}

int main() {
    printf("Content-Type: text/html\r\n\r\n");

    char *query = getenv("QUERY_STRING");
    if (!query) {
        print_result("No data received", false, "/Login.html");
        return 1;
    }

    char username[50] = "", password[50] = "";
    char *token = strtok(query, "&");

    while (token) {
        if (strncmp(token, "username=", 9) == 0) strncpy(username, token + 9, sizeof(username) - 1);
        else if (strncmp(token, "password=", 9) == 0) strncpy(password, token + 9, sizeof(password) - 1);
        token = strtok(NULL, "&");
    }

    url_decode(username, username);
    url_decode(password, password);

    FILE *fp = fopen("users.txt", "r");
    if (!fp) {
        print_result("User database not found!", false, "/Login.html");
        return 1;
    }

    char line[512], file_username[50], file_email[50], file_phone[20], file_password[50], file_role[10];
    bool found = false;
    bool is_admin = false;

    while (fgets(line, sizeof(line), fp)) {
        if (line[0] == '\n' || strlen(line) <= 1) continue; // Skip empty lines
        sscanf(line, "%49[^|]|%49[^|]|%19[^|]|%49[^|]|%9[^\n]", file_username, file_email, file_phone, file_password, file_role);
        trim_newline(file_username);
        trim_newline(file_password);
        trim_newline(file_role);

        if (strcmp(username, file_username) == 0 && strcmp(password, file_password) == 0) {
            found = true;
            if (strcmp(file_role, "admin") == 0) {
                is_admin = true;
            }
            break;
        }
    }
    fclose(fp);

    if (found) {
        FILE *session = fopen("session.txt", "w");
        if (session) {
            fprintf(session, "%s\n", username);
            fclose(session);
        }
        if (is_admin) {
            print_result("Welcome Admin!", true, "/cgi-bin/admin.exe");
        } else {
            print_result("Login successful!", true, "/cgi-bin/profile.exe");
        }
    } else {
        print_result("Invalid User Credentials!", false, "/Login.html");
    }

    return 0;
}

