#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINE_MAX 255

struct config_item {
	char host[32];
};

int start_with(const char *check_string, const char *search_string);
int read_config(const char *filename, struct config_item *config);

int main(int argc, char *argv[]) {
	char default_path[256];
	const char *home = getenv("HOME");
	if (home == NULL) {
		printf("Error: HOME environment variable not set\n");
		return 1;
	}
	snprintf(default_path, sizeof(default_path), "%s/.ssh/config", home);
	const char *filename = default_path;
	int edit_mode = 0;
	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-f") == 0 && i + 1 < argc) {
			filename = argv[i + 1];
			printf("Loading given filename: %s", filename);
			i++;
		} else if (strcmp(argv[i], "--edit") == 0) {
			edit_mode = 1;
		} else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
			printf("Usage: clissh [options]\n");
			printf("  -f <file>   Use a custom SSH config file (default: ~/.ssh/config)\n");
			printf("  --edit      Open the config file with $EDITOR\n");
			printf("  -h, --help  Show this help message\n");
			return 0;
		}
	}
	if (edit_mode) {
		const char *editor = getenv("EDITOR");
		if (editor == NULL) {
			editor = "vi";
		}
		char command[256];
		snprintf(command, sizeof(command), "%s %s", editor, filename);
		return system(command);
	}
	// Parse the full list and show it to stdout
	struct config_item config[128];
	int count = read_config(filename, config);
	// Ask for the choice
	char input[32];
	printf("Choose the server to connect [c to close]: ");
	if (fgets(input, sizeof(input), stdin) == NULL) {
		return 0;
	}
	// Remove trailing newline
	input[strcspn(input, "\n")] = '\0';
	// Check for cancel
	if (strcmp(input, "c") == 0) {
		return 0;
	}
	// Try to parse as a number
	char *endptr;
	long choice = strtol(input, &endptr, 10);
	if (*endptr != '\0' || choice < 1 || choice > count) {
		printf("Unrecognized host, closing\n");
		return 1;
	}
	printf("You choosed %ld %s server... Connecting soon.\n", choice, config[choice].host);
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

int read_config(const char *filename, struct config_item *config) {
	FILE *file = fopen(filename, "r");
	if (file == NULL) {
		printf("Error: Could not open file %s\n", filename);
		return 0;
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
	return i;
}
