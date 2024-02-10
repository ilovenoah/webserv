CXX			=	c++
CXXFLAGS	=	-Wall -Werror -Wextra -std=c++98 -MMD -MP
RM			=	rm -rf
AR			=	ar rcs
NAME		=	webserv
UNAME_OS	=	$(shell uname -s)

INC			=	-I inc/

SRC_DIR		=	src/
CONF_DIR	=	conf/
UTILS_DIR	=	utils/
SOCK_DIR	=	sockets/
OBJ_DIR		=	objs/
LOOP_DIR	=	loop/
REQU_DIR	=	request/
RESP_DIR	=	response/

MAIN_NAME	=	main.cpp
CONF_NAME	=	Config.cpp Server.cpp AConfigurable.cpp Location.cpp
SOCK_NAME	=	ServerSocket.cpp ClientSocket.cpp
UTILS_NAME	=	utils.cpp CaseInsensitiveCompare.cpp
LOOP_NAME	=	loop.cpp
REQU_NAME	=	Request.cpp
RESP_NAME	=	Response.cpp

OBJ_NAME	=	$(MAIN_NAME:.cpp=.o)
OBJ_NAME	+=	$(addprefix $(CONF_DIR), $(CONF_NAME:.cpp=.o))
OBJ_NAME	+=	$(addprefix $(UTILS_DIR), $(UTILS_NAME:.cpp=.o))
OBJ_NAME	+=	$(addprefix $(LOOP_DIR), $(LOOP_NAME:.cpp=.o))
OBJ_NAME	+=	$(addprefix $(SOCK_DIR), $(SOCK_NAME:.cpp=.o))
OBJ_NAME	+=	$(addprefix $(REQU_DIR), $(REQU_NAME:.cpp=.o))
OBJ_NAME	+=	$(addprefix $(RESP_DIR), $(RESP_NAME:.cpp=.o))
OBJ			=	$(addprefix $(OBJ_DIR), $(OBJ_NAME))
DEPS		=	$(OBJ:.o=.d)

ifeq ($(UNAME_OS), Linux)
CXXFLAGS 			+= -D_LINUX
else ifeq ($(UNAME_OS), Darwin)
CXXFLAGS 			+= -D_DARWIN
endif

ifeq ($(MAKECMDGOALS), debug)
CXXFLAGS 			+= -D_DEBUG
CXXFLAGS 			+= -fsanitize=address
CXXFLAGS 			+= -g
endif

all: mkdir $(NAME)

$(NAME): $(OBJ)
	@$(CXX) $(CXXFLAGS) -o $@ $^
	@echo "##### $@ compiling finished! #####"

$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp
	@$(CXX) $(CXXFLAGS) $(INC) -o $@ -c $<
	@echo "##### Creating" [ $@ ] " #####"

mkdir:
	@if [ ! -d "$(OBJ_DIR)$(CONF_DIR)" ]; then \
		mkdir -p $(OBJ_DIR)$(CONF_DIR) && \
		echo "##### Creating $(OBJ_DIR)$(CONF_DIR) directory #####"; \
	fi
	@if [ ! -d "$(OBJ_DIR)$(UTILS_DIR)" ]; then \
		mkdir -p $(OBJ_DIR)$(UTILS_DIR) && \
		echo "##### Creating $(OBJ_DIR)$(UTILS_DIR) directory #####"; \
	fi
	@if [ ! -d "$(OBJ_DIR)$(SOCK_DIR)" ]; then \
		mkdir -p $(OBJ_DIR)$(SOCK_DIR) && \
		echo "##### Creating $(OBJ_DIR)$(SOCK_DIR) directory #####"; \
	fi
	@if [ ! -d "$(OBJ_DIR)$(LOOP_DIR)" ]; then \
		mkdir -p $(OBJ_DIR)$(LOOP_DIR) && \
		echo "##### Creating $(OBJ_DIR)$(LOOP_DIR) directory #####"; \
	fi
	@if [ ! -d "$(OBJ_DIR)$(REQU_DIR)" ]; then \
		mkdir -p $(OBJ_DIR)$(REQU_DIR) && \
		echo "##### Creating $(OBJ_DIR)$(REQU_DIR) directory #####"; \
	fi
	@if [ ! -d "$(OBJ_DIR)$(RESP_DIR)" ]; then \
		mkdir -p $(OBJ_DIR)$(RESP_DIR) && \
		echo "##### Creating $(OBJ_DIR)$(RESP_DIR) directory #####"; \
	fi

-include $(DEPS)

clean:
	@$(RM) $(OBJ_DIR)
	@echo "##### Removed object files #####"

fclean: clean
	@$(RM) $(NAME)
	@echo "##### Removed binary files #####"

re: fclean all

debug: all


.PHONY: all mkdir clean fclean re
