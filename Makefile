CXX = em++
CXXFLAGS = -O3 --std=c++20 -s USE_SDL=2
TARGET = web/index.html
SOURCE = snake.cpp
SHELL_FILE = shell.html
STYLE = style.css

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CXX) $(CXXFLAGS) $^ -o $@ --shell-file $(SHELL_FILE)
	cp $(STYLE) web/ 

clean:
	rm -rf $(TARGET)
	rm -f web/$(STYLE)

.PHONY: all clean

# --preload-file assets --use-preload-plugins