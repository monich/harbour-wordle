.PHONY: all clean words

SRC_DIR = $(shell pwd)
DICT_DIR = $(shell dirname $(SRC_DIR))
TOP_DIR = $(shell dirname $(DICT_DIR))

ENC ?= windows-1250
LANG = $(shell basename $(SRC_DIR))

OUT_DIR = $(TOP_DIR)/data/$(LANG)
TOOL_DIR = $(TOP_DIR)/tools
WORDLE_TOOL = $(TOOL_DIR)/wordle-tool

WORDS_TXT = $(SRC_DIR)/words.txt
WORDS = $(OUT_DIR)/words

all: words

clean:
	@rm *~

words: $(WORDLE_TOOL)
	@$(WORDLE_TOOL) -C $(OUT_DIR) $(V) $(WORDS_TXT) $(ENC)

$(WORDLE_TOOL): $(WORDLE_TOOL).c
	@$(MAKE) -C $(TOOL_DIR) wordle-tool
