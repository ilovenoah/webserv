CXX			=	c++
CXXFLAGS	=	-Wall -Werror -Wextra -std=c++98
RM			=	rm -rf
AR			=	ar rcs
NAME		=	webserv

INC			=	-I inc/

SRC_DIR		=	src/
CONF_DIR	=	conf/
UTILS_DIR	=	utils/
OBJ_DIR		=	objs/

MAIN_NAME	=	main.cpp
CONF_NAME	=	Config.cpp Location.cpp Server.cpp
UTILS_NAME	=	Error.cpp

OBJ_NAME	=	$(MAIN_NAME:.cpp=.o)
OBJ_NAME	+=	$(addprefix $(CONF_DIR), $(CONF_NAME:.cpp=.o))
OBJ_NAME	+=	$(addprefix $(UTILS_DIR), $(UTILS_NAME:.cpp=.o))

OBJ			=	$(addprefix $(OBJ_DIR), $(OBJ_NAME))

all: mkdir $(NAME)

$(NAME): $(OBJ)
	@$(CXX) -o $@ $^
	@echo "##### $@ compiling finished! #####"

$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp
	@$(CXX) $(CXXFLAGS) $(INC) -o $@ -c $^
	@echo "##### Creating" [ $@ ] " #####"

mkdir:
	@mkdir -p $(OBJ_DIR)$(CONF_DIR)
	@mkdir -p $(OBJ_DIR)$(UTILS_DIR)
	@echo "##### Creating obj directory #####"

clean:
	@$(RM) $(OBJ_DIR)
	@echo "##### Removed object files #####"

fclean: clean
	@$(RM) $(NAME)
	@echo "##### Removed binary files #####"

re: fclean all

.PHONY: all mkdir clean fclean re
