CXX			=	c++
CXXFLAGS	=	-Wall -Werror -Wextra -std=c++98
RM			=	rm -rf
AR			=	ar rcs
NAME		=	webserv

INC			=	-I inc/

SRC_DIR		=	src/
OBJ_DIR		=	objs/

BUILT_NAME	=	main.cpp

OBJ_NAME	=	$(addprefix $(BUILT_DIR), $(BUILT_NAME:.cpp=.o))
OBJ_NAME	+=	$(addprefix $(EXEC_DIR), $(EXEC_NAME:.cpp=.o))
OBJ			=	$(addprefix $(OBJ_DIR), $(OBJ_NAME))

all: mkdir $(NAME)

$(NAME): $(OBJ)
	@$(CXX) -o $@ $^
	@echo "##### $@ compiling finished! #####"

$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp
	@$(CXX) $(CXXFLAGS) $(INC) -o $@ -c $^
	@echo "##### Creating" [ $@ ] " #####"

mkdir:
	@mkdir -p $(OBJ_DIR)$(BUILT_DIR)
	@echo "##### Creating obj directory #####"

clean:
	@$(RM) $(OBJ_DIR)
	@echo "##### Removed object files #####"

fclean: clean
	@$(RM) $(NAME)
	@echo "##### Removed binary files #####"

re: fclean all

.PHONY: all mkdir clean fclean re
