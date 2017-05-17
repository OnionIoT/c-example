#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#define STRING_LENGTH 255

#define EXPORT_PATH "/sys/class/gpio/export"
#define UNEXPORT_PATH "/sys/class/gpio/unexport"
#define GPIO_PATH "/sys/class/gpio/gpio%d/%s"

#define INPUT_DIR "in"
#define EDGE_BOTH "both"
#define EDGE_RISING "rising"


#define DIRECTION_FILE "direction"
#define VALUE_FILE "value"
#define EDGE_FILE "edge"

int _writeToFile(int gpio, char* path, char* contents) {
	int status = 0;
	int ret;
	int fd;
	char text[STRING_LENGTH];


	fd = open(path, O_WRONLY);
	if (fd < 0) {
		return -1;
	}

	ret = write(fd, contents, strlen(contents));
	if (ret < 0) {
		status = -1;
	}

	close(fd);

	return status;
}

int toggleGpio(int gpio, int bOpen) {
	int status;
	char text[STRING_LENGTH];

	sprintf(text, "%d", gpio);

	status = _writeToFile(gpio, bOpen == 1 ? EXPORT_PATH : UNEXPORT_PATH, text);

	return status;
}

int setInputDir(int gpio) {
	int status;
	char path[STRING_LENGTH];

	sprintf(path, GPIO_PATH, gpio, DIRECTION_FILE);

	status = _writeToFile(gpio, path, INPUT_DIR);

	return status;
}

int readInput(int gpio, int *value) {
	int status;
	int fd;
	char path[STRING_LENGTH];
	char buf;

	sprintf(path, GPIO_PATH, gpio, VALUE_FILE);

	fd = open(path, O_RDONLY);
	if (fd < 0) {
		return -1;
	}
	
	status = read(fd, &buf, 1);
	if (status < 0) {
		return -1;
	}

	if (buf != '0') {
		*value = 1;
	}
	else {
		*value = 0;
	}

	close(fd);
	return 0;
}


int main(int argc, char* argv[])
{
	int i;
	int status; 
	int gpio;
	int value;

	if (argc < 2) {
		printf("Usage: gpioRead <gpio>\n\n");
		printf("Reads the input value of the specified GPIO pin once a second for 20 seconds\n");
		exit(-1);
	}

	
	gpio=atoi(argv[1]);

	// export the gpio
	printf("> exporting gpio\n");
	status = toggleGpio(gpio, 1);
	printf("     status: %d\n", status);


	// set the direction to input
	printf("> setting to input\n");
	status |= setInputDir(gpio);
	printf("     status: %d\n", status);

	// read the gpio 20 times
	for (i = 0; i < 20; i++) 
	{ 
		// read the gpio
		status = readInput(gpio, &value);

		if (status < 0) {
			printf("ERROR: reading GPIO%d input\n", gpio);
		}
		else {
			printf("  > Read GPIO%d: value '%d'\n", gpio, value);
		}

		// pause between each read
		sleep(1);
	}

	// unexport the gpio
	printf("> unexporting gpio\n");
	status |= toggleGpio(gpio, 0);
	printf("     status: %d\n", status);
	
	return 0;
}
