NAME       = libcvault.a
CC         = cc
CFLAGS     = -std=gnu11 -Wall -Wextra -Werror -Wpedantic \
             -fstack-protector-strong \
             -fPIC \
             -D_FORTIFY_SOURCE=2 \
             -O2 -Iinclude

SRC_DIR    = src
OBJ_DIR    = obj
LIB_DIR    = lib

SRC        = $(shell find $(SRC_DIR) -name "*.c")
OBJ        = $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

all: $(LIB_DIR)/$(NAME)

$(LIB_DIR)/$(NAME): $(OBJ)
	@mkdir -p $(LIB_DIR)
	ar rcs $(LIB_DIR)/$(NAME) $(OBJ)
	@echo "==> [Cvault] library created. [$(LIB_DIR)/$(NAME)]"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)
	@echo "==> [Cvault] Cleared object files."

fclean: clean
	rm -rf $(LIB_DIR)
	@echo "==> [Cvault] Cleared library archives."

re: fclean all

.PHONY: all clean fclean re