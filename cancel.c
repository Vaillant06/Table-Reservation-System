#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Simple function to parse POST data
void getPostData(char *reservation_id) {
    char *lenStr = getenv("CONTENT_LENGTH");
    int len = lenStr ? atoi(lenStr) : 0;
    if (len <= 0) return;
    
    char *data = malloc(len + 1);
    fread(data, 1, len, stdin);
    data[len] = '\0';

    sscanf(data, "reservation_id=%s", reservation_id);
    free(data);
}

int main() {
    printf("Content-Type: text/html\n\n");

    char reservation_id[200] = {0};
    getPostData(reservation_id);

    FILE *fp = fopen("reservations.txt", "r");
    FILE *temp = fopen("temp.txt", "w");
    char line[500];

    if (fp && temp) {
        while (fgets(line, sizeof(line), fp)) {
            if (strstr(line, reservation_id) == NULL) {
                fputs(line, temp);
            }
        }
        fclose(fp);
        fclose(temp);
        remove("reservations.txt");
        rename("temp.txt", "reservations.txt");
    }

    // Redirect back to profile page
    printf("<script>window.location.href='/cgi-bin/profile.exe';</script>");
    return 0;
}

