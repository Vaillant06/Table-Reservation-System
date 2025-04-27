#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

void trim_newline(char *str) {
    size_t len = strlen(str);
    if (len && str[len - 1] == '\n') str[len - 1] = '\0';
}

char from_hex(char ch) {
    if (ch >= '0' && ch <= '9') return ch - '0';
    if (ch >= 'A' && ch <= 'F') return ch - 'A' + 10;
    if (ch >= 'a' && ch <= 'f') return ch - 'a' + 10;
    return 0;
}

void url_decode(char *dst, const char *src) {
    while (*src) {
        if (*src == '%') {
            if (*(src + 1) && *(src + 2)) {
                *dst++ = from_hex(*(src + 1)) * 16 + from_hex(*(src + 2));
                src += 3;
            }
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
    printf("Content-Type: text/html\r\n\r\n");

    char current_user[50] = "";
    FILE *session = fopen("session.txt", "r");
    if (!session || !fgets(current_user, sizeof(current_user), session)) {
        printf("<h2>No session found. Please login again.</h2>");
        return 1;
    }
    fclose(session);
    trim_newline(current_user);

    char *query = getenv("QUERY_STRING");
    if (!query) {
        printf("<h2>Error: No data received.</h2>");
        return 1;
    }

    char name[50] = "", email[50] = "", phone[20] = "";
    char *token = strtok(query, "&");
    while (token) {
        if (strncmp(token, "name=", 5) == 0) url_decode(name, token + 5);
        else if (strncmp(token, "email=", 6) == 0) url_decode(email, token + 6);
        else if (strncmp(token, "phone=", 6) == 0) url_decode(phone, token + 6);
        token = strtok(NULL, "&");
    }

    if (strlen(name) == 0) {
        printf("<h2>Error: Name cannot be empty.</h2>");
        return 1;
    }

    FILE *fp = fopen("users.txt", "r");
    FILE *temp = fopen("temp.txt", "w");
    if (!fp || !temp) {
        printf("<h2>Error: Cannot access files.</h2>");
        return 1;
    }

    char line[256], file_user[50], file_email[50], file_phone[20], file_pass[50];
    bool updated = false;

    while (fgets(line, sizeof(line), fp)) {
        sscanf(line, "%49[^|]|%49[^|]|%19[^|]|%49[^\n]", file_user, file_email, file_phone, file_pass);
        if (strcmp(file_user, current_user) == 0) {
            fprintf(temp, "%s|%s|%s|%s\n", name, email, phone, file_pass);
            updated = true;
        } else {
            fputs(line, temp);
        }
    }

    fclose(fp);
    fclose(temp);

    remove("users.txt");
    rename("temp.txt", "users.txt");

    if (updated) {
        FILE *sfp = fopen("session.txt", "w");
        if (sfp) {
            fprintf(sfp, "%s\n", name);
            fclose(sfp);
        }

        printf("<!DOCTYPE html><html><head><meta charset='UTF-8'>");
        printf("<meta name='viewport' content='width=device-width, initial-scale=1.0'>");
        printf("<meta http-equiv='refresh' content='2;url=/cgi-bin/profile.exe'>");
        printf("<title>Profile Updated</title>");
        printf("<style>"
               "body { font-family: 'Poppins', sans-serif; background-color: #e6ffe6; display: flex; justify-content: center; align-items: center; height: 100vh; }"
               ".msg { text-align: center; background: white; padding: 30px; border: 2px solid #28a745; border-radius: 10px; }"
               ".msg h2 { color: #28a745; }"
               "</style></head><body>");
        printf("<div class='msg'><h2>Profile updated successfully!</h2><p>Redirecting...</p></div>");
        printf("</body></html>");
    } else {
        printf("<h2>Error: Could not update. User not found.</h2>");
    }

    return 0;
}
