# main compiler
CC := gcc

TARGET1 := gpioRead

all: $(TARGET1)

$(TARGET1): 
	@echo "Compiling C program"
	$(CC) $(CFLAGS) $(TARGET1).c -o $(TARGET1)

clean:
	@rm -rf $(TARGET1)
