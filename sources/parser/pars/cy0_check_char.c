#include "parser_new.h"
#include "libftfull.h"

int	analyse_char(char c)
{
	if (c == '>')
		return (-4);
	if (c == '<')
		return (-5);
	if (c == '|')
		return (-6);
	if (c == '$')
		return (-7);
	if (c == '=')
		return (-8);
	if (c == '-')
		return (-9);
	if (c == '/')
		return (-10);
	if (c == '~')
		return (-11);
	if (c == '.')
		return (-12);
	if (c == 0)
		return (0);
	return (-13);
}

int	cy0_analyse_char(char c)
{
	if ((c >= '\t' && c <= '\r') || c == ' ')
		return (-1);
	if (c == '?')
		return (-14);
	if (c == '\'')
		return (-2);
	if (c == '"')
		return (-3);
	return (analyse_char(c));
}

int	cy0_analyse_char2(char c)
{
	if ((c >= '\t' && c <= '\r') || c == ' ')
		return (-1);
	if (c == '#' || c == '`' || c == '\\' || c == '*' || c == '!' || c == '&'
		|| c == '[' || c == ']' || c == '{' || c == '}' || c == '(' || c == ')'
		|| c == '+' || c == '%' || c == ';' || c == '@' || c == ':')
		return (1);
	if (c == '?')
		return (-14);
	if (c == '\'')
		return (-2);
	if (c == '"')
		return (-3);
	return (analyse_char(c));
}
