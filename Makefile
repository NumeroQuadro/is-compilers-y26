# Variables
BUILD_DIR = build
CMAKE_GENERATOR = Ninja
EXECUTABLE = $(BUILD_DIR)/bin/yal
TEST_EXECUTABLE = $(BUILD_DIR)/tests/run_tests

# Default target: build the project
all: $(EXECUTABLE)

# Build the main executable
$(EXECUTABLE): $(BUILD_DIR)/build.ninja
	cd $(BUILD_DIR) && ninja yal

# Build the test executable
tests: $(TEST_EXECUTABLE)

$(TEST_EXECUTABLE): $(BUILD_DIR)/build.ninja
	cd $(BUILD_DIR) && ninja run_tests

# Configure the project (only if necessary)
$(BUILD_DIR)/build.ninja:
	@if [ ! -d $(BUILD_DIR) ]; then mkdir $(BUILD_DIR); fi
	cd $(BUILD_DIR) && cmake -G $(CMAKE_GENERATOR) ..

# Clean only Ninja build cache (incremental build files remain intact)
clean:
	@if [ -f $(BUILD_DIR)/build.ninja ]; then \
		cd $(BUILD_DIR) && ninja -t clean; \
	else \
		echo "Nothing to clean."; \
	fi

# Run the main executable
run: $(EXECUTABLE)
	$(EXECUTABLE)

# Run the tests
test: tests
	$(TEST_EXECUTABLE)

# Force a full reconfiguration but keep already-built files
reconfigure:
	cd $(BUILD_DIR) && cmake -G $(CMAKE_GENERATOR) ..

# Rebuild everything
rebuild: clean all

.PHONY: all tests clean run test rebuild reconfigure

delete: 
	rm -rf build