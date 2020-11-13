/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mini_paint.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: youlee <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/13 16:37:29 by youlee            #+#    #+#             */
/*   Updated: 2020/11/13 17:41:20 by youlee           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

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
	float r;
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
	return (zone->width > 0 && zone->width <= 300 &&
			zone->height > 0 && zone->height <= 300);
}

int check_shape(t_shape *shape)
{
	return (shape->r > 0.0 && (shape->type == 'c' || shape->type == 'C'));
}

int	get_zone (FILE *file, t_zone *zone)
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

char *background_paint(t_zone *zone)
{
	char *temp;
	int i;

	i = 0;
	if (!(temp = (char*)malloc(sizeof(*temp) * (zone->width * zone->height))))
		return (NULL);
	while (i < (zone->height * zone->width))
		temp[i++] = zone->back;
	return (temp);
}

int in_circle(float x, float y, t_shape *shape)
{
	float dist;

	dist = sqrt(powf(x - shape->x, 2.) + powf(y - shape->y, 2.));
	
	if (dist <= shape->r)
	{
		if ((shape->r - dist) < 1.0)
			return (2);
		return (1);
	}
	return (0);
}

void draw_shape(t_shape *shape, t_zone *zone, char *drawing)
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
			ret = in_circle((float)x, (float)y, shape);
			if ((ret == 2 && shape->type == 'c') ||
					(ret && shape->type == 'C'))
				drawing[(y * zone->width) + x] = shape->color;
			x++;
		}
		y++;
	}
}

int draw_shapes(FILE *file, t_zone *zone, char *drawing)
{
	t_shape shape;
	int scan_ret;

	while ((scan_ret = fscanf(file, "%c %f %f %f %c\n",
					&shape.type,
					&shape.x,
					&shape.y,
					&shape.r,
					&shape.color)) == 5)
	{
		if (!check_shape(&shape))
			return (0);
		draw_shape(&shape, zone, drawing);
	}
	if (scan_ret != -1)
		return (0);
	return (1);
}

void draw_drawing(char *drawing, t_zone *zone)
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
	char *drawing;
	t_zone zone;

	if (argc != 2)
		return (str_error("Error: argument\n", 1));
	if (!(file = fopen(argv[1], "r")))
		return (str_error("Error: Operation file corrupted\n", 1));
	if (!get_zone(file, &zone))
		return (clear_all(file, NULL) && str_error("Error: Operation file corrupted\n", 1));
	if (!(drawing = background_paint(&zone)))
		return (clear_all(file, NULL) && str_error("Error: Operation file corrupted\n", 1));
	if (!draw_shapes(file, &zone, drawing))
		return (clear_all(file, drawing) && str_error("Error: Operation file corrupted\n", 1));
	draw_drawing(drawing, &zone);
	clear_all(file, drawing);
	return (0);
}
