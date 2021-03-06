/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   asm_read_lines.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jucapik <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/04/15 13:38:19 by jucapik           #+#    #+#             */
/*   Updated: 2019/05/07 11:45:42 by jucapik          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "asm.h"

static int		get_onet_helper(char *line, int *i)
{
	int		j;

	j = 0;
	if (line[*i + j] == DIRECT_CHAR || line[*i + j] == COMMENT_CHAR
			|| line[*i + j] == COMMENT_CHAR2)
		++j;
	while (line[*i + j] != ' ' && line[*i + j] != '\t'
			&& line[*i + j] != SEPARATOR_CHAR && line[*i + j] != '\0'
			&& line[*i + j] != DIRECT_CHAR && line[*i + j] != COMMENT_CHAR
			&& line[*i + j] != COMMENT_CHAR2)
		++j;
	return (j);
}

static t_onet	*get_onet(char *line, int *i)
{
	int		j;
	t_onet	*onet;

	if ((onet = create_onet()) == NULL)
		return (NULL);
	while ((line[*i] == ' ' || line[*i] == '\t') && line[*i] != '\0')
		++(*i);
	if (line[*i] == '\0' || line[*i] == COMMENT_CHAR
			|| line[*i] == COMMENT_CHAR2)
	{
		free_onet(onet);
		return (NULL);
	}
	j = get_onet_helper(line, i);
	onet->str = ft_strndup(line + *i, j);
	*i += j;
	while ((line[*i] == ' ' || line[*i] == '\t') && line[*i] != '\0')
		++(*i);
	onet->sep_after = (line[*i] == SEPARATOR_CHAR) ? true : false;
	*i += ((line[*i] == SEPARATOR_CHAR) ? 1 : 0);
	return (onet);
}

static t_tokens	*tokenize(char *line)
{
	int			i;
	t_onet		*onet;
	t_tokens	*tok;

	i = 0;
	onet = NULL;
	if ((tok = create_tokens()) == NULL)
		return (NULL);
	if ((tok->allt = get_onet(line, &i)) == NULL)
	{
		free_tokens(tok);
		return (NULL);
	}
	onet = tok->allt;
	while (line[i] != '\0' && onet != NULL)
	{
		onet->next = get_onet(line, &i);
		onet = onet->next;
	}
	return (tok);
}

static t_tokens	*get_base(t_data *data)
{
	char		*line;
	t_tokens	*base;
	int			res;

	line = NULL;
	base = NULL;
	while (base == NULL
			&& (res = get_next_line(data->code_file_fd, &line)) > 0)
	{
		if ((base = tokenize(line)) != NULL)
		{
			if (parsing(base, data->line_nb) == error)
			{
				free_tokens(base);
				ft_strdel(&line);
				return (NULL);
			}
		}
		ft_strdel(&line);
		++(data->line_nb);
	}
	return ((res < 0) ? gnl_error_null(base) : base);
}

t_tokens		*read_lines(t_data *data)
{
	char		*line;
	t_tokens	*base;
	t_tokens	*cur;
	int			res;

	line = NULL;
	if ((base = get_base(data)) == NULL)
		return (NULL);
	cur = base;
	while ((res = get_next_line(data->code_file_fd, &line)) > 0)
	{
		if ((cur->next = tokenize(line)) != NULL)
		{
			if (parsing(cur->next, data->line_nb) == error)
			{
				free_tokens(base);
				ft_strdel(&line);
				return (NULL);
			}
			cur = cur->next;
		}
		ft_strdel(&line);
		++(data->line_nb);
	}
	return ((res < 0) ? gnl_error_null(base) : base);
}
