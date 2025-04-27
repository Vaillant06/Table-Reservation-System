#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void url_decode(char *dst, const char *src) {
    char a, b;
    while (*src) {
        if ((*src == '%') &&
            ((a = src[1]) && (b = src[2])) &&
            (isxdigit(a) && isxdigit(b))) {
            a = (a >= 'a') ? a - 'a' + 10 : (a >= 'A') ? a - 'A' + 10 : a - '0';
            b = (b >= 'a') ? b - 'a' + 10 : (b >= 'A') ? b - 'A' + 10 : b - '0';
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

void parse_post_data(char *name, char *email, char *phone, char *message) {
    char *lenstr = getenv("CONTENT_LENGTH");
    if (!lenstr) return;

    int len = atoi(lenstr);
    char *postdata = (char *)malloc(len + 1);
    fread(postdata, 1, len, stdin);
    postdata[len] = '\0';

    char *token = strtok(postdata, "&");
    while (token) {
        if (strncmp(token, "name=", 5) == 0) url_decode(name, token + 5);
        else if (strncmp(token, "email=", 6) == 0) url_decode(email, token + 6);
        else if (strncmp(token, "phone=", 6) == 0) url_decode(phone, token + 6);
        else if (strncmp(token, "message=", 8) == 0) url_decode(message, token + 8);
        token = strtok(NULL, "&");
    }

    free(postdata);
}

int main() {
    printf("Content-Type: text/html\r\n\r\n");

    char name[100] = "", email[100] = "", phone[20] = "", message[1000] = "";
    parse_post_data(name, email, phone, message);

    if (strlen(name) == 0 || strlen(email) == 0 || strlen(phone) == 0 || strlen(message) == 0) {
        printf("<html><body style='text-align:center; font-family:sans-serif; padding:50px;'>");
        printf("<h2 style='color:red;'>All fields are required!</h2>");
        printf("<a href='/Contact Us.html' style='padding:10px 20px; background:#8B5A2B; color:white; border-radius:6px; text-decoration:none;'>Back</a>");
        printf("</body></html>");
        return 1;
    }

    // Save to messages.txt
    FILE *f = fopen("messages.txt", "a");
    if (f) {
        fprintf(f, "Name: %s\nEmail: %s\nPhone: %s\nMessage: %s\n\n", name, email, phone, message);
        fclose(f);
    }

    printf("<html><body style='text-align:center; font-family:sans-serif; padding:50px;'>");
    printf("<h2 style='color:green;'>Your message has been saved!</h2>");
    printf("<p>We’ll review it soon.</p>");
    printf("<a href='/Home.html' style='padding:10px 20px; background:#8B5A2B; color:white; border-radius:6px; text-decoration:none;'>Return to Home</a>");
    printf("</body></html>");

    return 0;
}

