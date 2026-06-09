# ─────────────────────────────────────────────────────────────────────────────
# Katlans Language  —  Makefile
# ─────────────────────────────────────────────────────────────────────────────

PYTHON := python3
KATLANS := $(PYTHON) files/katlans.py
CC     := gcc
CFLAGS := -lm -O2 -Wall -Wextra -Wno-unused-parameter -Wno-unused-function

# Where .kl source files live
KL_DIR := files
BUILD_DIR := build

# All .kl test files
KL_TESTS := $(wildcard $(KL_DIR)/test_phase*.kl) \
            $(KL_DIR)/hello.kl $(KL_DIR)/control.kl \
            $(KL_DIR)/functions.kl $(KL_DIR)/test_all.kl

# Generated .c files go in build/
C_TARGETS := $(patsubst $(KL_DIR)/%.kl,$(BUILD_DIR)/%.c,$(KL_TESTS))

# Compiled binaries (if CC is available)
BINARIES := $(patsubst $(KL_DIR)/%.kl,$(BUILD_DIR)/%.exe,$(KL_TESTS))

.PHONY: all clean test emit compile check-cc

all: check-cc emit compile

# ── Check for C compiler ────────────────────────────────────────────────────
check-cc:
	@which $(CC) > /dev/null 2>&1 && echo "  $(CC) available" || echo "  $(CC) NOT available — install MinGW/gcc"

# ── Create build directory ──────────────────────────────────────────────────
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# ── Emit C code from .kl files ──────────────────────────────────────────────
emit: $(BUILD_DIR) $(C_TARGETS)

$(BUILD_DIR)/%.c: $(KL_DIR)/%.kl $(KL_DIR)/katlans.py
	@echo "  Emit  $* ..."
	@$(KATLANS) emit "$<" > "$@" 2>&1 || { echo "  FAILED $*"; exit 1; }

# ── Compile .c → binary ────────────────────────────────────────────────────
compile: $(BUILD_DIR) check-cc $(BINARIES)

$(BUILD_DIR)/%.exe: $(BUILD_DIR)/%.c
	@echo "  Build $* ..."
	@$(CC) -o "$@" "$<" $(CFLAGS) 2>&1 && echo "  ✓ $*" || echo "  FAILED $*"

# ── Run tests ────────────────────────────────────────────────────────────────
test: $(BUILD_DIR)
	@$(PYTHON) $(KL_DIR)/test_runner.py

test-verbose: $(BUILD_DIR)
	@$(PYTHON) $(KL_DIR)/test_runner.py --emit

# ── Run a specific .kl file ─────────────────────────────────────────────────
run-%: $(KL_DIR)/%.kl
	@$(KATLANS) emit "$<" > $(BUILD_DIR)/$*.c 2>&1 && \
	  echo "  ✓ Emitted $*" || { echo "  FAILED $*"; exit 1; }
	@$(CC) -o $(BUILD_DIR)/$* $(BUILD_DIR)/$*.c $(CFLAGS) 2>&1 && \
	  echo "  ✓ Built $*" || { echo "  Compile FAILED"; exit 1; }
	@echo "  Output:"
	@$(BUILD_DIR)/$*

# ── Clean ────────────────────────────────────────────────────────────────────
clean:
	rm -rf $(BUILD_DIR)
	find files/ -name '*.pyc' -delete
	find files/ -name '__pycache__' -type d -exec rm -rf {} + 2>/dev/null || true
	find files/ -name '*.egg-info' -type d -exec rm -rf {} + 2>/dev/null || true
	rm -rf files/dist/

# ── Package ──────────────────────────────────────────────────────────────────
sdist:
	$(PYTHON) -m build --sdist

install:
	$(PYTHON) -m pip install -e .
