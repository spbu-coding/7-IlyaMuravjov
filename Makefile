CONFIG = CONFIG.cfg
include $(CONFIG)

CC = gcc
LD = gcc
CFLAGS = -Wall -Wextra -pedantic

OBJECT_DIR = $(BUILD_DIR)/objs
TARGET = $(BUILD_DIR)/$(NAME)
SOURCE_FILES = $(wildcard $(SOURCE_DIR)/*.c)
OBJECT_FILES = $(SOURCE_FILES:$(SOURCE_DIR)/%.c=$(OBJECT_DIR)/%.o)
TEST_RESULT_DIR = $(BUILD_DIR)/test
TEST_IN_FILES = $(sort $(wildcard $(TEST_DIR)/*.in))
TEST_NAMES = $(TEST_IN_FILES:$(TEST_DIR)/%.in=%)
TEST_LOG_FILES = $(TEST_NAMES:%=$(TEST_RESULT_DIR)/%.log)
TEST_ACTUAL_FILES = $(TEST_NAMES:%=$(TEST_RESULT_DIR)/%.actual)
TEST_TARGET_PREFIX = test_target_prefix
TEST_TARGETS = $(TEST_NAMES:%=$(TEST_TARGET_PREFIX)/%)

PASSED_MSG = "passed"
FAILED_MSG = "failed"

.PHONY: all check clean $(TEST_TARGETS)

all: $(TARGET)

$(OBJECT_FILES): $(OBJECT_DIR)/%.o: $(SOURCE_DIR)/%.c $(CONFIG) | $(OBJECT_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJECT_DIR)/main.o: $(SOURCE_DIR)/priority_queue.h $(SOURCE_DIR)/logging.h
$(OBJECT_DIR)/priority_queue.o: $(SOURCE_DIR)/priority_queue.h $(SOURCE_DIR)/logging.h

$(OBJECT_DIR): ; mkdir -p $@

$(TARGET): $(OBJECT_FILES) $(CONFIG) | $(BUILD_DIR)
	$(LD) $(OBJECT_FILES) -o $@

$(BUILD_DIR): ; mkdir -p $@

check: $(TEST_TARGETS)
	@tests_failed=0; \
 	tests_passed=0; \
	for test in $(TEST_NAMES); do \
	  	log="$$(cat $(TEST_RESULT_DIR)/$$test.log)"; \
	  	if [ $$log = $(FAILED_MSG) ]; then tests_failed=$$((tests_failed + 1)); fi; \
	  	if [ $$log = $(PASSED_MSG) ]; then tests_passed=$$((tests_passed + 1)); fi; \
	done; \
	echo "Tests failed: $$tests_failed, passed: $$tests_passed"; \
	exit $$tests_failed

$(TEST_TARGETS): $(TEST_TARGET_PREFIX)/%: $(TEST_RESULT_DIR)/%.log
	@echo "Test $* $$(cat $(TEST_RESULT_DIR)/$*.log)"

$(TEST_LOG_FILES): $(TEST_RESULT_DIR)/%.log: $(TEST_DIR)/%.out $(TEST_RESULT_DIR)/%.actual $(CONFIG) | $(TEST_RESULT_DIR)
	@if cmp "$(TEST_DIR)/$*.out" "$(TEST_RESULT_DIR)/$*.actual"; then \
    	echo -n $(PASSED_MSG) > "$@"; \
     else \
       	echo -n $(FAILED_MSG) > "$@"; \
     fi; \

$(TEST_ACTUAL_FILES): $(TEST_RESULT_DIR)/%.actual: $(TEST_DIR)/%.in $(TARGET) $(CONFIG) | $(TEST_RESULT_DIR)
	./$(TARGET) "$<" > "$@"

$(TEST_RESULT_DIR): ; mkdir -p $@

clean:
	rm -rf $(OBJECT_FILES) $(TARGET) $(TEST_LOG_FILES) $(TEST_ACTUAL_FILES)
