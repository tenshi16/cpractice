CC     = gcc
CFLAGS = -Wall -Wextra
TARGET = poker

# detect OS
UNAME := $(shell uname)

ifeq ($(UNAME), Darwin)
    # macOS via Homebrew
    CFLAGS  += -I/opt/homebrew/include
    LDFLAGS  = -L/opt/homebrew/lib -lraylib
else
    # Fedora/Linux
    LDFLAGS  = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
endif

$(TARGET): poker.c
	$(CC) $(CFLAGS) poker.c $(LDFLAGS) -o $(TARGET)

clean:
	rm -f $(TARGET)
