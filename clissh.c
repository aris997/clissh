#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_FILE_TO_READ "config"
#define LINE_MAX 255

struct config_item {
	char host[32];
};

int start_with(const char *check_string, const char *search_string);
void read_config(const char *filename, struct config_item *config);

int main(int argc, char *argv[]) {
	const char *filename = DEFAULT_FILE_TO_READ;
	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-f") == 0 && i + 1 < argc) {
			filename = argv[i + 1];
			printf("Loading given filename: %s", filename);
			i++;
		}
	}
	// Parse the full list and show it to stdout
	struct config_item config[128];
	read_config(filename, config);
	// Ask for the choice
	int choice;
	printf("Choose the server to connect [close with ctrl+c]: ");
	scanf("%d", &choice);
	printf("You choosed %d %s server... Connecting soon.\n", choice, config[choice].host);
	// run the connecting command, ssh will do the rest
	char command[128];
	snprintf(command, sizeof(command), "/usr/bin/ssh %s", config[choice].host);
	system(command);
	return 0;
}

int start_with(const char *check_string, const char *search_string) {
	for (int i = 0; i < strlen(search_string); i++) {
		if (check_string[i] != search_string[i]) {
			return 1;
		}
	}
	return 0;
}

void read_config(const char *filename, struct config_item *config) {
	FILE *file = fopen(filename, "r");
	if (file == NULL) {
		printf("Error: Could not open file %s\n", filename);
		return;
	}

	char line_buf[LINE_MAX];
	int i = 0;
	const char host[6] = "Host"; // starting word for item in config
	char value[32];
	char key[8];
	while (fgets(line_buf, LINE_MAX, file) != NULL) {
		if (start_with(line_buf, host) == 0) {
			i++;
			if (sscanf(line_buf, "%7s %31s", key, value) == EOF) {
				printf("Error parsing the config.");
				exit(1);
			}
			printf("%2d %s\n", i, value);
			snprintf(config[i].host, sizeof(config[i].host), "%s", value);
		}
	}
	fclose(file);
	// return config;
}
