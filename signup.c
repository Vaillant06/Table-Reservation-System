#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct User {
    char username[100];
    char email[100];
    char phone[20];
    char password[100];
    char role[10];
    char vip[10];  // added VIP field
    struct User *next;
} User;

User* head = NULL;

User* createUser(char username[], char email[], char phone[], char password[], char vip[]) {
    User* newUser = (User*)malloc(sizeof(User));
    strcpy(newUser->username, username);
    strcpy(newUser->email, email);
    strcpy(newUser->phone, phone);
    strcpy(newUser->password, password);
    strcpy(newUser->role, "user");  // always signup as user
    strcpy(newUser->vip, vip);      // save vip field
    newUser->next = NULL;
    return newUser;
}

void addUser(User* user) {
    if (head == NULL) {
        head = user;
    } else {
        User* temp = head;
        while (temp->next != NULL)
            temp = temp->next;
        temp->next = user;
    }
}

void saveUserToFile(User* user) {
    FILE *file = fopen("users.txt", "a");
    if (!file) {
        printf("Content-Type: text/html\n\n");
        printf("<h1>Error opening users.txt</h1>");
        return;
    }
    fprintf(file, "%s|%s|%s|%s|%s|%s\n", user->username, user->email, user->phone, user->password, user->role, user->vip);
    fclose(file);
}

void urlDecode(char *src, char *dest) {
    char a, b;
    while (*src) {
        if ((*src == '%') && ((a = src[1]) && (b = src[2])) && (isxdigit(a) && isxdigit(b))) {
            if (a >= 'a') a -= 'a'-'A';
            if (a >= 'A') a = a - 'A' + 10; else a -= '0';
            if (b >= 'a') b -= 'a'-'A';
            if (b >= 'A') b = b - 'A' + 10; else b -= '0';
            *dest++ = 16*a+b;
            src+=3;
        } else if (*src == '+') {
            *dest++ = ' ';
            src++;
        } else {
            *dest++ = *src++;
        }
    }
    *dest = '\0';
}

void parseData(char* data, char* username, char* email, char* phone, char* vip, char* password, char* confirmPassword) {
    char tempUsername[200], tempEmail[200], tempPhone[200], tempVIP[20], tempPassword[200], tempConfirmPassword[200];

    sscanf(data, "username=%[^&]&email=%[^&]&phone=%[^&]&vip=%[^&]&password=%[^&]&confirmPassword=%s",
           tempUsername, tempEmail, tempPhone, tempVIP, tempPassword, tempConfirmPassword);

    urlDecode(tempUsername, username);
    urlDecode(tempEmail, email);
    urlDecode(tempPhone, phone);
    urlDecode(tempVIP, vip);
    urlDecode(tempPassword, password);
    urlDecode(tempConfirmPassword, confirmPassword);
}

void printSuccessPage() {
    printf("Content-Type: text/html\n\n");
    printf("<html><head><meta http-equiv='refresh' content='2;url=../Home.html'>");
    printf("<title>Signup Success</title></head><body>");
    printf("<h2 style='color:green;'>Signup Successful! Redirecting to Home...</h2>");
    printf("</body></html>");
}

void printErrorPage() {
    printf("Content-Type: text/html\n\n");
    printf("<html><head><title>Signup Error</title></head><body>");
    printf("<h2 style='color:red;'>Passwords do not match!</h2>");
    printf("<a href='../Signup.html'>Back to Signup</a>");
    printf("</body></html>");
}

int main() {
    char *data;
    char username[100], email[100], phone[20], password[100], confirmPassword[100], vip[10];

    data = getenv("QUERY_STRING");
    if (data != NULL) {
        parseData(data, username, email, phone, vip, password, confirmPassword);

        if (strcmp(password, confirmPassword) != 0) {
            printErrorPage();
            return 0;
        }

        User* newUser = createUser(username, email, phone, password, vip);
        addUser(newUser);
        saveUserToFile(newUser);

        printSuccessPage();
    } else {
        printf("Content-Type: text/html\n\n");
        printf("<h1>No form data received</h1>");
    }

    return 0;
}

