/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mini_paint.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: youlee <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/13 15:15:02 by youlee            #+#    #+#             */
/*   Updated: 2020/11/13 17:41:21 by youlee           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

typedef struct	s_zone
{
	int width;
	int height;
	char back;
}				t_zone;

typedef struct	s_shape
{
	char type;
	float x;
	float y;
	float width;
	float height;
	char color;
}				t_shape;

int ft_strlen(char *str)
{
	int len;

	len = 0;
	while (str[len])
		len++;
	return (len);
}

int str_error(char *str, int ret)
{
	write(1, str, ft_strlen(str));
	return (ret);
}

int clear_all(FILE *file, char *drawing)
{
	if (file)
		fclose(file);
	if (drawing)
		free(drawing);
	return (1);
}

int check_zone(t_zone *zone)
{
	return(zone->height > 0 && zone->height <= 300 && zone->width > 0 && zone->width <= 300);
}

int check_shape(t_shape *shape)
{
	return (shape->width > 0.00000000 && shape->height > 0.00000000 && 
			(shape->type == 'r' || shape->type =='R'));
}

int	get_zone(FILE *file, t_zone *zone)
{
	int scan_ret;

	if ((scan_ret = fscanf(file, "%d %d %c\n", &zone->width, &zone->height, &zone->back)) != 3)
		return (0);
	if (!check_zone(zone))
		return (0);
	if (scan_ret == -1)
		return (0);
	return (1);
}

char	*background_paint(t_zone *zone)
{
	int i;
	char *temp;

	if (!(temp = (char*)malloc(sizeof(*temp) * (zone->width * zone->height))))
		return (NULL);
	i = 0;
	while (i < zone->width * zone->height)
		temp[i++] = zone->back;
	return (temp);
}

int	in_rectangle(float x, float y, t_shape *shape)
{
	if (((x < shape->x || (shape->x + shape->width < x)) ||
				(y < shape->y)) || (shape->y + shape->height < y))
		return (0);
	if (((x - shape->x < 1.00000000) || ((shape->x + shape->width) - x < 1.00000000)) || 
			((y - shape->y < 1.00000000) || ((shape->y + shape->height) - y < 1.00000000)))
		return (2);
	return (1);
}

void draw_shape(char **drawing, t_shape *shape, t_zone *zone)
{
	int x;
	int y;
	int ret;
	
	y = 0;
	while (y < zone->height)
	{
		x = 0;
		while (x < zone->width)
		{
			ret = in_rectangle(x, y, shape);
			if ((ret == 2 && shape->type == 'r') || 
					(ret && shape->type == 'R'))
				(*drawing)[(y * zone->width) + x] = shape->color;
			x++;
		}
		y++;
	}
}

int draw_shapes(FILE *file, char **drawing, t_zone *zone)
{
	t_shape shape;
	int ret;
	
	while ((ret = fscanf(file, "%c %f %f %f %f %c\n", 
					&shape.type,
					&shape.x,
					&shape.y,
					&shape.width,
					&shape.height,
					&shape.color)) == 6)
	{
		if (!check_shape(&shape))
			return (0);
		draw_shape(drawing, &shape, zone);
	}
	if (ret != -1)
		return (0);
	return (1);
}

void	draw_drawing(char *drawing ,t_zone *zone)
{
	int i;

	i = 0;
	while (i < zone->height)
	{
		write(1, drawing + (i * zone->width), zone->width);
		write(1, "\n", 1);
		i++;
	}
}

int main(int argc, char **argv)
{
	FILE *file;
	t_zone zone;
	char *drawing;

	zone.width = 0;
	zone.height = 0;
	zone.back = 0;

	if (argc != 2)
		return (str_error("Error: argument\n", 1));
	if (!(file = fopen(argv[1], "r")))
		return (str_error("Error: Operation file corrupted\n", 1));
	if (!get_zone(file, &zone))
		return (clear_all(file, NULL) && str_error("Error: Operation file corrupted\n", 1));
	if (!(drawing = background_paint(&zone)))
		return (clear_all(file, NULL) && str_error("Error: Operation file corrupted\n", 1));
	if (!draw_shapes(file, &drawing, &zone))
		return (clear_all(file, NULL) && str_error("Error: Operation file corrupted\n", 1));
	draw_drawing(drawing, &zone);
	clear_all(file, drawing);
	return (0);
}
