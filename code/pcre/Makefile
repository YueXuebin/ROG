# Makefile for pcre
PROJECT_NAME = pcre

# macros
COMPILING_FLAG = $(CXX_ADDITION_FLAGS) -DHAVE_CONFIG_H
LINK_FLAG = -r

# project directory
ifndef $(OBJ_ROOT)
OBJ_ROOT = ../obj/
endif

OBJ_DIR =  $(OBJ_ROOT)$(PROJECT_NAME)
TARGET_DIR = ../lib
TARGET = $(TARGET_DIR)/lib$(PROJECT_NAME).a

SRC_DIR = .
SRC = $(wildcard $(SRC_DIR)/*.c)
DEPS = $(addprefix $(OBJ_DIR)/, $(patsubst %cpp, %d, $(filter %c, $(notdir $(SRC)))))
OBJS = $(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(basename $(notdir $(SRC)))))

# includ and lib directory
INCLUDE_DIRS =
LIBARAY_DIRS =
LIBARAYS = 
All_INCLUDE_DIRS = $(addprefix -I,$(INCLUDE_DIRS))
ALL_LIBARAY_DIRS = $(addprefix -L,$(LIBARAY_DIRS))
ALL_LIBARAYS = $(addprefix -l,$(LIBARAYS))

# rule all
all : $(TARGET)
	@echo build $(PROJECT_NAME) End!

# for target
$(TARGET) : $(OBJS)
	@echo linking $(TARGET)
	@$(AR) $(LINK_FLAG) -o $@ $(OBJS) $(ALL_LIBARAY_DIRS) $(ALL_LIBARAYS)

# for .o
$(OBJ_DIR)/%.o : $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(TARGET_DIR)
	@echo building $<
	@$(CXX) $(COMPILING_FLAG) $(All_INCLUDE_DIRS) -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c $< -o $@

# clean
clean:
	@echo clean $(PROJECT_NAME)
	@rm -f $(TARGET) $(OBJS) $(DEPS)
.PHONY: clean

# include rules
-include $(DEPS)

# help
help:
	@echo $(DEPS)
.PHONY: help
